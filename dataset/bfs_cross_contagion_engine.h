/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Apr 2012 05:08:58 PM
#
# Filename: cross_contagion_engine.h
#
=========================================*/

#ifndef DATASET_CROSS_CONTAGION_ENGINE_H_
#define DATASET_CROSS_CONTAGION_ENGINE_H_

#include "dataset/base_contagion_engine.h"
#include <queue>

namespace SocialNetwork {
  enum VertexStatus {UNVISITED, VISITED, IN_QUEUE};

class BfsCrossContagionEngine : public BaseContagionEngine {
 public:
  BfsCrossContagionEngine(SocialGraph* graph)
    : graph_(graph),
      BaseContagionEngine(graph) {};
  ~BfsCrossContagionEngine() {};

  int Init(const std::string& start_id);
  int Run();

 private:
  SocialGraph* graph_;
  std::queue<Vertex> Q_;
  std::vector<VertexStatus> vertex_status_;
};
}

#endif  // DATASET_CROSS_CONTAGION_ENGINE_H_

