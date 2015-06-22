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
 * @file z3_utils.hpp
 *
 * @brief
 *
 * @author Mathias Soeken
 *         Heinz Riener
 * @since  1.0
 */

#include <z3++.h>
#include <string>

const bool expr_to_bool( const z3::expr& e );
const std::string expr_to_bin( const z3::expr &e );

unsigned decl_num_parameters( const z3::expr& e );
unsigned decl_int_parameter( const z3::expr& e, const unsigned n );
unsigned hi( const z3::expr& e );
unsigned lo( const z3::expr& e );
unsigned z3_expr_id( const z3::expr& e );

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
