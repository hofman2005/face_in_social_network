/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 13 Dec 2012 10:55:38 AM
#
# Filename: bp_ambiguous_contagion_engine.cc
#
=========================================*/
#ifndef __DATASET_BP_AMBIGUOUS_CONTAGION_ENGINE_H__
#define __DATASET_BP_AMBIGUOUS_CONTAGION_ENGINE_H__

#include "dataset/bp_ambiguous_contagion_engine.h"
#include <stdlib.h>
#include <time.h>

namespace SocialNetwork {
template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::Init() {
  srand( time(NULL) );
  return 0;
}

// Generate the initial label list and weights.
template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::FirstRun() {
  VertexIterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(*graph_);
      vi != vi_end;
      ++vi) {
    // Use Ransac to generate the initial labels and weights.
    RansacOnSingleVertex(*vi);
  }

  char output_album_file[80];
  sprintf(output_album_file, "/tmp/sn_init_stage.alb");
  WriteAlbumMapToFile(*album_map_, output_album_file);

  return 0;
}

template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::Run() {
  FirstRun();

  visit_queue_1_.clear();
  visit_queue_2_.clear();

  // Set initial visit queue.
  VertexIterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(*graph_);
      vi != vi_end;
      ++vi) {
    visit_queue_1_.push_back(*vi);
  }

  // Main loop
  const int MAX_ITER = 2;
  int iter = 0;
  while (iter < MAX_ITER) {
    int subcount = 0;
    AlbumMap album_copy = *album_map_;
    need_decision_set_.clear();
    while (!visit_queue_1_.empty()) {
      std::cout << "Iter: " << iter
        << " Length of visit queue_1: " << visit_queue_1_.size()
        << " Length of need_desicion: " << need_decision_set_.size() << std::endl;
      Vertex current = visit_queue_1_.front();
      visit_queue_1_.pop_front();

      if (classifier_)
        delete classifier_;
      classifier_ = new AmbiguousClassifier;

      // Train
      if (TrainOnSingleVertex(current) < 0)
        continue;
      
      // Propagate
      PropagateOnSingleVertex(current, current, &album_copy);
      need_decision_set_.insert(current);
      Album& album = album_copy[(*graph_)[current].person_id];
      bool changed = MakeDecisionOnSingleVertex(&album);
      AdjacencyIterator adj, adj_end;
      for (tie(adj, adj_end) = adjacent_vertices(current, *graph_); 
          adj != adj_end;
          ++adj) {
        PropagateOnSingleVertex(current, *adj, &album_copy);
        need_decision_set_.insert(*adj);
      Album& album = album_copy[(*graph_)[*adj].person_id];
      bool changed = MakeDecisionOnSingleVertex(&album);
      }

      ++subcount;

      delete classifier_;
      classifier_ = NULL;
    }  // while (!visit_queue_1_.empty())

    // Make decision
    for (std::set<Vertex>::iterator it = need_decision_set_.begin();
        it != need_decision_set_.end();
        ++it) {
      Album& album = album_copy[(*graph_)[*it].person_id];
      bool changed = MakeDecisionOnSingleVertex(&album);
      if (changed)
        visit_queue_2_.push_back(*it);
    }
    need_decision_set_.clear();

    album_map_->swap(album_copy);


    char output_album_file[80];
    sprintf(output_album_file, "/tmp/sn_%5.5d_stage.alb", iter);
    WriteAlbumMapToFile(*album_map_, output_album_file);

    visit_queue_1_.swap(visit_queue_2_);
    ++iter;

    std::cout << "Number of points need to be visited in new iteration: " << visit_queue_1_.size() << std::endl;
  }

  return 0;
}

template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::AppendTrainingImageList
  (Vertex current, 
   FaceRecognition::AmbiguousImageList * image_list,
   bool load_image) {
    Album& album = (*album_map_)[(*graph_)[current].person_id];
    for (Album::iterator it = album.begin(); it != album.end(); ++it) {
      cv::Mat * image = new cv::Mat;
      if (load_image) {
        *image = (*it).GetImage(image_prefix_);
      }
      image_list->push_back( std::make_pair( image, &(it->GetPhotoRes()) ) );
    }
    return 0;
}

template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::TrainOnSingleVertex(Vertex current) {
  std::cout << "Training on " << (*graph_)[current].person_id << std::endl;

  // Prepare the image list
  FaceRecognition::AmbiguousImageList image_list;
  AppendTrainingImageList(current, &image_list);

  // Train
  int res = classifier_->Train(image_list);

  // Release resources
  for (FaceRecognition::AmbiguousImageList::iterator it = image_list.begin();
       it!=image_list.end();
       ++it) {
    delete it->first;
  }

  return res;
}

