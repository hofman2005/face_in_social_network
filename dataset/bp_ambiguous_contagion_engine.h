/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Dec 2012 03:22:31 PM
#
# Filename: bp_ambiguous_contagion_engine.h
#
=========================================*/
#ifndef __BP_AMBIGUOUS_CONTAGIN_ENGINE_H__
#define __BP_AMBIGUOUS_CONTAGIN_ENGINE_H__

#include "dataset/base_contagion_engine.h"
#include "classifier/base_ambiguous_classifier.h"

#include <deque>
#include <set>

namespace SocialNetwork {
template <class AmbiguousClassifier>
class BeliefPropagationAmbiguousContagionEngine : public BaseContagionEngine {
  public:
    BeliefPropagationAmbiguousContagionEngine(const std::string& image_prefix,
        SocialGraph* graph,
        AlbumMap* album_map)
      : classifier_(NULL), 
      BaseContagionEngine(image_prefix, graph, album_map) {};
    virtual ~BeliefPropagationAmbiguousContagionEngine() {
      if (classifier_)
        delete classifier_;
    };

    virtual int Init();
    virtual int Run();

    virtual int FirstRun();
    
  private:
    int AppendTrainingImageList(Vertex current,
                                FaceRecognition::AmbiguousImageList * image_list,
                                bool load_image = true);
    int TrainOnSingleVertex(Vertex current);
    int PrepareRansacTrainingImageList(Vertex current,
        FaceRecognition::AmbiguousImageList * image_list,
        double select_ratio);
    int RansacOnSingleVertex(Vertex current);
    int PropagateOnSingleVertex(const Vertex base, const Vertex current, AlbumMap* album_map);
    bool MakeDecisionOnSingleVertex(Album* album);

    FaceRecognition::BaseAmbiguousClassifier* classifier_;
    std::deque<Vertex> visit_queue_1_, visit_queue_2_;
    std::set<Vertex> need_decision_set_;
};
}

#include "dataset/bp_ambiguous_contagion_engine.cc"

#endif  // __BP_AMBIGUOUS_CONTAGIN_ENGINE_H__
