cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0048 NEW)
project(flow VERSION 1)

add_subdirectory(ReducedEvent)

add_executable(Correct Correct.cpp CorrectionTask.cpp)
target_compile_definitions(Correct PUBLIC "-DUSE_ROOT")
target_include_directories(Correct PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(Correct PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Correct QnCorrections ReducedEvent Base Correction ${ROOT_LIBRARIES})

add_executable(Analysis Analysis.cpp CorrelationTask.cpp)
target_compile_definitions(Analysis PUBLIC "-DUSE_ROOT")
target_include_directories(Analysis PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(Analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Analysis QnCorrections ReducedEvent Base Correlation ${ROOT_LIBRARIES})
