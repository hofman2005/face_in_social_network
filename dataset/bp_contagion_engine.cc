/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 11 Nov 2012 10:51:02 PM
#
# Filename: bp_contagion_engine.cc
#
=========================================*/

#ifndef __DATASET_BP_CONTAGION_ENGINE_CC__
#define __DATASET_BP_CONTAGION_ENGINE_CC__

#include "dataset/bp_contagion_engine.h"

namespace SocialNetwork {
template <class Classifier>
BeliefPropagationContagionEngine<Classifier>::~BeliefPropagationContagionEngine() {
  ReleaseAllClassifiers();
}

template <class Classifier>
int BeliefPropagationContagionEngine<Classifier>::ReleaseAllClassifiers() {
  for (std::vector<FaceRecognition::BaseClassifier*>::iterator it = classifiers_.begin();
       it != classifiers_.end();
       ++it) {
    if (*it) delete *it;
  }
  return 0;
}

template <class Classifier>
int BeliefPropagationContagionEngine<Classifier>::Init() {
  // Initialize the classifiers
  ReleaseAllClassifiers();
  classifiers_.clear();

  for (int i=0; i<num_vertices(*graph_); ++i) {
    classifiers_.push_back(new Classifier);
  }

  return 0;
}

template <class Classifier>
int BeliefPropagationContagionEngine<Classifier>::Run() {
  visit_queue_1_.clear();
  visit_queue_2_.clear();

  // Set initial visit queue.
  VertexIterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(*graph_);
      vi != vi_end;
      ++vi) {
    Album& album = (*album_map_)[(*graph_)[*vi].person_id];
    for (Album::const_iterator it = album.begin();
        it != album.end();
        ++it) {
      if (!it->GetAssignedId().compare("-")) {
        visit_queue_1_.push_back(*vi);
        break;
      }
    }
  }

  // Main loop
  const int MAX_ITER = 10;
  int iter = 0;
  while (iter < MAX_ITER) {
    AlbumMap album_copy = *album_map_;
    int subcount = 0;
    need_decision_set_.clear();
    while (!visit_queue_1_.empty()) {
      std::cout << "Iter: " << iter
        << " Length of visit queue_1: " << visit_queue_1_.size()
        << " Length of need_desicion: " << need_decision_set_.size() << std::endl;
      Vertex current = visit_queue_1_.front();
      visit_queue_1_.pop_front();

      // Train
      TrainOnSingleVertex(current);
      
      // Propagate
      PropagateOnSingleVertex(current, current, &album_copy);
      need_decision_set_.insert(current);
      AdjacencyIterator adj, adj_end;
      for (tie(adj, adj_end) = adjacent_vertices(current, *graph_); 
          adj != adj_end;
          ++adj) {
        PropagateOnSingleVertex(current, *adj, &album_copy);
        need_decision_set_.insert(*adj);
      }

      // Save the album
      char output_album_file[80];
      sprintf(output_album_file, "/tmp/sn_%5.5d_%5.5d.alb", iter, subcount);
      WriteAlbumMapToFile(album_copy, output_album_file);
      ++subcount;
    }

    // Make decision
    for (std::set<Vertex>::iterator it = need_decision_set_.begin();
        it != need_decision_set_.end();
        ++it) {
      Album& album = album_copy[(*graph_)[*it].person_id];
      bool changed = MakeDecisionOnSingleVertex(&album);
      if (changed)
        visit_queue_2_.push_back(*it);
    }

    visit_queue_1_.swap(visit_queue_2_);
    album_map_->swap(album_copy);
    ++iter;

    std::cout << "Number of points need to be visited in new iteration: " << visit_queue_1_.size() << std::endl;
  }

  return 0;
}

template <class Classifier>
bool BeliefPropagationContagionEngine<Classifier>::MakeDecisionOnSingleVertex (Album* album) {
  bool changed = false;
  float count = 0;
  float correct_count = 0;
  float decision_count = 0;
  for (Album::iterator it = album->begin();
      it != album->end();
      ++it) {
    FaceRecognition::PhotoResult& res = it->GetPhotoRes();
    std::string res_id = "-";
    const double threshold = 3.0f;
    if (res.GetNumRecord() > 1) {
      std::string id_0, id_1;
      double score_0, score_1;
      res.GetSortedDecision(0, &score_0, &id_0);
      res.GetSortedDecision(1, &score_1, &id_1);
      if (fabs(score_1 / score_0) > threshold) {
        res_id = id_0;
        if (it->GetAssignedId() != res_id) {
          it->SetAssignedId(res_id, "n/a");
          changed = true;
        }
      }
    }

    // Simple analysis
    ++count;
    if (res_id == it->GetTrueId()) 
      ++correct_count;
    if (res_id != "-") 
      ++decision_count;
  }
  std::cout << " Accuracy: " << correct_count/decision_count*100 << "%" << std::endl;
  std::cout << " Decision made on " << decision_count << " of " << count << std::endl;

  return changed;
}

