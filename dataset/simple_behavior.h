/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Mar 2012 02:58:20 PM
#
# Filename: simple_behavior.h
#
=========================================*/

#ifndef DATASET_SIMPLE_HEHAVIOR_H_
#define DATASET_SIMPLE_HEHAVIOR_H_

#include "dataset/base_behavior.h"

namespace SocialNetwork {
// Simple behavior will use image of the current node with labels for train,
// and test on all unlabled images of current node and its neighbors 
//
// All changes made are recorded in album_map_
template <class Classifier>
//class SimpleBehavior : public BaseBehavior<Classifier> {
class SimpleBehavior : public BaseBehavior {
 public:
  SimpleBehavior(const std::string &image_prefix,
                 AlbumMap *album_map) 
    //: BaseBehavior<Classifier>(image_prefix, album_map) {};
    : BaseBehavior(image_prefix, album_map, new Classifier) {};
  ~SimpleBehavior() {if (classifier_) delete classifier_;};

  int SelfUpdate(Vertex current, const SocialGraph &graph);
  int Propagate(Vertex current, const SocialGraph &graph);
  int PropagateOnSingleVertex(Vertex current,
                              const SocialGraph &graph);
};
}

#include "dataset/simple_behavior.cc"

#endif  // DATASET_SIMPLE_HEHAVIOR_H_


