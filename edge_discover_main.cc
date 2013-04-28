/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: edge_discover.cc
#
=========================================*/
#include <boost/program_options.hpp>
#include <map>
#include <string>
#include "dataset/social_graph.h"
#include "edge_discover/edge_discover.h"
#include "classifier/stat_model_classifier.h"
#include "edge_discover/edge_discover_baseline.h"

int main(int argc, char **argv) {
  namespace sn = SocialNetwork;
  namespace fr = FaceRecognition;
  namespace po = boost::program_options;
  using namespace std;

  // Parse options
  std::string image_prefix;
  std::string graph_filename;
  std::string album_filename;
  std::string output;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("image-prefix", boost::program_options::value<std::string>(&image_prefix), 
     "Image prefix.")
    ("graph-file", boost::program_options::value<std::string>(&graph_filename), 
     "Graph file.")
    ("album-file", boost::program_options::value<std::string>(&album_filename), 
     "Album file.")
    ("output,o", boost::program_options::value<std::string>(&output), 
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
      !vmap.count("graph-file") || 
      !vmap.count("album-file") ||
      !vmap.count("output")) {
    std::cout << options << std::endl;
    return 0;
  }

  // Load graph and album
  sn::SocialGraph graph(0);
  std::cout << "Loading graph file: " << graph_filename << std::endl;
  sn::ReadGraphFromFile(graph_filename, &graph);

  sn::AlbumMap album_map;
  std::cout << "Loading album file: " << album_filename << std::endl;
  sn::ReadAlbumMapFromFile(album_filename, &album_map);

  sn::Photo::SetPrefix(image_prefix);

  // Run
  // sn::EdgeDiscoverBaseline * discover_engine = new sn::EdgeDiscoverBaseline();
  sn::EdgeDiscover * discover_engine = new sn::EdgeDiscover();
  fr::BaseClassifier * classifier = new fr::StatModelClassifier(fr::StatModelClassifier::KNearest);
  discover_engine->SetClassifier(classifier);

  map<pair<string, string>, double> result;
  discover_engine->Run(graph, album_map, result);

  delete discover_engine;
  delete classifier;

  // Save Result
  ofstream out_file(output.c_str());
  for (map<pair<string,string>,double>::const_iterator it = result.begin();
      it != result.end();
      ++it) {
    out_file << it->first.first << " "
      << it->first.second << " "
      << it->second << endl;
  }
}
