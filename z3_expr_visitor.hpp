/* metaSMT SMT2 Evaluator
 * Copyright (C) 2015  German Aerospace Center (DLR, e.V.)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file consistency_checker.hpp
 *
 * @brief pretty printing z3 expressions
 *
 * @author Heinz Riener
 * @since  1.0
 */

#include "z3_utils.hpp"

#include <metaSMT/support/default_visitation_unrolling_limit.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/frontend/Logic.hpp>
#include <metaSMT/frontend/QF_BV.hpp>

#include <z3++.h>

#include <map>
#include <iostream>

#pragma once

template < typename Solver >
class result_type_generator
{
public:
  using result_type = typename Solver::result_type;

  result_type_generator( Solver& solver )
    : solver( solver )
  {}

  virtual ~result_type_generator() {}

  result_type convert_constant_or_variable( const z3::expr& e )
  {
    using namespace metaSMT;
    using namespace metaSMT::logic;
    using namespace metaSMT::logic::QF_BV;

    const z3::func_decl& decl = e.decl();
    assert( decl.arity() == 0u && "Expression is not a constant or variable" );

    /*** debug ***/
    // const std::string name = decl.name().str();
    // std::cout << "CONVERT CONST/VAR:" << name << '\n';

    const Z3_decl_kind& decl_kind = decl.decl_kind();

    result_type r;
    switch( decl_kind )
    {
    case Z3_OP_TRUE:
      r = evaluate( solver, True );
      break;
    case Z3_OP_FALSE:
      r = evaluate( solver, False );
      break;
    case Z3_OP_BNUM:
      {
        const std::string value = expr_to_bin( e );
        r = evaluate( solver, bvbin( value ) );
      }
      break;
    case 0x92d:
      {
        /* variable */
        const z3::sort& sort = decl.range();
        const Z3_sort_kind& sort_kind = sort.sort_kind();
        if ( sort_kind == Z3_BOOL_SORT )
        {
          r = evaluate( solver, new_variable() );
        }
        else if ( sort_kind == Z3_BV_SORT )
        {
          const unsigned w = decl.range().bv_size();
          r = evaluate( solver, new_bitvector( w ) );
        }
        else if ( sort_kind == Z3_INT_SORT )
        {
          assert( false && "Integer variables are not supported by metaSMT" );
        }
        else
        {
          assert( false && "Unsupported range type" );
        }
        break;
      }
    default:
      std::cerr << "[e] convert_constant_or_variable: does not support expression of type " << decl.name().str() << " [ " << std::hex << decl_kind << "]\n";
      assert( false );
    }

    /*** update map ***/
    const unsigned id = z3_expr_id( e );
    the_map.insert( std::make_pair(id,r) );
    return r;
  }

