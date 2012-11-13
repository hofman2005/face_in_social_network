/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 13 Nov 2012 05:13:29 PM
#
# Filename: random_generators.cc
#
=========================================*/

#include "random_generator/random_generators.h"
#include "assert.h"
#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace SocialNetwork {

  using namespace std;

void RandomGenerators::AssignID(const AlbumMap &image_map, SocialGraph *graph) {
  // Check
  assert( num_vertices(*graph) == image_map.size() );

  // Assign
  std::pair<VertexIterator, VertexIterator> vertex_it = vertices(*graph);
  AlbumMap::const_iterator image_map_it = image_map.begin();

  // Random assign ID
  srand( time(NULL) ); 
  vector<string> ids;
  while (image_map_it != image_map.end()) {
    ids.push_back((*image_map_it).first);
    ++image_map_it;
  }

  unsigned int count = ids.size();
  while (vertex_it.first != vertex_it.second && count >=1) {
    unsigned int i = rand() % count;
    (*graph)[*(vertex_it.first)].person_id = ids[i];
    (*graph)[*(vertex_it.first)].SetColor(0);

    graph->label_map_[ids[i]] = *(vertex_it.first);

    swap(ids[i], ids[count-1]);

    ++vertex_it.first;
    --count;
  }

  // while (image_map_it != image_map.end()) {
  //   (*graph)[*(vertex_it.first)].person_id = (*image_map_it).first;
  //   //(*graph)[*(vertex_it.first)].SetColor(i*255/568, 128, 255-i*255/568);
  //   (*graph)[*(vertex_it.first)].SetColor(0);

  //   ++vertex_it.first;
  //   ++image_map_it;
  //   // ++i;
  // }
}

void RandomGenerators::AlbumGenerator(const SocialGraph &graph,
                                      const AlbumMap &image_map,
                                      AlbumMap *album_map) {
  // // Everyone's album contains all the images of himself and his step1 friends
  // std::pair<VertexIterator, VertexIterator> vertex_it = vertices(graph);
  // std::pair<AdjacencyIterator, AdjacencyIterator> friends_it;
  // for (; vertex_it.first != vertex_it.second; ++vertex_it.first) {
  //   friends_it = boost::adjacent_vertices(*(vertex_it.first), graph);
  //   const std::string &current_id = graph[*(vertex_it.first)].person_id;
  //   Album &current_album = (*album_map)[current_id];

  //   const Album &self_album = image_map.find(current_id)->second;
  //   current_album.insert(current_album.end(),
  //                        self_album.begin(),
  //                        self_album.end());

  //   for (; friends_it.first != friends_it.second; ++friends_it.first) {
  //     const std::string &neighbor_id = graph[*(friends_it.first)].person_id;
  //     const Album &neighbor_album = image_map.find(neighbor_id)->second;
  //     current_album.insert(current_album.end(), 
  //                          neighbor_album.begin(), 
  //                          neighbor_album.end());
  //   }
  // }
  
  // The probability of an image appears in one album depends on the distance
  // between the image's owner to the album's owner.
  std::deque<Vertex> visit_queue_1, visit_queue_2;
  int count = 0;
  for (AlbumMap::const_iterator image_it = image_map.begin();
       image_it != image_map.end();
       ++image_it, ++count) {
    const std::string& current_id = image_it->first;
    // std::cout << "Generating album. Processing image set: " 
    std::cout << count << " of " << image_map.size() << "\r";

    Vertex current_vertex = graph.label_map_.find(current_id)->second;

    visit_queue_1.clear();
    visit_queue_2.clear();
    visit_queue_1.push_back(current_vertex);

    int dist = 0;
    const int FARTHEST_APPEAR = 2;
    while (dist <= FARTHEST_APPEAR) {
      while (!visit_queue_1.empty()) {
        Vertex vertex = visit_queue_1.front();
        visit_queue_1.pop_front();
        // Decide if add image.
        const std::string &current_id = graph[vertex].person_id;
        Album &current_album = (*album_map)[current_id];
        for (Album::const_iterator photo_it = image_it->second.begin();
            photo_it != image_it->second.end();
            ++photo_it) {
          int prob = rand() % 10000;
          if (dist == 0 && prob < 50 ||
              dist == 1 && prob < 10 ||
              dist == 2 && prob < 5) {
            current_album.push_back(*photo_it);
          }
        }

        // Maintain the queue.
        std::pair<AdjacencyIterator, AdjacencyIterator> friends_it = boost::adjacent_vertices(vertex, graph);
        for (; friends_it.first != friends_it.second; ++friends_it.first) {
          visit_queue_2.push_back(*friends_it.first);
        }
      }
      visit_queue_1.swap(visit_queue_2);
      ++dist;
    }
  }
  std::cout << "Album generation done. " << std::endl;
}

}