template <class Classifier>
int BeliefPropagationContagionEngine<Classifier>::AppendTrainingImageList
  (Vertex current, 
   FaceRecognition::ImageList * image_list,
   bool load_image) {
    Album& album = (*album_map_)[(*graph_)[current].person_id];
    std::string id;
    for (Album::iterator it = album.begin(); it != album.end(); ++it) {
      id = (*it).GetAssignedId();
      // The "-" images will be omited in TrainWithUpdatedLabels
      // if ( id == "-" )
      //   continue;
      cv::Mat * image = new cv::Mat;
      if (load_image) {
        *image = (*it).GetImage(image_prefix_);
      }
      image_list->push_back(std::make_pair(image, id));
    }

    return 0;
}

template <class Classifier>
int BeliefPropagationContagionEngine<Classifier>::TrainOnSingleVertex(Vertex current) {
  // Do not process the dummy source
  // if ((*graph_)[current].person_id == "Infection_Source") return 0;
  
  std::cout << "Training on " << (*graph_)[current].person_id << std::endl;

  // Prepare the image list
  bool load_image = !classifiers_[current]->IsTrained();
  FaceRecognition::ImageList image_list;
  AppendTrainingImageList(current, &image_list);

  // Append the images in friends.
  // AdjacencyIterator adj, adj_end;
  // for (tie(adj, adj_end) = adjacent_vertices(current, *graph_); 
  //     adj != adj_end;
  //     ++adj) {
  //   AppendTrainingImageList(*adj, &image_list, load_image);
  // }

  // Train
  if (classifiers_[current]->IsTrained()) {
    classifiers_[current]->TrainWithUpdatedLabels(image_list);
  } else {
    classifiers_[current]->Train(image_list);
  }

  // Release resources
  for (FaceRecognition::ImageList::iterator it = image_list.begin();
       it!=image_list.end();
       ++it) {
    delete it->first;
  }

  return 0;
}

template <class Classifier>
int BeliefPropagationContagionEngine<Classifier>::PropagateOnSingleVertex
  (const Vertex base, 
   const Vertex current, 
   AlbumMap* album_map) {
  // TODO
  // Do not process the dummy source
  // if ((*graph_)[base].person_id    == "Infection_Source") return 0;
  // if ((*graph_)[current].person_id == "Infection_Source") return 0;
  
  std::cout << "Propagate to " << (*graph_)[current].person_id << std::endl;

  FaceRecognition::BaseClassifier* pclassifier = classifiers_[base]; 

  Album &album = (*album_map)[(*graph_)[current].person_id]; 
  std::string id;
  std::string assigned_by;

  float count = 0;
  float correct_count = 0;
  float decision_count = 0;
  int num_label_changed = 0;
  for (Album::iterator it = album.begin(); it != album.end(); ++it) {
    id = it->GetAssignedId();

    // Do not change those initial labels
    assigned_by = it->GetAssignedBy();
    // if (assigned_by == "God") continue;
    if (assigned_by != "-") continue;

    // Load the image
    cv::Mat image  = it->GetImage(image_prefix_);

    // Identify
    FaceRecognition::PhotoResult& res = it->GetPhotoRes();
    pclassifier->Identify(image, &res); 

    // // Make a decision.
    // std::string res_id = "-";
    // const double threshold = 3.0f;
    // // if (vec[1].second / vec[0].second > threshold) {
    // //   res_id = vec[0].first;
    // //   (*it).SetAssignedId(res_id, (*graph_)[current].person_id); 
    // // } else {
    // //   res_id = "-";
    // // }
    // if (res.GetNumRecord() > 1) {
    //   std::string id_0, id_1;
    //   double score_0, score_1;
    //   res.GetSortedDecision(0, &score_0, &id_0);
    //   res.GetSortedDecision(1, &score_1, &id_1);
    //   if (score_1 / score_0 > threshold) {
    //     res_id = id_0;
    //     (*it).SetAssignedId(res_id, (*graph_)[current].person_id);
    //   }
    // }

    // // Simple analysis
    // ++count;
    // if (res_id == (*it).GetTrueId()) ++correct_count;
    // if (res_id != "-") ++decision_count;

    // if (res_id != id) ++num_label_changed;
  }
  // std::cout << " Accuracy: " << correct_count/decision_count*100 << "%" << std::endl;
  // std::cout << " Decision made on " << decision_count << " of " << count << std::endl;

  // Save the album
  // char output_album_file[80];
  // sprintf(output_album_file, "/tmp/sn_aa.alb");
  // WriteAlbumMapToFile(*album_map, output_album_file);

  // return num_label_changed;
  return 0;
}


}

#endif  // __DATASET_BP_CONTAGION_ENGINE_CC__
