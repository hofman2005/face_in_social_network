/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 29 Feb 2012 10:47:35 AM
#
# Filename: main.cc
#
=========================================*/

#include <iostream>
#include <string>
#include <fstream>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
#include <boost/graph/graphviz.hpp>
#include "dataset/social_graph.h"
#include "random_generator/edge_generator.h"

const char *getCmdOption(const char **begin, const char **end, const std::string &option) {
  const char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

int main(const int argc, const char **argv) {
  using std::string;
  using std::cout;
  using std::endl;

  namespace SN = SocialNetwork;
  namespace RG = RandomGenerators;

  const char *filename = getCmdOption(argv, argv+argc, "-o");
  if (filename == NULL) {
    cout << "Usage: %1 -o out_file" << endl;
    return 0;
  }

  const int num_vertex = 4000;
  SN::SocialGraph graph(num_vertex);

//   // Add a vertex
//   string name = "a";
// //  SN::Person person(name);
//   SN::Vertex ul = add_vertex(graph);
// //  graph[ul].person = &person;
//  graph[ul].person_id = &name; 
//  for (int i=0; i<num_vertex; i++) {
//    add_vertex(graph);
//  }

  RG::RandomEdgeGenerator(&graph);

  // Save graph
  std::ofstream out(filename);
  boost::write_graphviz(out, graph); 
  out.close();

  return 0;
}
