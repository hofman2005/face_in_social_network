/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 06 Mar 2012 11:31:16 AM
#
# Filename: build_graph.h
#
=========================================*/

#ifndef RANDOM_GENERATOR_BUILD_GRAPH_H_
#define RANDOM_GENERATOR_BUILD_GRAPH_H_

#include "dataset/social_graph.h"

namespace SocialNetwork {
namespace RandomGenerators {

void RandomEdgeGenerator(SocialNetwork::SocialGraph *graph);

}
}

#endif  // RANDOM_GENERATOR_BUILD_GRAPH_H_
