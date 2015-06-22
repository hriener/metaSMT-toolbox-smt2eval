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

#include "z3_utils.hpp"
#include "conversion_utils.hpp"

const bool expr_to_bool( const z3::expr& e )
{
  assert( e.decl().decl_kind() == Z3_OP_TRUE ||
          e.decl().decl_kind() == Z3_OP_FALSE );
  std::ostringstream ss; ss << e;
  assert( ss.str() == "true" || ss.str() == "false" );
  return ( ss.str() == "true" );
}

const std::string expr_to_bin( const z3::expr &e )
{
  assert( e.decl().decl_kind() == Z3_OP_BNUM );
  std::ostringstream ss; ss << e;
  std::string val = ss.str();
  if ( val[0] == '#' && val[1] == 'b' )
  {
    assert( val[0] == '#' && val[1] == 'b' );
    val = val.substr(2, val.size()-2);
    assert( val.size() == e.decl().range().bv_size() );
    return val;
  }
  else
  {
    assert ( val[0] == '#' && val[1] == 'x' );
    val = val.substr(2, val.size()-2);
    return convert_hex2bin( val );
  }
}

unsigned decl_num_parameters( const z3::expr& e )
{
  assert( e.is_app() );
  return static_cast<unsigned>( Z3_get_decl_num_parameters( e.ctx(), e.decl() ) );
}

unsigned decl_int_parameter( const z3::expr& e, const unsigned n )
{
  return static_cast<unsigned>(Z3_get_decl_int_parameter(e.ctx(), e.decl(), n));
}

unsigned hi( const z3::expr& e )
{
  assert( decl_num_parameters( e ) == 2 );
  return decl_int_parameter( e, 0 );
}

unsigned lo( const z3::expr& e )
{
  assert( decl_num_parameters( e ) == 2 );
  return decl_int_parameter( e, 1 );
}

unsigned z3_expr_id( const z3::expr& e )
{
  return Z3_get_ast_id( e.ctx(), e );
}

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
