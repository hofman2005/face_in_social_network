/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 01 May 2012 03:57:57 PM
#
# Filename: baseline_all.cc
#
# Description: Baseline of a normal FR. A classifier is trained and tested on the union of all images.
#
=========================================*/

#include <boost/program_options.hpp>
#include "dataset/social_graph.h"

int main(int argc, char** argv) {
  namespace sn = SocialNetwork;
  namespace fn = FaceRecognition;
  namespace po = boost::program_options;

  // Parse options
  std::string image_prefix;
  std::string graph_prefix;
  std::string output_prefix;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("image-prefix", boost::program_options::value<std::string>(&image_prefix), 
     "Image prefix.")
    ("graph-prefix", boost::program_options::value<std::string>(&graph_prefix), 
     "Graph file prefix. Extension .dot and .alb will be appended.")
    ("output-prefix,o", boost::program_options::value<std::string>(&output_prefix), 
     "Output")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, options), vmap);
  po::notify(vmap);

  if (vmap.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }
  if (!vmap.count("image-prefix") || 
      !vmap.count("graph-prefix") || 
      !vmap.count("output-prefix")) {
    std::cout << options << std::endl;
    return 0;
  }

  // Load graph and album
  sn::SocialGraph graph(0);
  std::string graph_filename = graph_prefix + ".dot";
  std::cout << "Loading graph file: " << graph_filename << std::endl;
  sn::ReadGraphFromFile(graph_filename, &graph);

  sn::AlbumMap album_map;
  std::string album_filename = graph_prefix + ".alb";
  std::cout << "Loading album file: " << album_filename << std::endl;
  sn::ReadAlbumMapFromFile(album_filename, &album_map);

  // Merge the albums

  // Train

  // Test

  // Give the final result.
}
