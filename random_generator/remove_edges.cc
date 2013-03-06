/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: remove_edges.cc
#
=========================================*/
#include "random_generator/random_generators.h"

namespace SocialNetwork {
using namespace std;
void RandomGenerators::RemoveEdges(SocialGraph* graph, double remove_edge_percent) {
  unsigned int num = num_edges(*graph);
  EdgeIterator ei, ei_end, next;
  tie(ei, ei_end) = edges(*graph);
  int count = 0;
  for (next = ei; ei != ei_end; ei = next) {
    ++next;
    int p = rand() % 1000;
    if (p < remove_edge_percent * 1000) {
      remove_edge(*ei, *graph);
      ++ count;
    }
  }

  cout << "Total edge: " << num << " Removed: " << count << endl;
}
}
