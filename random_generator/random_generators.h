/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Nov 2012 10:36:42 PM
#
# Filename: random_generators.h
#
=========================================*/

#ifndef RANDOM_GENERATOR_RANDOM_GENERATORS_H_
#define RANDOM_GENERATOR_RANDOM_GENERATORS_H_

#include <map>
#include <string>
#include <vector>
#include "dataset/social_graph.h"

namespace SocialNetwork {
class RandomGenerators {
 public:
  RandomGenerators() {};
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
 private:
};

}

#endif  // RANDOM_GENERATOR_RANDOM_GENERATORS_H_
