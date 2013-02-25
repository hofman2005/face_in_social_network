/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 25 Feb 2013 04:38:47 PM
#
# Filename: base_contagion_engine.h
#
=========================================*/

#ifndef DATASET_BASE_CONTAGION_ENGINE_H_
#define DATASET_BASE_CONTAGION_ENGINE_H_

// #include <tr1/memory>
#include "dataset/social_graph.h"
#include "classifier/base_classifier.h"
#include "classifier/base_ambiguous_classifier.h"

namespace SocialNetwork {
class BaseContagionEngine {
 public:
  BaseContagionEngine(SocialGraph* graph) : classifier_(NULL) {};
  BaseContagionEngine(const std::string& image_prefix, SocialGraph* graph, AlbumMap* album_map)
    : image_prefix_(image_prefix),
      graph_(graph),
      album_map_(album_map),
      classifier_(NULL) {};
  virtual ~BaseContagionEngine() {};
  
  virtual int Init() {};
  //virtual int Init(const std::string& start_id) {};
  virtual int FirstRun() {};
  virtual int Run() {};
  virtual void SetClassifier(FaceRecognition::BaseAmbiguousClassifier* classifier) {
    classifier_ = classifier;
  }

 protected:
  const std::string image_prefix_;
  SocialGraph* graph_;
  AlbumMap* album_map_;

  FaceRecognition::BaseAmbiguousClassifier* classifier_;
};
}

#endif  // DATASET_BASE_CONTAGION_ENGINE_H_

