/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Nov 2012 02:15:48 PM
#
# Filename: album_generator.cc
#
=========================================*/

#include <fstream>
#include <iostream>
#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include "dataset/social_graph.h"
#include "random_generator/random_generators.h"

int main(int argc, char ** argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  std::string input_file;
  std::string input_graph;
  std::string output_prefix;
  int factor = 10000;
  int dist_thres = 4e5;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("images,i", boost::program_options::value<std::string>(&input_file), 
     "Image list file.")
    ("graph,g", boost::program_options::value<std::string>(&input_graph),
     "Graph file.")
    ("out-prefix,o", boost::program_options::value<std::string>(&output_prefix), 
     "Output file prefix. Extension .dot and .alb will be appended.")
    ("factor", boost::program_options::value<int>(&factor)->default_value(10000),
     "The factor parameter will multiplied with a random number in [0,1].")
    ("dist-thres", boost::program_options::value<int>(&dist_thres)->default_value(4e5),
     "The threshold of the distance between a pair of friends.")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, options), vmap);
  po::notify(vmap);

  if (vmap.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }
  if (!vmap.count("images") 
      || !vmap.count("out-prefix")
      || !vmap.count("graph")) {
    std::cout << options << std::endl;
    return 0;
  }

  // Load the graph
  std::cout << "Loading graph file: " << input_graph << std::endl;
  sn::SocialGraph graph(0);
  sn::ReadGraphFromFile(input_graph, &graph);

  // Analysis image list file
  std::cout << "Loading image list file: " << input_file << std::endl;
  sn::AlbumMap image_map;
  sn::AnalysisImageList(input_file, &image_map);

  // Assign person id
  int num_nodes = num_vertices(graph);
  int num_subjects = image_map.size();
  std::cout << "Nodes in graph: " << num_nodes << std::endl;
  std::cout << "Subjects in image dataset: " << num_subjects << std::endl;

  if (num_nodes != num_subjects) {
    std::cout << "TODO: need to add support for num_nodes != num_subjects" << std::endl;
    return 0;
  }

  sn::RandomGenerators generator;
  generator.AssignID(image_map, &graph);

  // Generate the albums
  sn::AlbumMap album_map;
  generator.AlbumGenerator(graph, image_map, &album_map);

  // Save graph
  std::string output_dot_file = output_prefix + ".dot";
  sn::WriteGraphToFile(graph, output_dot_file);
  // Save the album
  std::string output_album_file = output_prefix + ".alb";
  sn::WriteAlbumMapToFile(album_map, output_album_file);
 }
