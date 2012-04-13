/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Mar 2012 02:37:30 PM
#
# Filename: behavior.h
#
=========================================*/

#ifndef DATASET_BASE_BEHAVIOR_H_
#define DATASET_BASE_BEHAVIOR_H_

#include "dataset/social_graph.h"
#include "classifier/base_classifier.h"

namespace SocialNetwork {
  namespace fr=FaceRecognition;
//template <class Classifier>
class BaseBehavior {
 public:
  BaseBehavior(const std::string &image_prefix,
               AlbumMap *album_map,
               fr::BaseClassifier *classifier = NULL) 
    : image_prefix_(image_prefix),
      album_map_(album_map),
      classifier_(classifier) {};
  virtual ~BaseBehavior() {};

  virtual int SelfUpdate(Vertex current, const SocialGraph &graph) {};
  virtual int Propagate(Vertex current, const SocialGraph &graph) {};

 protected:
  const std::string &image_prefix_;
  AlbumMap *album_map_;
  //Classifier classifier_;
  fr::BaseClassifier *classifier_;
};
}

#endif  // DATASET_BASE_BEHAVIOR_H_
