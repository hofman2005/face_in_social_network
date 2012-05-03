/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 16 Apr 2012 09:41:34 AM
#
# Filename: cross_contagion_engine.h
#
=========================================*/

#ifndef DATASET_CROSS_CONTAGION_ENGINE_H_
#define DATASET_CROSS_CONTAGION_ENGINE_H_

#include "dataset/base_contagion_engine.h"
#include "classifier/base_classifier.h"
#include <queue>

namespace SocialNetwork {
  enum VertexStatus {UNVISITED, VISITED, IN_QUEUE};

template <class Classifier>
class BfsCrossContagionEngine : public BaseContagionEngine {
 public:
  BfsCrossContagionEngine(const std::string& image_prefix, SocialGraph* graph, AlbumMap* album_map)
    : // image_prefix_(image_prefix),
      // graph_(graph),
      // album_map_(album_map),
      BaseContagionEngine(image_prefix, graph, album_map) {};
  ~BfsCrossContagionEngine();

  int Init(const std::string& start_id);
  int Run();

 private:
  int TrainOnSingleVertex(Vertex current);
  int PropagateOnSingleVertex(Vertex base, Vertex current);


  std::queue<Vertex> Q_;
  std::vector<VertexStatus> vertex_status_;
  std::vector<FaceRecognition::BaseClassifier*> classifiers_;
};
}

#include "dataset/bfs_cross_contagion_engine.cc"

#endif  // DATASET_CROSS_CONTAGION_ENGINE_H_

