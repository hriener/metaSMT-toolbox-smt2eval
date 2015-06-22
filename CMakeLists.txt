add_definitions(-std=c++11)

set(SOURCES conversion_utils.cpp z3_utils.cpp)

############################################################################
# consistency checker
############################################################################
add_tool_executable(
  smt2_consistency_check_SMT2
SOURCES
  smt2_consistency_check_SMT2.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND
)

add_tool_executable(
  smt2_consistency_check_Boolector
SOURCES
  smt2_consistency_check_Boolector.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND Boolector_FOUND
)

add_tool_executable(
  smt2_consistency_check_Z3
SOURCES
  smt2_consistency_check_Z3.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND
)

add_tool_executable(
  smt2_consistency_check_STP
SOURCES
  smt2_consistency_check_STP.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND STP_FOUND
)

add_tool_executable(
  smt2_consistency_check_CVC4
SOURCES
  smt2_consistency_check_CVC4.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND CVC4_FOUND
)

add_tool_executable(
  smt2_consistency_check_MiniSat
SOURCES
  smt2_consistency_check_MiniSat.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND MiniSat_FOUND
)

add_tool_executable(
  smt2_consistency_check_picosat
SOURCES
  smt2_consistency_check_picosat.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND PicoSAT_FOUND
)

add_tool_executable(
  smt2_consistency_check_lingeling
SOURCES
  smt2_consistency_check_lingeling.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND Lingeling_FOUND
)

############################################################################
# satisfiability checker
############################################################################

add_tool_executable(
  smt2_sat_check_SMT2
SOURCES
  smt2_sat_check_SMT2.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND
)

add_tool_executable(
  smt2_sat_check_Boolector
SOURCES
  smt2_sat_check_Boolector.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND Boolector_FOUND
)

add_tool_executable(
  smt2_sat_check_Z3
SOURCES
  smt2_sat_check_Z3.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND
)

add_tool_executable(
  smt2_sat_check_STP
SOURCES
  smt2_sat_check_STP.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND STP_FOUND
)

add_tool_executable(
  smt2_sat_check_CVC4
SOURCES
  smt2_sat_check_CVC4.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND CVC4_FOUND
)

add_tool_executable(
  smt2_sat_check_MiniSat
SOURCES
  smt2_sat_check_MiniSat.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND MiniSat_FOUND
)

add_tool_executable(
  smt2_sat_check_picosat
SOURCES
  smt2_sat_check_picosat.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND PicoSAT_FOUND
)

add_tool_executable(
  smt2_sat_check_lingeling
SOURCES
  smt2_sat_check_lingeling.cpp
  ${SOURCES}
REQUIRES
  Z3_FOUND Lingeling_FOUND
)

