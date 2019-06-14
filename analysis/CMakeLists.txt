cmake_minimum_required(VERSION 3.0)
project(flowanalysis VERSION 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug)

message(STATUS FEMTODST_HOME=${FEMTODST_HOME})
list(APPEND CMAKE_PREFIX_PATH ${FEMTODST_HOME})

link_directories(${FEMTODST_HOME})
add_executable(correct Correct.cpp CorrectionTask.cpp FemtoDstVarManager.cpp)
target_compile_definitions(correct PUBLIC _VANILLA_ROOT_)
target_include_directories(correct PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${FEMTODST_HOME})
target_link_libraries(correct QnCorrections Base Correction StFemtoDst ${ROOT_LIBRARIES})

add_executable(analysis Analysis.cpp CorrelationTask.cpp)
#target_compile_definitions(analysis PUBLIC "-DUSE_ROOT")
target_include_directories(analysis PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(analysis QnCorrections Base Correlation ${ROOT_LIBRARIES})

