/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 31 Jan 2013 05:01:15 PM
#
# Filename: contagion.cc
#
=========================================*/

#include <boost/program_options.hpp>
#include "dataset/social_graph.h"
#include "random_generator/random_generators.h"
#include "classifier/pca_classifier.h"
#include "dataset/bfs_cross_contagion_engine.h"
#include "dataset/parallel_cross_contagion_engine.h"
#include "dataset/bp_contagion_engine.h"

#include "dataset/bp_ambiguous_contagion_engine.h"
#include "classifier/pca_ambiguous_classifier.h"


int main(int argc, char **argv) {
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

  /* Use BFS to visit
  // BFS visit
  // Find start point
  sn::VertexIterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(graph); vi!=vi_end; ++vi) {
    if (graph[*vi].person_id == "Infection_Source") {
      break;
    }  
  }
  // Check if found the Infection_Source
  assert(vi!=vi_end);
 
  // BFS visit
  std::map<std::string, int> step_map;
  sn::BfsFaceRecognitionVisitor vis(image_prefix,
                                    &album_map,
                                    &step_map);
  boost::breadth_first_search(graph, vertex(*vi, graph), boost::visitor(vis));

  // Attach BFS visit result
  for (tie(vi, vi_end) = vertices(graph); vi!=vi_end; ++vi) {
    int color = step_map[graph[*vi].person_id];
    graph[*vi].SetColor(255, color*20, color*20);
  }
  */

  // Self defind visit engine.
//  sn::BaseContagionEngine * contagion_engine = new sn::BfsCrossContagionEngine<fn::PCAClassifier>(image_prefix, &graph, &album_map);
//  sn::BaseContagionEngine * contagion_engine = new sn::ParallelCrossContagionEngine<fn::PCAClassifier>(image_prefix, &graph, &album_map);
  // sn::BaseContagionEngine * contagion_engine = new sn::BeliefPropagationContagionEngine<fn::PCAClassifier>(image_prefix, &graph, &album_map);
  // Self correct (Exp 2)
  sn::BaseContagionEngine * contagion_engine = new sn::BeliefPropagationAmbiguousContagionEngine<fn::PCAAmbiguousClassifier>(image_prefix, &graph, &album_map);
  //contagion_engine->Init("Infection_Source");
  contagion_engine->Init();
  contagion_engine->FirstRun();
  contagion_engine->Run();
  delete contagion_engine;
 
  // Save result
  // Save graph
  std::string output_dot_file = output_prefix + ".dot";
  sn::WriteGraphToFile(graph, output_dot_file);
  // Save the album
  std::string output_album_file = output_prefix + ".alb";
  sn::WriteAlbumMapToFile(album_map, output_album_file);

  return 0;
}
