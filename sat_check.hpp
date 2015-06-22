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
 * @file consistency_check.hpp
 *
 * @brief consistency checking between metaSMT and Z3 for SMT-LIB2 instances
 *
 * @author Heinz Riener
 * @since  1.0
 */

#include "z3_expr_visitor.hpp"

#pragma once

template < typename Solver >
int metaSMT_satisfiability_checker_main( int argc, char *argv[] )
{
  if ( argc != 2 )
  {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    return -1;
  }

  const std::string filename = argv[1];
  // std::cout << "Read SMT-LIB2 benchmark file ''" << filename << "''\n";

  /*** Parse SMT-LIB2 instance ***/
  z3::context ctx;
  const Z3_ast ast = Z3_parse_smtlib2_file( ctx, filename.c_str(), 0, 0, 0, 0, 0, 0 );

  /*** Convert to metaSMT result_type ***/
  Solver solver_ctx;
  result_type_generator< Solver > generator( solver_ctx );
  const z3::expr instance( ctx, ast );
  typename Solver::result_type r = generator( instance );

  /*** Check satisfiability utilizing metaSMT ***/
  metaSMT::assertion( solver_ctx, r );
  const bool metaSMT_sat = metaSMT::solve( solver_ctx );

  if ( metaSMT_sat )
  {
    return 1;
  }
  else if ( !metaSMT_sat )
  {
    return 0;
  }

  /* Error */
  return -1;
}

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
