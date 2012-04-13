/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 08 Mar 2012 05:10:16 PM
#
# Filename: infection_source_generator.cc
#
=========================================*/

#include "random_generator/random_generators.h"
#include <boost/graph/topological_sort.hpp>
#include <stdlib.h>
#include <time.h>

namespace SocialNetwork {
// This function will add a invisible vertex with name Infection_Source into
// the graph. This vertex has edges to all infection sources and can be used as
// a start point for the BFS algorithm.
// The infection sources are randomly selected.
// 
// Warning: Only call this function after you dont want to alter the graph 
//          TODO: add compatibility in other module to support the invisible
//          source vertex concept
//
// TODO Select vertex with some criterions.
// TODO Fix compactibility with AssignID()
Vertex RandomGenerators::InfectionSourceGenerator(const int number, SocialGraph *graph) {
  // Select sources and put in the last 'number' elements of candidate
  boost::graph_traits<SocialGraph>::vertices_size_type size = num_vertices(*graph);

  std::vector<int> candidate(size);
  std::vector<int>::iterator it = candidate.begin();
  for (int i=0; it != candidate.end(); ++it, ++i) {
    *it = i; 
  }
  srand( time(NULL) );
  for (int i=0; i<number; i++) {
    int p = rand() % (size-i);
    std::swap(candidate[p], candidate[size-i-1]);
  }

  // Insert invisible source vertex
  const std::string name = "Infection_Source";
  Vertex source = boost::add_vertex(*graph);
  (*graph)[source].person_id = name;
  (*graph)[source].style = "invis";

  // Add edges
  for (int i=0; i<number; i++) {
    SocialGraph::vertex_descriptor o = vertex(candidate[size-i-1], *graph);
    boost::add_edge(source, o, *graph);
  }

  return source;
}

}
