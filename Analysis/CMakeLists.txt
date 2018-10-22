cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0048 NEW)
project(flowanalysis VERSION 1)

add_subdirectory(ReducedEvent)

add_executable(correct Correct.cpp CorrectionTask.cpp)
target_compile_definitions(correct PUBLIC "-DUSE_ROOT")
target_include_directories(correct PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(correct PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(correct QnCorrections ReducedEvent Base Correction ${ROOT_LIBRARIES})

add_executable(analysis Analysis.cpp CorrelationTask.cpp)
target_compile_definitions(analysis PUBLIC "-DUSE_ROOT")
target_include_directories(analysis PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(analysis QnCorrections ReducedEvent Base Correlation ${ROOT_LIBRARIES})

