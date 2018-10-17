cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
project(flow)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_cpp_STANDARD 14)

set(REDUCEDEVENT_SOURCES AliReducedBaseEvent.cpp
        AliReducedCaloClusterInfo.cpp
        AliReducedEventPlaneInfo.cpp
        AliReducedPairInfo.cpp
        AliReducedVarCut.cpp
        AliReducedBaseTrack.cpp
        AliReducedEventCut.cpp
        AliReducedFMDInfo.cpp
        AliReducedTrackCut.cpp
        AliReducedVarManager.cpp
        AliReducedBaseTrackCut.cpp
        AliReducedEventInfo.cpp
        AliReducedInfoCut.cpp
        AliReducedTrackInfo.cpp)

string(REPLACE ".cpp" ".h" REDUCEDEVENT_HEADERS "${REDUCEDEVENT_SOURCES}")

set(CMAKE_VERBOSE_MAKEFILE ON)

ROOT_GENERATE_DICTIONARY(G__ReducedEvent ${REDUCEDEVENT_HEADERS} LINKDEF ReducedEventLinkDef.h)
add_library(ReducedEvent SHARED ${REDUCEDEVENT_SOURCES} G__ReducedEvent)
target_compile_definitions(ReducedEvent PUBLIC "-DUSE_ROOT")
target_include_directories(ReducedEvent PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(ReducedEvent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ReducedEvent ROOT::Core ROOT::MathMore ROOT::MathCore ROOT::Hist ROOT::TreePlayer ROOT::Tree)

install(TARGETS ReducedEvent DESTINATION lib)
install(FILES ${QNCORR_HEADERS} DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libReducedEvent_rdict.pcm DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libReducedEvent.rootmap DESTINATION lib)
