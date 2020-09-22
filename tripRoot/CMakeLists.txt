CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

## make Visual Studio behave nicely
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tripRoot")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tripRoot")
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/bin")
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/tripRoot")
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/tripRoot")
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

include_directories(${tripRoot_SOURCE_DIR})

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -m64 -O3")
endif(NOT WIN32)

if(WIN32)
  set(NO_N4_128 1)
endif(WIN32)

if(WIN32)
  set(extension ".dll")
else(WIN32)
  set(extension ".so")
endif(WIN32)

if(NOT NO_N4_128)
  add_definitions(-DN4INT128)
endif(NOT NO_N4_128)

if(NOT NOEXTRACT)
  add_definitions(-DdoExtractAndContract)
endif(NOT NOEXTRACT)

if(CONTRACT_NUM)
  add_definitions("-DCONTRACT_MAX_EXTRA_SIZE=$(CONTRACT_NUM)")
endif(CONTRACT_NUM)

message(STATUS "CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

## LIBRARIES
## EXECUTABLES

add_executable(test_read_write_newick test_read_write_newick.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  TripletRooting.cpp
  int_stuff.cpp)
    

add_executable(tripRoot2 triplet_MV_rooting.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  MinVarCounter.cpp
  TripletRooting.cpp
  TrplMVRooting.cpp  
  int_stuff.cpp)

add_executable(tripMVRoot triplet_MV_rooting.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  MinVarCounter.cpp
  TripletRooting.cpp
  TrplMVRooting.cpp  
  int_stuff.cpp)

add_executable(MVRoot MV_rooting.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  MinVarCounter.cpp
  MVRooting.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  int_stuff.cpp)

add_executable(pruneTree prune_tree.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  MinVarCounter.cpp
  MVRooting.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  int_stuff.cpp)

add_executable(tripVote triplet_voting.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  MinVarCounter.cpp
  MVRooting.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  TripletRooting.cpp
  int_stuff.cpp)

add_executable(delta_triplet_dist delta_triplet_distance.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  MinVarCounter.cpp
  MVRooting.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  TripletRooting.cpp
  int_stuff.cpp)

add_executable(tripOGRoot triplet_og_rooting.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  TripletRooting.cpp
  TripletRooting_OG.cpp
  int_stuff.cpp)

add_executable(ogRoot outgroup_rooting.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  OutgroupRooting.cpp
  int_stuff.cpp  
)

add_executable(collapseTree collapse_tree.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  RefTreeGen.cpp
  int_stuff.cpp  
)
add_executable(test_trplMV_root test_trplMV_root.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  MinVarCounter.cpp
  TripletCounter.cpp
  TripletRooting.cpp
  TrplMVRooting.cpp
  int_stuff.cpp
)

add_executable(test_triplet_root test_triplet_root.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletCounter.cpp
  TripletRooting.cpp
  int_stuff.cpp)
add_test(
  NAME test_triplet_root 
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_triplet_root)

## Installation
#install(TARGETS tripRoot RUNTIME DESTINATION bin)
#if(WIN32)
#  install(FILES VCredist/msvcp120.dll VCredist/msvcr120.dll DESTINATION bin)
#endif(WIN32)
