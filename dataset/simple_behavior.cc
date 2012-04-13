/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Mar 2012 02:14:51 PM
#
# Filename: simple_behavior.cc
#
=========================================*/

#ifndef DATASET_SIMPLE_BEHAVIOR_CC_
#define DATASET_SIMPLE_BEHAVIOR_CC_

#include "dataset/simple_behavior.h"
#include "classifier/base_classifier.h"
#include <iostream>
#include <cv.h>
#include <highgui.h>

namespace SocialNetwork {
template <class Classifier>
int SimpleBehavior<Classifier>::SelfUpdate(Vertex current, const SocialGraph &graph) {
  namespace fr = FaceRecognition;
  
  std::cout << "On " << graph[current].person_id << std::endl;
  // Don't process the dummy source
  if (graph[current].person_id == "Infection_Source") return 0;

  // Pack labeled training images
  // const std::string &image_prefix = BaseBehavior<Classifier>::image_prefix_;
  const std::string &image_prefix = BaseBehavior::image_prefix_;
  // Album &album = (*BaseBehavior<Classifier>::album_map_)[graph[current].person_id]; 
  Album &album = (*BaseBehavior::album_map_)[graph[current].person_id]; 
  fr::ImageList image_list;
  std::string id;

  for (Album::iterator it = album.begin(); it != album.end(); ++it) {
    id = (*it).GetAssignedId();
    if ( id == "-") continue;
    cv::Mat *image = new cv::Mat;
    *image  = (*it).GetImage(image_prefix);
    image_list.push_back(std::make_pair(image, id));
  }

  // Train
  // BaseBehavior<Classifier>::classifier_.Train(image_list);
  classifier_->Train(image_list);

  // Clean
  for (fr::ImageList::iterator it = image_list.begin(); it!=image_list.end(); ++it) {
    delete (*it).first;
  }
  return 0;
}

template <class Classifier>
int SimpleBehavior<Classifier>::Propagate(Vertex current, const SocialGraph &graph) {
  namespace fr = FaceRecognition;
  
  // Don't process the dummy source
  if (graph[current].person_id == "Infection_Source") return 0;

  // On self
  PropagateOnSingleVertex(current, graph);

  // On neighbor
  AdjacencyIterator it, it_end;
  tie(it, it_end) = boost::adjacent_vertices(current, graph);
  for (; it != it_end; ++it) {
    PropagateOnSingleVertex(*it, graph);
  }
  
  // Record result
  
  return 0;
}

template <class Classifier>
int SimpleBehavior<Classifier>::PropagateOnSingleVertex(Vertex current, 
                                                        const SocialGraph &graph) {
  // Do not process the dummy source
  if (graph[current].person_id == "Infection_Source") return 0;
  std::cout << "Propagate to " << graph[current].person_id << std::endl;

  // const std::string &image_prefix = BaseBehavior<Classifier>::image_prefix_;
  const std::string &image_prefix = image_prefix_;
  // Album &album = (*BaseBehavior<Classifier>::album_map_)[graph[current].person_id]; 
  Album &album = (*album_map_)[graph[current].person_id]; 
  std::string id;

  float count = 0;
  float correct_count = 0;
  for (Album::iterator it = album.begin(); it != album.end(); ++it) {
    id = (*it).GetAssignedId();
    if ( id != "-") continue;
    cv::Mat image  = (*it).GetImage(image_prefix);

    std::string res_id;
    // if (BaseBehavior<Classifier>::classifier_.Identify(image, &res_id)) {
    if (classifier_->Identify(image, &res_id)) {
      // If identification is successful
      (*it).SetAssignedId(res_id, graph[current].person_id); 
    };

    // Simple analysis
    ++count;
    if (res_id == (*it).GetTrueId()) ++correct_count;
  }
  std::cout << " Accuracy: " << correct_count/count*100 << "%" << std::endl;

  return 0;
}

}

#endif  // DATASET_SIMPLE_BEHAVIOR_CC_
