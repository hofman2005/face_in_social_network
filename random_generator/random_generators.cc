/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 07 Mar 2012 04:24:33 PM
#
# Filename: random_generators.cc
#
=========================================*/

#include "random_generator/random_generators.h"
#include "assert.h"
#include <utility>
#include <iostream>

namespace SocialNetwork {

void RandomGenerators::AssignID(const AlbumMap &image_map, SocialGraph *graph) {
  // Check
  assert( num_vertices(*graph) == image_map.size() );

  // Assign
  std::pair<VertexIterator, VertexIterator> vertex_it = vertices(*graph);
  AlbumMap::const_iterator image_map_it = image_map.begin();
  // int i=0;
  while (image_map_it != image_map.end()) {
    (*graph)[*(vertex_it.first)].person_id = (*image_map_it).first;
    //(*graph)[*(vertex_it.first)].SetColor(i*255/568, 128, 255-i*255/568);
    (*graph)[*(vertex_it.first)].SetColor(0);

    ++vertex_it.first;
    ++image_map_it;
    // ++i;
  }
}

void RandomGenerators::AlbumGenerator(const SocialGraph &graph,
                                      const AlbumMap &image_map,
                                      AlbumMap *album_map) {
  // Everyone's album contains all the images of himself and his step1 friends
  std::pair<VertexIterator, VertexIterator> vertex_it = vertices(graph);
  std::pair<AdjacencyIterator, AdjacencyIterator> friends_it;
  for (; vertex_it.first != vertex_it.second; ++vertex_it.first) {
    friends_it = boost::adjacent_vertices(*(vertex_it.first), graph);
    const std::string &current_id = graph[*(vertex_it.first)].person_id;
    Album &current_album = (*album_map)[current_id];

    const Album &self_album = image_map.find(current_id)->second;
    current_album.insert(current_album.end(),
                         self_album.begin(),
                         self_album.end());

    for (; friends_it.first != friends_it.second; ++friends_it.first) {
      const std::string &neighbor_id = graph[*(friends_it.first)].person_id;
      const Album &neighbor_album = image_map.find(neighbor_id)->second;
      current_album.insert(current_album.end(), 
                           neighbor_album.begin(), 
                           neighbor_album.end());
    }
  }
}

}
