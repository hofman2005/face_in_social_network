/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 16 Apr 2012 08:43:10 AM
#
# Filename: generator_driver.cc
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
#include "random_generator/random_generators.h"

namespace {
void TestGraphReader(std::string input_prefix) {
  namespace sn = SocialNetwork;
  sn::SocialGraph graph(0);
  sn::AlbumMap album_map;
  sn::ReadGraphFromFile(input_prefix+".dot", &graph);
  sn::ReadAlbumMapFromFile(input_prefix+".alb", &album_map);

  // Write out the graph.
  sn::WriteGraphToFile(graph, "/tmp/verify.dot");
  sn::WriteAlbumMapToFile(album_map, "/tmp/verify.alb");
}

}

int main(int argc, char **argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  std::string input_file;
  std::string output_prefix;
  int max_num_nodes;
  int factor = 10000;
  int dist_thres = 4e5;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("in,i", boost::program_options::value<std::string>(&input_file), 
     "Image list file.")
    ("out-prefix,o", boost::program_options::value<std::string>(&output_prefix), 
     "Output file prefix. Extension .dot and .alb will be appended.")
    ("max-nodes", boost::program_options::value<int>(&max_num_nodes)->default_value(0),
     "The maximum number of nodes in the graph.")
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
  if (!vmap.count("in") || !vmap.count("out-prefix")) {
    std::cout << options << std::endl;
    return 0;
  }

  // Analysis image list file
  sn::AlbumMap image_map;
  sn::AnalysisImageList(input_file, &image_map);
  // FOR TESTING ONLY Show loaded image list info
/*  for(ImageMap::const_iterator it1 = image_map.begin(); 
      it1 != image_map.end(); 
      ++it1) {
    for(std::vector<std::string>::const_iterator it2 = (*it1).second.begin(); 
        it2 != (*it1).second.end(); 
        ++it2) {
      std::cout << (*it1).first << " " << *it2 << std::endl;
    }
  }
*/

  std::cout << "Number of subject: " << image_map.size() << std::endl;

  if (max_num_nodes > 0) {
    std::cout << "Maximum number of nodes is set to: " << max_num_nodes << std::endl;
    int count = 0;
    sn::AlbumMap::iterator it;
    for (it = image_map.begin();
         it != image_map.end() && count <max_num_nodes;
         ++it, ++count) {}
    image_map.erase(it, image_map.end());
  }

  std::cout << "Factor: " << factor << std::endl << "dist_thres: " << dist_thres << std::endl;

  // Generate graph
  //namespace rg = SocialNetwork::RandomGenerators;
  const int num_vertex = image_map.size();
  sn::SocialGraph graph(num_vertex);
  
  //rg::RandomEdgeGenerator(&graph);
  sn::RandomGenerators generator;
  generator.EdgeGenerator(factor, dist_thres, &graph);

  // Assign person_ID
  generator.AssignID(image_map, &graph);
  
  // Generate albums
  sn::AlbumMap album_map;
  generator.AlbumGenerator(graph, image_map, &album_map);

  // Generate infection sources
  // const int num_source = 2;
  // sn::Vertex source = generator.InfectionSourceGenerator(num_source, &graph);

  // Assign labels
  // generator.LabelGenerator(graph, source, &album_map);
  generator.LabelGenerator(graph, &album_map);
  
  // Save graph
  std::string output_dot_file = output_prefix + ".dot";
  sn::WriteGraphToFile(graph, output_dot_file);
  // Save the album
  std::string output_album_file = output_prefix + ".alb";
  sn::WriteAlbumMapToFile(album_map, output_album_file);
  
  // Test the reader
  TestGraphReader(output_prefix);

  return 0;
}
