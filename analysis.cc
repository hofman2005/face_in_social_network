/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Mar 2012 09:13:00 AM
#
# Filename: analysis.cc
#
=========================================*/

#include <boost/program_options.hpp>
#include "dataset/social_graph.h"

int main(int argc, char **argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  std::string graph_prefix;
  std::string output_prefix;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("graph-prefix,i", boost::program_options::value<std::string>(&graph_prefix), 
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
  if (!vmap.count("graph-prefix") || 
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

  // Convert the recognition accuracy to color
  sn::VertexIterator ver_it, ver_it_end;
  tie(ver_it, ver_it_end) = vertices(graph);
  
  for (; ver_it != ver_it_end; ++ver_it) {
    std::string &id = graph[*ver_it].person_id;
    sn::Album &album = album_map[id];
    // Compute the accuracy for a node
    int count = 0;
    int correct_count = 0;
    for (sn::Album::const_iterator it=album.begin();
         it!=album.end();
         ++it) {
      if (it->GetAssignedBy() != "God") {
        ++count;
        if (it->GetAssignedId() == it->GetTrueId()) {
          ++correct_count;
        }
      }
    }
    int color = 0;
    if (count==0) {
      int a=0;
      graph[*ver_it].SetColor(0, 0, 0);
    }
    if (count!=0) {
      color = correct_count * 255 / count;
      graph[*ver_it].SetColor(color, 128, 0);
    }
  }

  // Save result
  // Save graph
  std::string output_dot_file = output_prefix + ".dot";
  sn::WriteGraphToFile(graph, output_dot_file);
  // Save the album
  // std::string output_album_file = output_prefix + ".alb";
  // sn::WriteAlbumMapToFile(album_map, output_album_file);

  return 0;
}
