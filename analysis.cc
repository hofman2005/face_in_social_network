/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Nov 2012 11:46:09 PM
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

  // Test
  // sn::WriteAlbumMapToFile(album_map, "/tmp/test.alb");

  // Convert the recognition accuracy to color
  sn::VertexIterator ver_it, ver_it_end;
  tie(ver_it, ver_it_end) = vertices(graph);

  sn::SocialGraph false_alarm_graph = graph;

  float total_count = 0.0;
  float total_correct_count = 0.0;
  
  std::vector< std::vector<float> > hist;
  for (; ver_it != ver_it_end; ++ver_it) {
    std::string &id = graph[*ver_it].person_id;
    sn::Album &album = album_map[id];
    // Compute the accuracy for a node
    float count = 0.0;
    float correct_count = 0.0;
    float wrong_count = 0.0;
    for (sn::Album::const_iterator it=album.begin();
         it!=album.end();
         ++it) {
      if (it->GetAssignedBy() != "God") {
        ++count;
        if (it->GetAssignedId() == it->GetTrueId()) {
          ++correct_count;
        }
        if (it->GetAssignedId() != "-" && it->GetAssignedId() != it->GetTrueId()) {
          ++wrong_count;
        }
      }
    }
    int color = 0;
    if (count==0) {
      int a=0;
      graph[*ver_it].SetColor(0, 0, 0);
      false_alarm_graph[*ver_it].SetColor(0,0,0);
      // Set accuracy
      std::ostringstream ss;
      ss << std::setprecision(4) << "0.00%";
      graph[*ver_it].accuracy = ss.str();
    }
    if (count!=0) {
      color = correct_count * 255 / count;
      graph[*ver_it].SetColor(0, color, 0);
      color = wrong_count * 255 / count;
      false_alarm_graph[*ver_it].SetColor(color, 0, 0);

      // Set accuracy
      float accuracy = (float)(correct_count) / (float)(count);
      std::ostringstream ss;
      ss << std::setprecision(4) << accuracy * 100 << "%";
      graph[*ver_it].accuracy = ss.str();
      
      // std::cout << "Accuracy " << correct_count << " of " << count 
      //   << " False alarm " << wrong_count << std::endl;

      // Overall accuracy
      total_count += count;
      total_correct_count += correct_count;

      // Distrubition of accuracy on degree
      sn::AdjacencyIterator friends_it, friends_it_end;
      boost::tie(friends_it, friends_it_end) = boost::adjacent_vertices(*ver_it, graph);
      int num = std::distance(friends_it, friends_it_end);
      if (num >= hist.size())
        hist.resize(num+1);
      hist[num].push_back(correct_count / count);

    }
  }

  std::cout << "Overall accuracy: " << total_correct_count / total_count * 100.0 << "%" << std::endl;

  // Save result
  // Save graph
  std::string output_dot_file = output_prefix + ".dot";
  sn::WriteGraphToFile(graph, output_dot_file);
  // output_dot_file = output_prefix + "false_alarm.dot";
  // sn::WriteGraphToFile(false_alarm_graph, output_dot_file);

  // Save the album
  // std::string output_album_file = output_prefix + ".alb";
  // sn::WriteAlbumMapToFile(album_map, output_album_file);
  
  // Save the dist
  std::string output_dist_filename = output_prefix + "_accuracy_dist.dat";
  std::ofstream output_dist_file(output_dist_filename.c_str());
  for (int i=0; i<hist.size(); ++i) {
    float mean=0.0, stdev=0.0;
    for (std::vector<float>::const_iterator it = hist[i].begin();
        it != hist[i].end();
        ++it) {
      mean += *it;
      stdev += (*it)*(*it);
    }
    mean = mean / hist[i].size();
    stdev = stdev / hist[i].size() - mean*mean;
    stdev = sqrt(stdev);
    if (hist[i].size()>0)
      output_dist_file << i << " " << mean << " " << stdev << std::endl;
  }

  return 0;
}
