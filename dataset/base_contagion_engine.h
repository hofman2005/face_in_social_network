/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Apr 2012 03:14:09 PM
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
  virtual ~BaseContagionEngine() {};
  
  virtual int Init(const std::string& start_id) {};
  virtual int Run() {};
};
}

#endif  // DATASET_BASE_CONTAGION_ENGINE_H_

