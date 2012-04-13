/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 07 Mar 2012 04:52:52 PM
#
# Filename: edge_generator.cc
#
=========================================*/

//#include "random_generator/edge_generator.h"
#include "random_generator/random_generators.h"
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <stdlib.h>
#include <time.h>

namespace SocialNetwork {
//namespace RandomGenerators {
  
void RandomGenerators::EdgeGenerator(const int factor, 
                                     const int dist_thres,
                                     SocialGraph *graph) {
  std::vector<SocialNetwork::SocialGraph::vertex_descriptor> vertex_list;
  boost::topological_sort(*graph, std::back_inserter(vertex_list));

  const int size = vertex_list.size();
  std::vector<float> x(size);
  std::vector<float> y(size);

  srand( time(NULL) );
  // Random Euclidean graph on X-Y plane
  //const int factor = 10000;
  for (int i=0; i<size; i++) {
    x[i] = rand() % factor;
    y[i] = rand() % factor;
  }
  //const int dist_thres = 7e5; 
  for (int i=0; i<size; i++) {
    for (int j=i+1; j<size; j++) {
      int dist = (x[i]-x[j])*(x[i]-x[j]);
      dist += (y[i]-y[j])*(y[i]-y[j]);

      if (dist<dist_thres) {
        SocialNetwork::SocialGraph::vertex_descriptor o1, o2;
        o1 = vertex(i, *graph);
        o2 = vertex(j, *graph);
        boost::add_edge(o1, o2, *graph);
      }
    }
  }

  // Random Euclidean graph on sphere surface
  // const int factor = 10000;
  // const float pi = 3.141592654;
  // for (int i=0; i<size; i++) {
  //   x[i] = static_cast<float>(rand() % factor) / factor;
  //   x[i] = x[i] * 2 * pi;
  //   y[i] = static_cast<float>(rand() % factor) / factor;
  //   y[i] = (y[i]-0.5) * pi;
  // }
  // using std::sin;
  // using std::cos;
  // using std::sqrt;
  // using std::atan2;
  // const float dist_thres = 0.3; 
  // for (int i=0; i<size; i++) {
  //   for (int j=i+1; j<size; j++) {
  //     //float dist =  sin(y[i])*sin(y[j]);
  //     //dist += cos(y[i])*cos(y[j])*cos(x[i]-x[j]);
  //     //dist = acos(dist);
  //     float dx = x[i] - x[j];
  //     float dy = y[i] - y[j];
  //     float dist11 = cos(y[i])*sin(dx);
  //     float dist12 = cos(y[j])*sin(y[i]) - sin(y[j])*cos(y[i])*cos(dx);
  //     float dist1 = sqrt(dist11*dist11 + dist12*dist12);
  //     float dist2 = sin(y[j])*sin(y[i]) + cos(y[j])*cos(y[i])*cos(dx);
  //     float dist = atan2(dist1, dist2);

  //     if (dist<dist_thres) {
  //       SocialNetwork::SocialGraph::vertex_descriptor o1, o2;
  //       o1 = vertex(i, *graph);
  //       o2 = vertex(j, *graph);
  //       boost::add_edge(o1, o2, *graph);
  //     }
  //   }
  // }
}

//}
}
