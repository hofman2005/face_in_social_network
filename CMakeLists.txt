#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Mar 2012 08:57:30 AM
#
# Filename: CMakeLists.txt
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

PROJECT(Social_Network)

SET(Social_Network_VERSION_MAJOR 1)
SET(Social_Network_VERSION_MINOR 0)

# Enable OpenMP
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread -fopenmp -O2")
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
  "fr_engine/*.cc"
  "graph/*.cc" 
  "random_generator/*.cc")
  #"*.cc")

# Outputs
# ADD_EXECUTABLE(social_network main.cc ${SRC_LIST})
# TARGET_LINK_LIBRARIES(social_network ${OpenCV_LIBS})

ADD_EXECUTABLE(generator_driver generator_driver.cc ${SRC_LIST})
TARGET_LINK_LIBRARIES(generator_driver ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(contagion contagion.cc ${SRC_LIST})
TARGET_LINK_LIBRARIES(contagion ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(analysis analysis.cc ${SRC_LIST})
TARGET_LINK_LIBRARIES(analysis ${OpenCV_LIBS} ${Boost_LIBRARIES})
