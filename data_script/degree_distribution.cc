/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Nov 2012 11:42:38 PM
#
# Filename: degree_distribution.cc
#
=========================================*/

#include <fstream>
#include <iostream>
#include <vector>
#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include "dataset/social_graph.h"

int main(int argc, char ** argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  std::string input_file;
  std::string output_file;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("graph,g", boost::program_options::value<std::string>(&input_file), 
     "Graph file in dot format.")
    ("output,o", boost::program_options::value<std::string>(&output_file),
     "Output file.")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, options), vmap);
  po::notify(vmap);

  if (vmap.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }
  if (!vmap.count("graph") 
      || !vmap.count("output")) {
    std::cout << options << std::endl;
    return 0;
  }

  sn::SocialGraph graph(0);
  sn::ReadGraphFromFile(input_file, &graph);

  std::pair<sn::VertexIterator, sn::VertexIterator> vertex_it = vertices(graph);
  std::pair<sn::AdjacencyIterator, sn::AdjacencyIterator> friends_it;
  
  std::vector<int> hist;
  int max=0;

  for (; vertex_it.first != vertex_it.second; ++vertex_it.first) {
    friends_it = boost::adjacent_vertices(*(vertex_it.first), graph);
    int num = std::distance(friends_it.first, friends_it.second);
    if (max < num)
      max = num;
    if (num >= hist.size()) {
      hist.resize(num+1);
    }
    ++hist[num];
  }

  std::ofstream out(output_file.c_str());
  for (int i=0; i<max; ++i)
    out << i << " " << hist[i] << std::endl;
}
