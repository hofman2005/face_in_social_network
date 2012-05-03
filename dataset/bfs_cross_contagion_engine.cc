/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 16 Apr 2012 07:45:34 AM
#
# Filename: cross_contagion_engine.cc
#
=========================================*/

#ifndef DATASET_BFS_CROSS_CONTAGION_ENGINE_CC_
#define DATASET_BFS_CROSS_CONTAGION_ENGINE_CC_

#include "bfs_cross_contagion_engine.h"

namespace SocialNetwork {
template <class Classifier>
BfsCrossContagionEngine<Classifier>::~BfsCrossContagionEngine() {
  for (std::vector<FaceRecognition::BaseClassifier*>::iterator it = classifiers_.begin();
       it != classifiers_.end();
       ++it) {
    if (*it) delete *it;
  }
}
  
template <class Classifier>
int BfsCrossContagionEngine<Classifier>::Init(const std::string& start_id) {
  // Initialize the queue
  vertex_status_.clear();
  vertex_status_.resize(num_vertices(*graph_), UNVISITED);

  VertexIterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(*graph_); vi!=vi_end; ++vi) {
    if ((*graph_)[*vi].person_id == start_id) {
      break;
    }  
  }
  // Check if found the Infection_Source
  assert(vi!=vi_end);

  // Add the start point to the queue
  assert(Q_.size() == 0);
  Q_.push(*vi);

  // Initialize the classifiers
  classifiers_.clear();
  //classifiers_.resize(num_vertices(*graph_), NULL);
  for (int i=0; i<num_vertices(*graph_); ++i) {
    classifiers_.push_back(new Classifier);
  }
  return 0;
}

template <class Classifier>
int BfsCrossContagionEngine<Classifier>::Run() {
  AdjacencyIterator adj, adj_end;
  int count = 0;
  while (Q_.size() > 0) {
    Vertex& v = Q_.front();
    Q_.pop();

    // Train on the vertex
    TrainOnSingleVertex(v);
    // Test on the vertex
    PropagateOnSingleVertex(v, v);

    // Discover unvisited neighbors
    for (tie(adj, adj_end) = adjacent_vertices(v, *graph_); adj != adj_end; ++adj) {
      if (vertex_status_[*adj] == UNVISITED) {
        Q_.push(*adj);
        vertex_status_[*adj] = IN_QUEUE;
      }
      // Test on the neighbors
      int num_label_changed = PropagateOnSingleVertex(v, *adj);

      // Cross contagion
      if (num_label_changed>0 && vertex_status_[*adj] != IN_QUEUE) {
        Q_.push(*adj);
        vertex_status_[*adj] = IN_QUEUE;
      }
    }
    vertex_status_[v] = VISITED;

    // Output some intermediate result?
    // Save graph
    //std::string output_dot_file = "/tmp/" + ".dot";
    // char output_dot_file[80];
    // sprintf(output_dot_file, "/tmp/sn_%5.5d.dot", count);
    // WriteGraphToFile(graph_, output_dot_file);
    // Save the album
    char output_album_file[80];
    sprintf(output_album_file, "/tmp/sn_%5.5d.alb", count);
    WriteAlbumMapToFile(*album_map_, output_album_file);

    ++count;
  }
  return 0;
}

template <class Classifier>
int BfsCrossContagionEngine<Classifier>::TrainOnSingleVertex(Vertex current) {
  // Do not process the dummy source
  if ((*graph_)[current].person_id == "Infection_Source") return 0;
  std::cout << "Training on " << (*graph_)[current].person_id << std::endl;

  if (classifiers_[current]->IsTrained()) {
    // Train on updated labels.
    Album &album = (*album_map_)[(*graph_)[current].person_id]; 
    FaceRecognition::ImageList image_list;
    std::string id;
    for (Album::iterator it = album.begin(); it != album.end(); ++it) {
      id = (*it).GetAssignedId();
      // if ( id == "-") continue;
      cv::Mat image;
      image_list.push_back(std::make_pair(&image, id));
    }
     classifiers_[current]->TrainWithUpdatedLabels(image_list);
  } else {
    // Load images and train 
    Album &album = (*album_map_)[(*graph_)[current].person_id]; 
    FaceRecognition::ImageList image_list;
    std::string id;
    // Load images
    for (Album::iterator it = album.begin(); it != album.end(); ++it) {
      id = (*it).GetAssignedId();
      // if ( id == "-") continue;
      cv::Mat *image = new cv::Mat;
      *image  = (*it).GetImage(image_prefix_);
      image_list.push_back(std::make_pair(image, id));
    }
    classifiers_[current]->Train(image_list);
    for (FaceRecognition::ImageList::iterator it = image_list.begin();
         it!=image_list.end();
         ++it) {
      delete (*it).first;
    }
  }

  return 0;
}

template <class Classifier>
int BfsCrossContagionEngine<Classifier>::PropagateOnSingleVertex
  (Vertex base, Vertex current) {
  // Do not process the dummy source
  if ((*graph_)[base].person_id    == "Infection_Source") return 0;
  if ((*graph_)[current].person_id == "Infection_Source") return 0;
  std::cout << "Propagate to " << (*graph_)[current].person_id << std::endl;

  FaceRecognition::BaseClassifier* classifier = classifiers_[base]; 

  // const std::string &image_prefix = BaseBehavior<Classifier>::image_prefix_;
  const std::string &image_prefix = image_prefix_;
  // Album &album = (*BaseBehavior<Classifier>::album_map_)[graph[current].person_id]; 
  Album &album = (*album_map_)[(*graph_)[current].person_id]; 
  std::string id;
  std::string assigned_by;

  float count = 0;
  float correct_count = 0;
  int num_label_changed = 0;
  for (Album::iterator it = album.begin(); it != album.end(); ++it) {
    id = (*it).GetAssignedId();
    if ( id != "-") continue;
    
    // // Do not change those initial labels
    // assigned_by = (*it).GetAssignedBy();
    // if (id == "God") continue;

    cv::Mat image  = (*it).GetImage(image_prefix);

    std::string res_id;
    // if (BaseBehavior<Classifier>::classifier_.Identify(image, &res_id)) {
    if (classifier->Identify(image, &res_id)) {
      // If identification is successful
      (*it).SetAssignedId(res_id, (*graph_)[current].person_id); 
    };

    // Simple analysis
    ++count;
    if (res_id == (*it).GetTrueId()) ++correct_count;

    if (res_id != id) ++num_label_changed;
  }
  std::cout << " Accuracy: " << correct_count/count*100 << "%" << std::endl;

  return num_label_changed;
}


}

#endif  // DATASET_BFS_CROSS_CONTAGION_ENGINE_CC_

