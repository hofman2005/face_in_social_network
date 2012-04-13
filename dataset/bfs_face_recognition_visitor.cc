/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Mar 2012 02:15:29 PM
#
# Filename: bfs_face_recognition_visitor.cc
#
=========================================*/

#include "dataset/social_graph.h"
// #include "classifier/base_classifier.h"
#include "classifier/pca_classifier.h"
#include "dataset/simple_behavior.h"
#include <iostream>

namespace SocialNetwork {
void BfsFaceRecognitionVisitor::discover_vertex(Vertex current,
                                                const SocialGraph &graph) {
  namespace fr = FaceRecognition;
  // Record distance
  (*step_map_)[graph[current].person_id] = step_ + 1;

//   // Train
//   fr::BaseClassifier *classifier = new fr::PCAClassifier();
//   fr::ImageList train_image_list;
//   classifier->Train(train_image_list);
//   
//   // Test on friends
//   fr::ImageList test_image_list;
//   classifier->Identify(&test_image_list);
  // BaseBehavior<fr::PCAClassifier> *behavior = new SimpleBehavior<fr::PCAClassifier>(image_prefix_, album_map_);
  BaseBehavior *behavior = new SimpleBehavior<fr::PCAClassifier>(image_prefix_, album_map_);
  behavior->SelfUpdate(current, graph);
  behavior->Propagate(current, graph);
  delete behavior;
}

void BfsFaceRecognitionVisitor::examine_vertex(Vertex current,
                                               const SocialGraph &graph) {
  step_ = (*step_map_)[graph[current].person_id];
}

}
