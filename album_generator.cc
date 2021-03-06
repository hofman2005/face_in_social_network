/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
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

#include <stdlib.h>
#include <time.h>

int main(int argc, char ** argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  std::string input_file;
  std::string input_graph;
  std::string output_prefix;
  int factor = 10000;
  int dist_thres = 4e5;
  double label_percent = 0.3;
  double wrong_label_percent = 0.0;
  double remove_edge_percent = 0.0;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("images,i", boost::program_options::value<std::string>(&input_file), 
     "Image list file.")
    ("graph,g", boost::program_options::value<std::string>(&input_graph),
     "Graph file.")
    ("out-prefix,o", boost::program_options::value<std::string>(&output_prefix), 
     "Output file prefix. Extension .dot and .alb will be appended.")
    // ("factor", boost::program_options::value<int>(&factor)->default_value(10000),
    //  "The factor parameter will multiplied with a random number in [0,1].")
    // ("dist-thres", boost::program_options::value<int>(&dist_thres)->default_value(4e5),
    //  "The threshold of the distance between a pair of friends.")
    ("label-percent,l", boost::program_options::value<double>(&label_percent),
     "The percentage of initial labeled images, default 0.3.")
    ("wrong-label-percent,w", boost::program_options::value<double>(&wrong_label_percent),
     "The percentage of initial labeled images, default 0.0.")
    ("remove-edge-percent,e", boost::program_options::value<double>(&remove_edge_percent),
     "The percentage of removed edges, default 0.0.")
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

  // If the network has fewer nodes than the image_map
  if (num_nodes < num_subjects) {
    std::vector<std::string> ids;
    ids.reserve(image_map.size());
    for (sn::AlbumMap::const_iterator it = image_map.begin();
        it != image_map.end();
        ++it) {
      ids.push_back(it->first);
    }
    srand( time(NULL) );
    sn::AlbumMap temp_album;
    for (int i = 0; i < num_nodes; ++i) {
      int j = rand() % (num_subjects - i);
      temp_album[ids[j]] = image_map[ids[j]];
      std::swap(ids[j], ids[num_subjects-i-1]);
    }
    image_map.swap(temp_album);
  }
  if (num_nodes > num_subjects) {
    std::cout << "TODO: need to add support for num_nodes > num_subjects" << std::endl;
    return 0;
  }

  sn::RandomGenerators generator;
  generator.AssignID(image_map, &graph);

  // Generate labels
  // const double label_percent = 0.3;
  generator.LabelGenerator2(&image_map, label_percent);

  // Generate the albums
  sn::AlbumMap album_map;
  generator.AlbumGenerator(graph, image_map, &album_map);

  // Change labels
  if (wrong_label_percent > 0.0)
    generator.LabelGenerator_WrongLabels(&album_map, wrong_label_percent);

  // Randomly remove labels
  if (remove_edge_percent > 0.0) {
    std::string output_dot_file = output_prefix + "_ori.dot";
    sn::WriteGraphToFile(graph, output_dot_file);
    generator.RemoveEdges(&graph, remove_edge_percent);
  }

  // Save graph
  std::string output_dot_file = output_prefix + ".dot";
  sn::WriteGraphToFile(graph, output_dot_file);
  // Save the album
  std::string output_album_file = output_prefix + ".alb";
  sn::WriteAlbumMapToFile(album_map, output_album_file);

  sn::WriteAlbumMapToFile(image_map, "/tmp/image_map.alb");
 }
