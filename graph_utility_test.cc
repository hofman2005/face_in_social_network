/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Nov 2012 04:18:56 PM
#
# Filename: graph_utility_test.cc
#
=========================================*/

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include "dataset/social_graph.h"


void TestGraphReader(std::string input_prefix) {
  namespace sn = SocialNetwork;
  sn::SocialGraph graph(0);
  sn::AlbumMap album_map;
  std::string input_graph = input_prefix+".dot";
  std::cout << "Reading: " << input_graph << std::endl;
  sn::ReadGraphFromFile(input_graph, &graph);
  std::string input_album = input_prefix+".alb";
  std::cout << "Reading: " << input_album << std::endl;
  sn::ReadAlbumMapFromFile(input_album, &album_map);

  // Write out the graph.
  std::string output_graph = "/tmp/verify.dot";
  std::cout << "Writing: " << output_graph << std::endl;
  sn::WriteGraphToFile(graph, output_graph);
  std::string output_album = "/tmp/verify.alb";
  std::cout << "Writing: " << output_album << std::endl;
  sn::WriteAlbumMapToFile(album_map, output_album);
}

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cout << "usage: %prog graph-prefix" << std::endl;
    return 0;
  }

  std::string input(argv[1]);
  TestGraphReader(input);
}
