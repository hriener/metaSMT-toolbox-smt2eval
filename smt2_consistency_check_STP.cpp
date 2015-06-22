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

#include "consistency_check.hpp"
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/STP.hpp>

using Solver = metaSMT::DirectSolver_Context< metaSMT::solver::STP >;

int main( int argc, char *argv[] )
{
  return metaSMT_Z3_consistency_checker_main< Solver >( argc, argv );
}

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
