/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 11 Jan 2013 02:36:44 PM
#
# Filename: bp_contagion_engine.h
#
=========================================*/

#ifndef __BP_CONTAGION_ENGINE_H__
#define __BP_CONTAGION_ENGINE_H__

#include "dataset/base_contagion_engine.h"
#include "classifier/base_classifier.h"

#include <deque>
#include <set>

namespace SocialNetwork {
template <class Classifier>
class BeliefPropagationContagionEngine : public BaseContagionEngine {
  public:
    BeliefPropagationContagionEngine(const std::string& image_prefix,
                                     SocialGraph* graph,
                                     AlbumMap* album_map)
      : classifier_(NULL), 
        BaseContagionEngine(image_prefix, graph, album_map){};
    virtual ~BeliefPropagationContagionEngine();

    virtual int Init();
    virtual int Run();
    virtual int FirstRun();

  private:
    int AppendTrainingImageList(Vertex current,
                                FaceRecognition::ImageList * image_list,
                                bool load_image = true);
    int TrainOnSingleVertex(Vertex current);
    // int PropagateOnSingleVertex(Vertex base, Vertex current);
    int PropagateOnSingleVertex(const Vertex base, const Vertex current, AlbumMap* album_map);
    bool MakeDecisionOnSingleVertex(Album* album);
    int ReleaseAllClassifiers();

    int PrepareRansacTrainingImageList(Vertex current,
        FaceRecognition::ImageList * image_list,
        double select_ratio);
    int RansacOnSingleVertex(Vertex current);

    std::vector<FaceRecognition::BaseClassifier*> classifiers_;
    FaceRecognition::BaseClassifier* classifier_;

    std::deque<Vertex> visit_queue_1_, visit_queue_2_;
    std::set<Vertex> need_decision_set_;
};
}

#include "dataset/bp_contagion_engine.cc"

#endif  // __BP_CONTAGION_ENGINE_H__
