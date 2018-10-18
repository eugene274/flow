cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0048 NEW)
project(flow VERSION 1)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

# By default build optimized code with debug symbols.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()
set(CMAKE_ALLOWED_BUILD_TYPES DEBUG RELEASE RELWITHDEBINFO)
if(NOT CMAKE_BUILD_TYPE IN_LIST CMAKE_ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "Invalid build type ${CMAKE_BUILD_TYPE}. Use one of: ${CMAKE_ALLOWED_BUILD_TYPES}.")
endif()
# Set default flags for the different build types.
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG -D__DEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ftree-vectorize -ffast-math -DNODEBUG")
message(STATUS "Using CXX flags for ${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")

# ROOT
find_package(ROOT REQUIRED COMPONENTS Core MathCore RIO Hist Tree Net TreePlayer)
include(${ROOT_USE_FILE})
if(ROOT_FOUND)
    message(STATUS "Using ROOT: ${ROOT_VERSION} <${ROOT_CONFIG}>")

    add_executable(analysis main.cpp CorrectionTask.cpp CorrelationTask.cpp)
    target_compile_definitions(analysis PUBLIC "-DUSE_ROOT")
    target_include_directories(analysis PRIVATE ${ROOT_INCLUDE_DIRS})
    target_include_directories(analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(analysis QnCorrections ReducedEvent FlowBase DifferentialCorrection Correlation ${ROOT_LIBRARIES})
    target_compile_options(analysis PRIVATE -Wall cxx_std_14)

endif()