  result_type convert_operator( const z3::expr& e )
  {
    using namespace metaSMT;
    using namespace metaSMT::logic;
    using namespace metaSMT::logic::QF_BV;

    const z3::func_decl& decl = e.decl();
    assert( decl.arity() > 0 && "Expression is not an operator" );

    /*** debug ***/
    // const std::string name = decl.name().str();
    // std::cout << "CONVERT OP:" << name << '\n';

    const Z3_decl_kind& decl_kind = decl.decl_kind();

    result_type r;
    switch( decl_kind )
    {
    case Z3_OP_EQ:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, logic::equal( lhs, rhs ) );
      }
      break;
    case Z3_OP_DISTINCT:
      {
        if( e.num_args() == 2u )
        {
          const result_type lhs = operator()( e.arg( 0u ) );
          const result_type rhs = operator()( e.arg( 1u ) );
          r = evaluate( solver, Not( logic::equal( lhs, rhs ) ) );
        }
        else
        {
          std::vector< result_type > c;
          for ( unsigned n = 0u; n < e.num_args(); ++n )
          {
            c.push_back( operator()( e.arg( n ) ) );
          }
          r = evaluate( solver, logic::True );
          for ( unsigned i = 0u; i < e.num_args(); ++i )
          {
            for ( unsigned j = i+1; j < e.num_args(); ++j )
            {
              r = evaluate( solver, And( r, logic::nequal( c[i], c[j] ) ) );
            }
          }
        }
      }
      break;
    case Z3_OP_ITE:
      {
        assert( e.num_args() == 3u );
        const result_type cond = operator()( e.arg( 0u ) );
        const result_type x = operator()( e.arg( 1u ) );
        const result_type y = operator()( e.arg( 2u ) );
        r = evaluate( solver, Ite( cond, x, y ) );
      }
      break;
    case Z3_OP_AND:
      {
        const unsigned size = e.num_args();
        result_type compound = operator()( e.arg( size-1u ) );
        for ( unsigned i = 1u; i < size; ++i )
        {
          const result_type arg = operator()( e.arg( size-1u-i ) );
          compound = evaluate( solver, And( arg, compound ) );
        }
        r = compound;
      }
      break;
    case Z3_OP_OR:
      {
        const unsigned size = e.num_args();
        result_type compound = operator()( e.arg( size-1u ) );
        for ( unsigned i = 1u; i < size; ++i )
        {
          const result_type arg = operator()( e.arg( size-1u-i ) );
          compound = evaluate( solver, Or( arg, compound ) );
        }
        r = compound;
      }
      break;
    case Z3_OP_IFF:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, logic::equal( lhs, rhs ) );
      }
      break;
    case Z3_OP_XOR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, Xor( lhs, rhs ) );
      }
      break;
    case Z3_OP_NOT:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        r = evaluate( solver, Not( arg ) );
      }
      break;
    case Z3_OP_IMPLIES:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, implies( lhs, rhs ) );
      }
      break;
    case Z3_OP_BNEG:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        r = evaluate( solver, bvneg( arg ) );
      }
      break;
    case Z3_OP_BADD:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvadd( lhs, rhs ) );
      }
      break;
    case Z3_OP_BSUB:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvsub( lhs, rhs ) );
      }
      break;
    case Z3_OP_BMUL:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvmul( lhs, rhs ) );
      }
      break;
    case Z3_OP_BSDIV:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvsdiv( lhs, rhs ) );
      }
      break;
    case Z3_OP_BUDIV:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvudiv( lhs, rhs ) );
      }
      break;
    case Z3_OP_BSMOD:
    case Z3_OP_BSREM:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvsrem( lhs, rhs ) );
      }
      break;
    case Z3_OP_BUREM:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvurem( lhs, rhs ) );
      }
      break;
    case Z3_OP_BSDIV0:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BSDIV0" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_BUDIV0:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BUDIV0" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_BSREM0:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BSREM0" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_BUREM0:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BUREM0" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_BSMOD0:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BSMOD0" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_ULEQ:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvule( lhs, rhs ) );
      }
      break;
    case Z3_OP_SLEQ:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvsle( lhs, rhs ) );
      }
      break;
    case Z3_OP_UGEQ:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvuge( lhs, rhs ) );
      }
      break;
    case Z3_OP_SGEQ:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvsge( lhs, rhs ) );
      }
      break;
    case Z3_OP_ULT:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvult( lhs, rhs ) );
      }
      break;
    case Z3_OP_SLT:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvslt( lhs, rhs ) );
      }
      break;
    case Z3_OP_UGT:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvugt( lhs, rhs ) );
      }
      break;
    case Z3_OP_SGT:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvsgt( lhs, rhs ) );
      }
      break;
    case Z3_OP_BAND:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvand( lhs, rhs ) );
      }
      break;
    case Z3_OP_BOR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvor( lhs, rhs ) );
      }
      break;
    case Z3_OP_BNOT:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        r = evaluate( solver, bvnot( arg ) );
      }
      break;
    case Z3_OP_BXOR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvxor( lhs, rhs ) );
      }
      break;
    case Z3_OP_BNAND:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvnand( lhs, rhs ) );
      }
      break;
    case Z3_OP_BNOR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvnor( lhs, rhs ) );
      }
      break;
    case Z3_OP_BXNOR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvxnor( lhs, rhs ) );
      }
      break;
    case Z3_OP_CONCAT:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, concat( lhs, rhs ) );
      }
      break;
    case Z3_OP_SIGN_EXT:
      {
        assert( e.num_args() == 1u );
        const unsigned result_size = decl.range().bv_size();
        const unsigned arg_size = e.arg( 0 ).decl().range().bv_size();
        const result_type arg = operator()( e.arg( 0u ) );
        r = evaluate( solver, sign_extend( result_size - arg_size, arg ) );
      }
      break;
    case Z3_OP_ZERO_EXT:
      {
        assert( e.num_args() == 1u );
        const unsigned result_size = decl.range().bv_size();
        const unsigned arg_size = e.arg( 0 ).decl().range().bv_size();
        const result_type arg = operator()( e.arg( 0u ) );
        r = evaluate( solver, zero_extend( result_size - arg_size, arg ) );
      }
      break;
    case Z3_OP_EXTRACT:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        r = evaluate( solver, extract( hi( e ), lo( e ), arg ) );
      }
      break;
    case Z3_OP_REPEAT:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        r = arg;
        const unsigned how_many = decl_int_parameter( e, 0u );
        for ( unsigned i = 1u; i < how_many; ++i )
        {
          r = evaluate( solver, concat(r, arg) );
        }
      }
      break;
    case Z3_OP_BREDOR:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BREDOR" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_BREDAND:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_BREDAND" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_BCOMP:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvcomp( lhs, rhs ) );
      }
      break;
    case Z3_OP_BSHL:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvshl( lhs, rhs ) );
      }
      break;
    case Z3_OP_BLSHR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvshr( lhs, rhs ) );
      }
      break;
    case Z3_OP_BASHR:
      {
        assert( e.num_args() == 2u );
        const result_type lhs = operator()( e.arg( 0u ) );
        const result_type rhs = operator()( e.arg( 1u ) );
        r = evaluate( solver, bvashr( lhs, rhs ) );
      }
      break;
    case Z3_OP_ROTATE_LEFT:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        const unsigned n = decl_int_parameter( e, 0u );
        if ( n > 0u )
        {
          const unsigned size = decl.range().bv_size();
          const result_type lower = evaluate( solver, extract( size-1u-n, 0u, arg ) );
          const result_type higher = evaluate( solver, extract( size-1u, size-n, arg ) );
          r = evaluate( solver, concat( lower, higher ) );
        }
        else
        {
          r = evaluate( solver, arg );
        }
      }
      break;
    case Z3_OP_ROTATE_RIGHT:
      {
        assert( e.num_args() == 1u );
        const result_type arg = operator()( e.arg( 0u ) );
        const unsigned n = decl_int_parameter( e, 0u );
        if ( n > 0u )
        {
          const unsigned size = decl.range().bv_size();
          const result_type lower = evaluate( solver, extract( n-1u, 0u, arg ) );
          const result_type higher = evaluate( solver, extract( size-1u, n, arg ) );
          r = evaluate( solver, concat( lower, higher ) );
        }
        else
        {
          r = evaluate( solver, arg );
        }
      }
      break;
    case Z3_OP_EXT_ROTATE_LEFT:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_EXT_ROTATE_LEFT" << '\n';
        assert( false );
      }
      break;
    case Z3_OP_EXT_ROTATE_RIGHT:
      {
        std::cout << "NOT IMPLEMENTED: Z3_OP_EXT_ROTATE_RIGHT" << '\n';
        assert( false );
      }
      break;
    default:
      std::cerr << "[e] convert_operator: does not support expression of type " << decl.name().str() << " [ " << std::hex << decl_kind << "]\n";
      break;
    }

    /*** update map ***/
    const unsigned id = z3_expr_id( e );
    the_map.insert( std::make_pair(id,r) );
    return r;
  }

  result_type operator()( const z3::expr& e )
  {
    using namespace metaSMT;
    using namespace metaSMT::logic;
    using namespace metaSMT::logic::QF_BV;

    const unsigned id = z3_expr_id( e );
    auto it = the_map.find( id );
    if ( it != the_map.end() )
    {
      return it->second;
    }

    if ( e.is_app() )
    {
      /*** Application ***/
      const unsigned size = e.num_args();
      if ( size == 0u )
      {
        return convert_constant_or_variable( e );
      }
      else
      {
        for ( unsigned i = 0u; i < size; ++i )
        {
          this->operator()( e.arg( i ) );
        }
        return convert_operator( e );
      }
    }
    else if ( e.is_quantifier() )
    {
      /*** Quantifier ***/
      assert( false && "metaSMT does not support quantifiers" );
    }
    else
    {
      assert( false && "Yet not implemented. Please report a bug to the developers." );
    }
    return evaluate( solver, False );
  }

protected:
  Solver& solver;
  std::map< unsigned, result_type > the_map;
}; /* z3_expr_visitor */

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
