/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Nov 2012 09:55:12 AM
#
# Filename: label_generator.cc
#
=========================================*/

#include "random_generator/random_generators.h"
#include <vector>
#include <stdlib.h>
#include <time.h>

namespace SocialNetwork {
// This function assgin thres% labels for the images of each person.
void RandomGenerators::LabelGenerator(const SocialGraph &graph,
                                      //const Vertex &start,
                                      AlbumMap *album_map) {
  // Assign the labels in album.
  VertexIterator ver_it, ver_it_end; 
  tie(ver_it, ver_it_end) = vertices(graph);
  std::string id;
 
  const int thres = 1000; // thres/10000 percent

  for (; ver_it != ver_it_end; ++ver_it) {
    id = graph[*ver_it].person_id;
    if ( id == "Infection_Source" ) continue;

    Album &album = (*album_map)[graph[*ver_it].person_id];
    srand( time(NULL) );
    for (int i=0; i<album.size(); ++i) {
      int p = rand() % 10000;
      if (p<thres) {
        album[i].SetAssignedId(album[i].GetTrueId(), "God");
      }
    }
  }
}

void RandomGenerators::LabelGenerator2(AlbumMap *image_map, double label_percent) {
  // The total number of images.
  int count = 0;
  for (AlbumMap::iterator it = image_map->begin();
      it != image_map->end();
      ++it) {
    count += it->second.size();
  }


  // Label at least one per subject.
  srand( time(NULL) );
  for (AlbumMap::iterator it = image_map->begin();
      it != image_map->end();
      ++it) {
    Album& album = it->second;
    unsigned int i = rand() % album.size();
    album[i].SetAssignedId(album[i].GetTrueId(), "God");
  }

  count = count * label_percent - image_map->size();

  // const int PRIME = 903531323;
  const int MAX_RETRY = 20;
  std::vector<unsigned int> retry_step(MAX_RETRY);
  for (int i=0; i<MAX_RETRY; ++i) {
    retry_step[i] = rand();
  }

  // Label the percentage
  for (int i=0; i<count; ++i) {
    unsigned int j = rand() % image_map->size();
    AlbumMap::iterator it = image_map->begin();
    advance(it, j);
    unsigned int k = rand() % it->second.size();

    int retry = 1;
    while (retry<20 && (it->second)[k].GetAssignedId().compare("-")) {
      j = (j + retry_step[retry]) % image_map->size();
      it = image_map->begin();
      advance(it,j);
      k = (k + retry_step[retry]) % it->second.size();
      ++retry;
      // std::cout << "Retry: " << retry << std::endl;
    }

    Album& album = it->second;
    album[k].SetAssignedId(album[k].GetTrueId(), "God");
  }
}

}
