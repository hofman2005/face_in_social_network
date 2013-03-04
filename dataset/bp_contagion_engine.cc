/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: bp_contagion_engine.cc
#
=========================================*/
#include "dataset/bp_contagion_engine.h"

namespace SocialNetwork {
BeliefPropagationContagionEngine::~BeliefPropagationContagionEngine() {
}

int BeliefPropagationContagionEngine::ReleaseAllClassifiers() {
  return 0;
}

int BeliefPropagationContagionEngine::Init() {
  return 0;
}

int BeliefPropagationContagionEngine::Run() {
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
      if (it->GetAssignedId().compare("God")) {
        visit_queue_1_.push_back(*vi);
        break;
      }
    }
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

      // Train
      TrainOnSingleVertex(current);
      
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

      // Save the album
      //if ((subcount % 30)==0) {
      //  char output_album_file[80];
      //  sprintf(output_album_file, "/tmp/sn_%5.5d_%5.5d.alb", iter, subcount);
      //  WriteAlbumMapToFile(album_copy, output_album_file);
      //}

      // Release the classifier
      //classifiers_.clear();

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

bool BeliefPropagationContagionEngine::MakeDecisionOnSingleVertex (Album* album) {
  bool changed = false;
  float count = 0;
  float correct_count = 0;
  float decision_count = 0;
  for (Album::iterator it = album->begin();
      it != album->end();
      ++it) {
    if (! (it->GetAssignedBy() == "-" ||
           it->GetAssignedBy() == "Amb") )
      continue;
    FaceRecognition::PhotoResult& res = it->GetPhotoRes();
    std::string res_id = "-";
    const double threshold = 4.0f;
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
      else
        it->SetAssignedId("-", "-");
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

int BeliefPropagationContagionEngine::AppendTrainingImageList
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
        //*image = (*it).GetImage(image_prefix_);
        *image = (*it).GetImage();
      }
      image_list->push_back(std::make_pair(image, id));
    }

    return 0;
}

int BeliefPropagationContagionEngine::TrainOnSingleVertex(Vertex current) {
  // Do not process the dummy source
  // if ((*graph_)[current].person_id == "Infection_Source") return 0;
  
  std::cout << "Training on " << (*graph_)[current].person_id << std::endl;

  // Prepare the image list
  // bool load_image = !classifiers_[current]->IsTrained();
  // bool load_image = true;
  // FaceRecognition::ImageList image_list;
  // AppendTrainingImageList(current, &image_list);

  // Append the images in friends.
  // AdjacencyIterator adj, adj_end;
  // for (tie(adj, adj_end) = adjacent_vertices(current, *graph_); 
  //     adj != adj_end;
  //     ++adj) {
  //   AppendTrainingImageList(*adj, &image_list, load_image);
  // }

  // Train
  classifier_->Reset();
  // classifier_->Train(image_list);
  Album& training_album = (*album_map_)[(*graph_)[current].person_id];
  classifier_->Train(training_album);

  // Release resources
  // for (FaceRecognition::ImageList::iterator it = image_list.begin();
  //      it!=image_list.end();
  //      ++it) {
  //   delete it->first;
  // }

  return 0;
}

int BeliefPropagationContagionEngine::PropagateOnSingleVertex
  (const Vertex base, 
   const Vertex current, 
   AlbumMap* album_map) {
  // Do not process the dummy source
  // if ((*graph_)[base].person_id    == "Infection_Source") return 0;
  // if ((*graph_)[current].person_id == "Infection_Source") return 0;
  
  // FaceRecognition::BaseClassifier* pclassifier = classifiers_[base]; 

  Album &album = (*album_map)[(*graph_)[current].person_id]; 
  std::cout << "Propagate to " << (*graph_)[current].person_id
  << " Album size: " << album.size() << std::endl;

  std::string id;
  std::string assigned_by;

  float count = 0;
  float correct_count = 0;
  float decision_count = 0;
  int num_label_changed = 0;
  int it_count = 0;
  // for (Album::iterator it = album.begin(); it != album.end(); ++it) {
  int it;
  cv::Mat image;
#pragma omp parallel for private(it, id, assigned_by, image)
  for (it = 0; it < album.size(); ++it) {
    // ++it_count;
    // std::cout << it_count << " of " << album.size() << "\r";

    // id = it->GetAssignedId();
    id = album[it].GetAssignedId();

    // Do not change those initial labels
    // assigned_by = it->GetAssignedBy();
    assigned_by = album[it].GetAssignedBy();
    // if (assigned_by == "God") continue;
    // if (assigned_by != "-") continue;
    if ( ! (assigned_by == "Amb" || 
            assigned_by == "-") ) continue;

    // Load the image
    // cv::Mat image  = it->GetImage(image_prefix_);
    // image = album[it].GetImage(image_prefix_);
    image = album[it].GetImage();

    // Identify
    // FaceRecognition::PhotoResult& res = it->GetPhotoRes();
    FaceRecognition::PhotoResult& res = album[it].GetPhotoRes();
    classifier_->Identify(image, &res); 

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
  // std::cout << std::endl;
  // std::cout << " Accuracy: " << correct_count/decision_count*100 << "%" << std::endl;
  // std::cout << " Decision made on " << decision_count << " of " << count << std::endl;

  // Save the album
  // char output_album_file[80];
  // sprintf(output_album_file, "/tmp/sn_aa.alb");
  // WriteAlbumMapToFile(*album_map, output_album_file);

  // return num_label_changed;
  return 0;
}

int BeliefPropagationContagionEngine::
PrepareRansacTrainingImageList(Vertex current,
                               FaceRecognition::ImageList * image_list,
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
      //*image = it->GetImage(image_prefix_);
      *image = it->GetImage();

      image_list->push_back( std::make_pair(image, id) );
    }
  }
  return 0;
}

int BeliefPropagationContagionEngine::RansacOnSingleVertex (Vertex current) {
  AlbumMap album_copy = *album_map_;

  const int max_fold = 5;
  // double select_ratio = 1 - 1. / max_fold;
  double select_ratio = 0.4;
  for (int i=0; i<max_fold; ++i) {
    std::cout << "Ransac training on " << (*graph_)[current].person_id << 
      " fold " << i << " of " << max_fold << std::endl;
    // if (classifier_)
    //   delete classifier_;
    // classifier_ = new Classifier;
    classifier_->Reset();

    FaceRecognition::ImageList image_list;
    // Prepare the training samples.
    PrepareRansacTrainingImageList(current, &image_list, select_ratio);

    // Can do nothing if no training samples.
    if (image_list.empty())
      continue;

    // Train
    int res = classifier_->Train(image_list);

    for (FaceRecognition::ImageList::iterator it = image_list.begin();
        it!=image_list.end();
        ++it) {
      // delete it->first;
      // delete it->second;
    }

    if (res < 0)
      continue;

    // Test
    // FaceRecognition::BaseClassifier* old_classifier = classifiers_[current];
    // classifiers_[current] = classifier_;
    PropagateOnSingleVertex(current, current, &album_copy);
    // classifiers_[current] = old_classifier;
    
    // Release
    // delete classifier_;
    // classifier_ = NULL;
  }

  album_map_->swap(album_copy);

  // // Test the accuracy of ransac result
  // Album album_copy_2 = (*album_map_)[(*graph_)[current].person_id];
  // MakeDecisionOnSingleVertex(&album_copy_2);
  Album& album = (*album_map_)[(*graph_)[current].person_id];
  MakeDecisionOnSingleVertex(&album);

  return 0;
}

// Generate the initial label list and weights.
int BeliefPropagationContagionEngine::FirstRun() {
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


}
