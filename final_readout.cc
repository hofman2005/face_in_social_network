/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 01 May 2012 03:26:28 AM
#
# Filename: final_readout.cc
#
=========================================*/

#include <boost/program_options.hpp>
#include "dataset/social_graph.h"

int main(int argc, char **argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  std::string in_alb_filename;
  std::string out_alb_filename;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("input-alb,i", boost::program_options::value<std::string>(&in_alb_filename), 
     "Input album file. (.alb file)")
    ("output-alb,o", boost::program_options::value<std::string>(&out_alb_filename), 
     "Output .alb file.")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, options), vmap);
  po::notify(vmap);

  if (vmap.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }
  if (!vmap.count("input-alb") || 
      !vmap.count("output-alb")) {
    std::cout << options << std::endl;
    return 0;
  }

  // Load the album
  sn::AlbumMap album_map;
  std::cout << "Loading album file: " << in_alb_filename << std::endl;
  sn::ReadAlbumMapFromFile(in_alb_filename, &album_map);

  // Make decision on all images.
  for (sn::AlbumMap::iterator it = album_map.begin();
       it != album_map.end();
       ++it) {
    for (sn::Album::iterator photo_it = it->second.begin();
         photo_it != it->second.end();
         ++photo_it) {
      photo_it->DecideId();
    }
  }
  
  // Save
  std::cout << "Save album to: " << out_alb_filename << std::endl;
  sn::WriteAlbumMapToFile(album_map, out_alb_filename); 

  return 0;
}
