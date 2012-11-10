/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Nov 2012 10:47:21 AM
#
# Filename: parallel_cross_contagion_engine.h
#
=========================================*/

#ifndef DATASET_PARALLEL_CROSS_CONTAGION_ENGINE_H_
#define DATASET_PARALLEL_CROSS_CONTAGION_ENGINE_H_

#include "dataset/base_contagion_engine.h"
#include "classifier/base_classifier.h"

namespace SocialNetwork {
template <class Classifier>
class ParallelCrossContagionEngine : public BaseContagionEngine {
 public:
  ParallelCrossContagionEngine(const std::string& image_prefix, SocialGraph* graph, AlbumMap* album_map)
    : BaseContagionEngine(image_prefix, graph, album_map) {};
  virtual ~ParallelCrossContagionEngine();

  int Init();
  int Run();

 private:
  int TrainOnSingleVertex(Vertex current);
  // int PropagateOnSingleVertex(Vertex base, Vertex current);
  int PropagateOnSingleVertex(const Vertex base, const Vertex current, AlbumMap* album_map);
  int ReleaseAllClassifiers();

  std::vector<FaceRecognition::BaseClassifier*> classifiers_;
};

}

#include "dataset/parallel_cross_contagion_engine.cc"

#endif  // DATASET_PARALLEL_CROSS_CONTAGION_ENGINE_H_

