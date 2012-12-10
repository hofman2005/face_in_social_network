/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Dec 2012 05:37:51 PM
#
# Filename: pca_ambiguous_classifier.cc
#
=========================================*/
#include "classifier/pca_ambiguous_classifier.h"
#include <cv.h>

namespace FaceRecognition {
PCAAmbiguousClassifier::~PCAAmbiguousClassifier() {
  if (feature_) delete feature_;
}

int PCAAmbiguousClassifier::Train(const AmbiguousImageList& image_list) {
  std::cout << "Train ambiguous PCA, # of training samples " << image_list.size() << std::endl;

  // Pack feature
  // Assuming every image has the same size
  int num = image_list.size();
  cv::Size size = image_list[0].first->size();
  cv::Mat* train_data = new cv::Mat(num, size.width * size.height, CV_64FC1);

  for (int i=0; i<num; i++) {
    cv::Mat row = train_data->row(i);
    cv::Mat &image = *(image_list[i].first);
    cv::Mat temp = image.reshape(1, 1);
    temp.convertTo(row, CV_64FC1);
  }

  // Build label look up table
  // Inner label starts at 0
  std::string ori_id;
  double score;
  int max_id = id_table_.size();
  for (int i=0; i<num; ++i) {
    for (int j=0; j<image_list[i].second.GetNumRecord(); ++j) {
      image_list[i].second.GetSortedDecision(j, 
          &score, &ori_id);
      if (id_table_.count(ori_id)==0) {
        id_table_[ori_id] = max_id;
        id_table_reverse_[max_id] = ori_id;
        ++max_id;
      }
    }
  }

  // Convert labels
  cv::Mat score_table(num, max_id, CV_64FC1);
  for (int i=0; i<num; ++i) {
    for (int j=0; j<image_list[i].second.GetNumRecord(); ++j) {
      image_list[i].second.GetSortedDecision(j,
          &score, &ori_id);
      score_table.at<double>(i, id_table_[ori_id]) = score;
    }
  }

  kernel_.train(*train_data, score_table);

  delete train_data;

  return 0;
}

bool PCAAmbiguousClassifier::Identify(const cv::Mat& image,
    PhotoResult* res) {
  return false;
}

}
