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
#include "classifier/pca_classifier.h"
#include "classifier/train_test_on_album.h"

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
  std::map<std::string, sn::Photo> merge_map;
  merge_map.clear();
  for (sn::AlbumMap::const_iterator album_map_it = album_map.begin();
       album_map_it != album_map.end();
       ++album_map_it) {
    for (sn::Album::const_iterator album_it = album_map_it->second.begin();
         album_it != album_map_it->second.end();
         ++album_it) {
      const sn::Photo& photo = *album_it;
      const std::string& filename = photo.GetFilename();

      // Quick fix
      // TODO check the bug in photo reader or album reader.
      if (filename.size() < 1) {
        continue;
      }

      std::map<std::string, sn::Photo>::iterator it = merge_map.find(filename);
      if (it == merge_map.end()) {
        merge_map[filename] = photo;
      } else if (it->second.GetAssignedId()=="-") {
          it->second = photo;
      }
    }
  }
  sn::Album merged_album;
  for (std::map<std::string, sn::Photo>::const_iterator it = merge_map.begin();
       it != merge_map.end();
       ++it) {
    merged_album.push_back(it->second);
  }

  // Check the merged album
  std::ofstream out_file("/tmp/baseline_all_album.alb");
  for (sn::Album::const_iterator it = merged_album.begin();
       it != merged_album.end();
       ++it) {
    it->WriteToStream(out_file);
  }
  out_file.close();

  // Train
  fn::PCAClassifier classifier;
  std::cout << "Training ..." << std::endl;
  fn::TrainOnSingleAlbum(image_prefix, &merged_album, &classifier);
  
  // Test
  std::cout << "Testing ..." << std::endl;
  fn::TestOnSingleAlbum(image_prefix, &classifier, &merged_album);

  // Give the final result.
}
