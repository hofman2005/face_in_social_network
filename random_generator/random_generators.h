/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: random_generators.h
#
=========================================*/

#ifndef RANDOM_GENERATOR_RANDOM_GENERATORS_H_
#define RANDOM_GENERATOR_RANDOM_GENERATORS_H_

#include <map>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "dataset/social_graph.h"

namespace SocialNetwork {
class RandomGenerators {
 public:
  RandomGenerators() {srand( time(NULL) );};
  ~RandomGenerators() {};
 
  void EdgeGenerator(const int factor, 
                     const int dist_thres, 
                     SocialGraph *graph);
  void AssignID(const AlbumMap &image_map, SocialGraph *graph);
  void AlbumGenerator(const SocialGraph &graph,
                      const AlbumMap &image_map,
                      AlbumMap *album_map);
  //void AttachAlbumToGraph(const AlbumMap &album_map, SocialGraph *graph);
  Vertex InfectionSourceGenerator(const int number, SocialGraph *graph);
  void LabelGenerator(const SocialGraph &graph,
                      //const Vertex &start,
                      AlbumMap *album_map); 
  void LabelGenerator2(AlbumMap *image_map, double label_percent);
  void LabelGenerator_WrongLabels(AlbumMap * image_map, double wrong_label_percent);
  void RemoveEdges(SocialGraph* graph, double remove_edge_percent);
 private:
};

}

#endif  // RANDOM_GENERATOR_RANDOM_GENERATORS_H_
