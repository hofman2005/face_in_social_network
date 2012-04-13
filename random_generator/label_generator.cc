/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Mar 2012 08:52:23 AM
#
# Filename: label_generator.cc
#
=========================================*/

#include "random_generator/random_generators.h"
#include <stdlib.h>
#include <time.h>

namespace SocialNetwork {
// This function assgin thres% labels for the images of each person.
void RandomGenerators::LabelGenerator(const SocialGraph &graph,
                                      const Vertex &start,
                                      AlbumMap *album_map) {
  // Assign the labels in album.
  VertexIterator ver_it, ver_it_end; 
  tie(ver_it, ver_it_end) = vertices(graph);
  std::string id;
 
  const int thres = 3000; // thres/10000 percent

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

}
