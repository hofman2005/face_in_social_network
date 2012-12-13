/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Dec 2012 10:31:17 PM
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
    for (int j=0; j<image_list[i].second->GetNumRecord(); ++j) {
      image_list[i].second->GetSortedDecision(j, 
          &score, &ori_id);
      if (id_table_.count(ori_id)==0) {
        id_table_[ori_id] = max_id;
        id_table_reverse_[max_id] = ori_id;
        ++max_id;
      }
    }
  }

  // Convert labels
  // cv::Mat score_table(num, max_id, CV_64FC1);
  cv::Mat score_table = cv::Mat::zeros(num, max_id, CV_64FC1);
  for (int i=0; i<num; ++i) {
    for (int j=0; j<image_list[i].second->GetNumRecord(); ++j) {
      image_list[i].second->GetSortedDecision(j,
          &score, &ori_id);
      score_table.at<double>(i, id_table_[ori_id]) = score;
    }
  }

  if (score_table.cols < 2) {
    delete train_data;
    return -1;
  } else {
    kernel_.train(*train_data, score_table);
    delete train_data;
    return 0;
  }
}

bool PCAAmbiguousClassifier::Identify(const cv::Mat& image,
    PhotoResult* res) {
  if (id_table_.size() < 2) {
    return false;
  }
  cv::Size size = image.size();
  cv::Mat temp = image.reshape(1, 1);
  cv::Mat test_data(1, size.width * size.height, CV_64FC1);
  temp.convertTo(test_data, CV_64FC1);
  //test_data.row(0) = image.reshape(size.width*size.height);
  //image.convertTo(test_data.row(0), CV_64FC1, 1, 0);
  //image.convertTo(temp, CV_64FC1, 1, 0);

  //cv::Mat feature = pca_.project(test_data);
  cv::Mat feature = test_data;

  std::map<int, double> raw_res; 
  //kernel_.Identify(feature, &raw_res);
  kernel_.predict(feature, &raw_res);

  // std::map<int, double>::const_iterator it;
  for (std::map<int, double>::const_iterator it=raw_res.begin();
       it!=raw_res.end(); 
       ++it) {
    // std::cout << it->first << " " << it->second << std::endl;
    //(*res)[id_table_reverse_[static_cast<int>(it->first)]] = it->second;
    // const std::string& id = id_table_reverse_[static_cast<int>(it->first)];
    // const std::string& id = id_table_reverse_[it->first];
    const std::string& id = id_table_reverse_.find(it->first)->second;
    double score = it->second;
    res->AddRecord(id, score);
  }

  // FOR DEBUG
  // std::map<std::string, double>::iterator it2;
  // std::cout << "Result:" << std::endl;
  // for (it2=res->begin(); it2!=res->end(); ++it2) {
  //   std::cout << it2->first << " " << it2->second << std::endl;
  // }
  return true;
}

}
