/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Nov 2012 12:51:11 PM
#
# Filename: bp_contagion_engine.h
#
=========================================*/

#ifndef __BP_CONTAGION_ENGINE_H__
#define __BP_CONTAGION_ENGINE_H__

#include "dataset/base_contagion_engine.h"
#include "classifier/base_classifier.h"

#include <deque>

namespace SocialNetwork {
template <class Classifier>
class BeliefPropagationContagionEngine : public BaseContagionEngine {
  public:
    BeliefPropagationContagionEngine(const std::string& image_prefix,
                                     SocialGraph* graph,
                                     AlbumMap* album_map)
      : BaseContagionEngine(image_prefix, graph, album_map) {};
    virtual ~BeliefPropagationContagionEngine();

    int Init();
    int Run();

  private:
    int AppendTrainingImageList(Vertex current,
                                FaceRecognition::ImageList * image_list,
                                bool load_image = true);
    int TrainOnSingleVertex(Vertex current);
    // int PropagateOnSingleVertex(Vertex base, Vertex current);
    int PropagateOnSingleVertex(const Vertex base, const Vertex current, AlbumMap* album_map);
    int ReleaseAllClassifiers();

    std::vector<FaceRecognition::BaseClassifier*> classifiers_;

    std::deque<Vertex> visit_queue_1_, visit_queue_2_;
};
}

#include "dataset/bp_contagion_engine.cc"

#endif  // __BP_CONTAGION_ENGINE_H__
