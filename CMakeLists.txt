#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 25 Apr 2013 04:56:01 PM
#
# Filename: CMakeLists.txt
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

PROJECT(Social_Network)

SET(Social_Network_VERSION_MAJOR 1)
SET(Social_Network_VERSION_MINOR 0)

INCLUDE(../CMakeLists.local OPTIONAL)

# Enable OpenMP
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread -fopenmp -O3")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread -fopenmp")

# Disable deprecated parts of boost
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-deprecated")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-deprecated")

# Boost
FIND_PACKAGE(Boost 1.40 COMPONENTS program_options graph regex REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})

# OpenCV
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# Source
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")
FILE(GLOB SRC_LIST "classifier/*.cc" 
  "dataset/*.cc" 
#  "fr_engine/*.cc"
  "graph/*.cc" 
  "random_generator/*.cc"
  "edge_discover/*.cc")
  #"*.cc")

# Outputs
# ADD_EXECUTABLE(social_network main.cc ${SRC_LIST})
# TARGET_LINK_LIBRARIES(social_network ${OpenCV_LIBS})

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ADD_LIBRARY(SocialNetwork ${SRC_LIST})
# ADD_EXECUTABLE(generator_driver generator_driver.cc ${SRC_LIST})
# TARGET_LINK_LIBRARIES(generator_driver ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(extract_standford data_script/extract_sub_v2.cc)

ADD_EXECUTABLE(generator_driver generator_driver.cc)
TARGET_LINK_LIBRARIES(generator_driver SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(album_generator album_generator.cc)
TARGET_LINK_LIBRARIES(album_generator SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(degree_distribution data_script/degree_distribution.cc)
TARGET_LINK_LIBRARIES(degree_distribution SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(hist_image hist_image.cc)
TARGET_LINK_LIBRARIES(hist_image SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(graph_utility_test graph_utility_test.cc)
TARGET_LINK_LIBRARIES(graph_utility_test SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(contagion contagion.cc)
TARGET_LINK_LIBRARIES(contagion SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(analysis analysis.cc)
TARGET_LINK_LIBRARIES(analysis SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(final_readout final_readout.cc)
TARGET_LINK_LIBRARIES(final_readout SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(baseline baseline.cc)
TARGET_LINK_LIBRARIES(baseline SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(baseline_all baseline_all.cc)
TARGET_LINK_LIBRARIES(baseline_all SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(edge_discover edge_discover_main.cc)
TARGET_LINK_LIBRARIES(edge_discover SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(edge_discover_baseline edge_discover_baseline_main.cc)
TARGET_LINK_LIBRARIES(edge_discover_baseline SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(analysis_edge_roc analysis_edge_roc.cc)
TARGET_LINK_LIBRARIES(analysis_edge_roc SocialNetwork ${OpenCV_LIBS} ${Boost_LIBRARIES})
