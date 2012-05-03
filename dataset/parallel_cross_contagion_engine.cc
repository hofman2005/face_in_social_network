/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 30 Apr 2012 01:07:50 PM
#
# Filename: parallel_cross_contagion_engine.cc
#
=========================================*/

#ifndef DATASET_PARALLEL_CROSS_CONTAGION_ENGINE_CC_
#define DATASET_PARALLEL_CROSS_CONTAGION_ENGINE_CC_

#include "dataset/parallel_cross_contagion_engine.h"

namespace SocialNetwork {
template <class Classifier>
ParallelCrossContagionEngine<Classifier>::~ParallelCrossContagionEngine() {
  ReleaseAllClassifiers();
}

template <class Classifier>
int ParallelCrossContagionEngine<Classifier>::ReleaseAllClassifiers() {
  for (std::vector<FaceRecognition::BaseClassifier*>::iterator it = classifiers_.begin();
       it != classifiers_.end();
       ++it) {
    if (*it) delete *it;
  }
  return 0;
}

template <class Classifier>
int ParallelCrossContagionEngine<Classifier>::Init() {
  // Initialize the classifiers
  ReleaseAllClassifiers();
  classifiers_.clear();

  for (int i=0; i<num_vertices(*graph_); ++i) {
    classifiers_.push_back(new Classifier);
  }

  return 0;
}

template <class Classifier>
int ParallelCrossContagionEngine<Classifier>::Run() {

  for (int count=0; count<2; ++count) {
    VertexIterator vi, vi_end;
    AlbumMap album_copy = *album_map_;
    int subcount = 0;
    for (tie(vi,vi_end)=vertices(*graph_);
         vi != vi_end;
         ++vi, ++subcount) {
      // Update self
      TrainOnSingleVertex(*vi);
      // Propagate
      PropagateOnSingleVertex(*vi, *vi, &album_copy);
      AdjacencyIterator adj, adj_end;
      for (tie(adj, adj_end) = adjacent_vertices(*vi, *graph_); adj != adj_end; ++adj) {
        PropagateOnSingleVertex(*vi, *adj, &album_copy);
      // Save the album
      char output_album_file[80];
      sprintf(output_album_file, "/tmp/sn_%5.5d_%5.5d.alb", count, subcount);
      WriteAlbumMapToFile(album_copy, output_album_file);
      }
    }
    *(album_map_) = album_copy;

    // Save the album
    // char output_album_file[80];
    // sprintf(output_album_file, "/tmp/sn_%5.5d.alb", count);
    // WriteAlbumMapToFile(*album_map_, output_album_file);
  }

  return 0;
}

template <class Classifier>
int ParallelCrossContagionEngine<Classifier>::TrainOnSingleVertex(Vertex current) {
  // Do not process the dummy source
  if ((*graph_)[current].person_id == "Infection_Source") return 0;
  std::cout << "Training on " << (*graph_)[current].person_id << std::endl;

  Album &album = (*album_map_)[(*graph_)[current].person_id]; 
  FaceRecognition::ImageList image_list;
  std::string id;
  if (classifiers_[current]->IsTrained()) {
    for (Album::iterator it = album.begin(); it != album.end(); ++it) {
      id = (*it).GetAssignedId();
      // if ( id == "-") continue;
      cv::Mat image;
      image_list.push_back(std::make_pair(&image, id));
    }
    classifiers_[current]->TrainWithUpdatedLabels(image_list);
  } else {
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
int ParallelCrossContagionEngine<Classifier>::PropagateOnSingleVertex
  (const Vertex base, const Vertex current, AlbumMap* album_map) {
  // Do not process the dummy source
  if ((*graph_)[base].person_id    == "Infection_Source") return 0;
  if ((*graph_)[current].person_id == "Infection_Source") return 0;
  std::cout << "Propagate to " << (*graph_)[current].person_id << std::endl;

  FaceRecognition::BaseClassifier* classifier = classifiers_[base]; 

  // const std::string &image_prefix = BaseBehavior<Classifier>::image_prefix_;
  const std::string &image_prefix = image_prefix_;
  // Album &album = (*BaseBehavior<Classifier>::album_map_)[graph[current].person_id]; 
  //Album &album = (*album_map_)[(*graph_)[current].person_id]; 
  Album &album = (*album_map)[(*graph_)[current].person_id]; 
  std::string id;
  std::string assigned_by;

  float count = 0;
  float correct_count = 0;
  float decision_count = 0;
  int num_label_changed = 0;
  for (Album::iterator it = album.begin(); it != album.end(); ++it) {
    id = (*it).GetAssignedId();
    //if ( id != "-") continue;
    
    // // Do not change those initial labels
    assigned_by = (*it).GetAssignedBy();
    if (assigned_by == "God") continue;

    cv::Mat image  = (*it).GetImage(image_prefix);

    // // Identification
    // std::string res_id;
    // // if (BaseBehavior<Classifier>::classifier_.Identify(image, &res_id)) {
    // if (classifier->Identify(image, &res_id)) {
    //   // If identification is successful
    //   (*it).SetAssignedId(res_id, (*graph_)[current].person_id); 
    // };

    // Multi candidate identification
    std::map<std::string, double>& res = it->GetRes();
    std::map<std::string, double> new_res;
    classifier->Identify(image, &new_res);

    // Merge result
    for (std::map<std::string, double>::iterator res_it = new_res.begin();
         res_it != new_res.end();
         ++res_it) {
      if (res.find(res_it->first)!=res.end()) {
        if (res_it->second < res[res_it->first]) {
          res[res_it->first] = res_it->second;
        }
      } else {
        res[res_it->first] = res_it->second;
      }
    }

    std::vector< std::pair<std::string, double> > vec(res.begin(), res.end());
    std::sort(vec.begin(), vec.end(), IntCmp());
    std::string res_id;
    const double threshold = 3.0f;
    if (vec[1].second / vec[0].second > threshold) {
      res_id = vec[0].first;
      (*it).SetAssignedId(res_id, (*graph_)[current].person_id); 
    } else {
      res_id = "-";
    }

    // Simple analysis
    ++count;
    if (res_id == (*it).GetTrueId()) ++correct_count;
    if (res_id != "-") ++decision_count;

    if (res_id != id) ++num_label_changed;
  }
  std::cout << " Accuracy: " << correct_count/decision_count*100 << "%" << std::endl;
  std::cout << " Decision made on " << decision_count << " of " << count << std::endl;

  // Save the album
  // char output_album_file[80];
  // sprintf(output_album_file, "/tmp/sn_aa.alb");
  // WriteAlbumMapToFile(*album_map, output_album_file);

  return num_label_changed;
}

//template <class Classifier>
//bool ParallelCrossContagionEngine<Classifier>::MyCompare
// bool MyCompare
//   (std::pair<std::string, double>& lhs, 
//    std::pair<std::string, double>& rhs) {
//   return lhs.second < rhs.second;
// }

}

#endif  // DATASET_PARALLEL_CROSS_CONTAGION_ENGINE_CC_

