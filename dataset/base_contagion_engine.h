/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 16 Apr 2012 10:06:45 AM
#
# Filename: base_contagion_engine.h
#
=========================================*/

#ifndef DATASET_BASE_CONTAGION_ENGINE_H_
#define DATASET_BASE_CONTAGION_ENGINE_H_

#include "dataset/social_graph.h"

namespace SocialNetwork {
class BaseContagionEngine {
 public:
  BaseContagionEngine(SocialGraph* graph) {};
  BaseContagionEngine(const std::string& image_prefix, SocialGraph* graph, AlbumMap* album_map)
    : image_prefix_(image_prefix),
      graph_(graph),
      album_map_(album_map) {};
  virtual ~BaseContagionEngine() {};
  
  virtual int Init() {};
  //virtual int Init(const std::string& start_id) {};
  virtual int Run() {};

 protected:
  const std::string image_prefix_;
  SocialGraph* graph_;
  AlbumMap* album_map_;};
}

#endif  // DATASET_BASE_CONTAGION_ENGINE_H_