template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::PropagateOnSingleVertex
  (const Vertex base, 
   const Vertex current, 
   AlbumMap* album_map) {
  Album &album = (*album_map)[(*graph_)[current].person_id]; 
  std::cout << "Propagate to " << (*graph_)[current].person_id
  << " Album size: " << album.size() << std::endl;

  std::string id;
  std::string assigned_by;

  // float count = 0;
  // float correct_count = 0;
  // float decision_count = 0;
  // int num_label_changed = 0;
  // int it_count = 0;
  // for (Album::iterator it = album.begin(); it != album.end(); ++it) {
  int it;
  cv::Mat image;
#pragma omp parallel for private(it, id, assigned_by, image)
  for (it = 0; it < album.size(); ++it) {
    // id = it->GetAssignedId();
    // id = album[it].GetAssignedId();

    // Do not change those initial labels
    // assigned_by = it->GetAssignedBy();
    // assigned_by = album[it].GetAssignedBy();
    // if (assigned_by == "God") continue;
    // if (assigned_by != "-") continue;

    // Load the image
    image = album[it].GetImage(image_prefix_);

    // Identify
    FaceRecognition::PhotoResult& res = album[it].GetPhotoRes();
    classifier_->Identify(image, &res); 
  }
  return 0;
}

template <class AmbiguousClassifier>
bool BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::MakeDecisionOnSingleVertex (Album* album) {
  bool changed = false;
  float count = 0;
  float correct_count = 0;
  float decision_count = 0;
  for (Album::iterator it = album->begin();
      it != album->end();
      ++it) {
    // if (it->GetAssignedBy() != "-")
    //   continue;
    FaceRecognition::PhotoResult& res = it->GetPhotoRes();
    std::string res_id = "-";
    const double threshold = 1.5f;
    if (res.GetNumRecord() > 1) {
      std::string id_0, id_1;
      double score_0, score_1;
      res.GetSortedDecision(0, &score_0, &id_0);
      res.GetSortedDecision(1, &score_1, &id_1);
      if (fabs(score_0 / score_1) > threshold) {
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

template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::
PrepareRansacTrainingImageList(Vertex current,
                               FaceRecognition::AmbiguousImageList * image_list,
                               double select_ratio) {
  Album& album = (*album_map_)[(*graph_)[current].person_id];
  const int mod = 1000;
  std::string id;
  for (Album::iterator it = album.begin(); it != album.end(); ++it) {
    double r = rand() % mod;
    r = r / mod;
    id = it->GetAssignedId();

    if (id != "-" && r < select_ratio) {
      cv::Mat * image = new cv::Mat;
      *image = it->GetImage(image_prefix_);

      FaceRecognition::PhotoResult * res = new FaceRecognition::PhotoResult;
      res->AddRecord(id, 1.0);
      image_list->push_back( std::make_pair(image, res) );
    }
  }
  return 0;
}

template <class AmbiguousClassifier>
int BeliefPropagationAmbiguousContagionEngine<AmbiguousClassifier>::RansacOnSingleVertex (Vertex current) {
  AlbumMap album_copy = *album_map_;

  const int max_fold = 2;
  // double select_ratio = 1 - 1. / max_fold;
  double select_ratio = 0.8;
  for (int i=0; i<max_fold; ++i) {
    std::cout << "Ransac training on " << (*graph_)[current].person_id << 
      " fold " << i << " of " << max_fold << std::endl;
    if (classifier_)
      delete classifier_;
    classifier_ = new AmbiguousClassifier;

    FaceRecognition::AmbiguousImageList image_list;
    // Prepare the training samples.
    PrepareRansacTrainingImageList(current, &image_list, select_ratio);

    // Can do nothing if no training samples.
    if (image_list.empty())
      continue;

    // Train
    int res = classifier_->Train(image_list);

    for (FaceRecognition::AmbiguousImageList::iterator it = image_list.begin();
        it!=image_list.end();
        ++it) {
      delete it->first;
      delete it->second;
    }

    if (res < 0)
      continue;

    // Test
    PropagateOnSingleVertex(current, current, &album_copy);
    
    // Release
    delete classifier_;
    classifier_ = NULL;
  }

  album_map_->swap(album_copy);

  // // Test the accuracy of ransac result
  // Album album_copy_2 = (*album_map_)[(*graph_)[current].person_id];
  // MakeDecisionOnSingleVertex(&album_copy_2);

  return 0;
}

}  // namespace SocialNetwork

#endif  // __DATASET_BP_AMBIGUOUS_CONTAGION_ENGINE_H__

