/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 30 Apr 2012 01:44:19 PM
#
# Filename: pca_classifier.cc
#
=========================================*/

#include "classifier/pca_classifier.h"
#include "classifier/bayes_classifier.h"
#include <iostream>
#include <highgui.h>

namespace FaceRecognition {
PCAClassifier::~PCAClassifier() {
  if (feature_) delete feature_;
}

int PCAClassifier::Train(const ImageList &image_list) {
  std::cout << "Train PCA, # of training samples " << image_list.size() << std::endl;

  // Pack feature
  // Assuming every image has the same size
  int num = image_list.size();
  cv::Size size = image_list[0].first->size();
  cv::Mat* train_data = new cv::Mat(num, size.width * size.height, CV_32FC1);

  for (int i=0; i<num; i++) {
    cv::Mat row = train_data->row(i);
    cv::Mat &image = *(image_list[i].first);
    cv::Mat temp = image.reshape(1, 1);
    temp.convertTo(row, CV_32FC1);
  }
//   pca_(train_data, cv::Mat(), CV_PCA_DATA_AS_ROW, num/2);
// //  cv::Mat feature(num, num/2, CV_32FC1);
// //  pca_.project(train_data, feature);
//   feature_.create(num, num/2, CV_32FC1);
//   pca_.project(train_data, feature_);

  feature_ = train_data;
  return TrainWithUpdatedLabels(image_list);
}

int PCAClassifier::TrainWithUpdatedLabels(const ImageList &image_list) {
  std::vector<int> useful_index;
  int num = image_list.size();
  cv::Mat train_label(num, 1, CV_32SC1); 
  int max = id_table_.size()+1;
  for (int i=0; i<num; ++i) {
    const std::string &ori_id = image_list[i].second;
    if ( ori_id == "-" ) continue;
    //std::cout << "Training data id " << ori_id << std::endl;
    std::map<std::string, int>::iterator it = id_table_.find(ori_id);
    if (it == id_table_.end()) {
      id_table_[ori_id] = max;
      id_table_reverse_[max] = ori_id;
      ++max;
    }
    train_label.at<int>(i) = id_table_[ori_id];
    useful_index.push_back(i);
  }

  cv::Mat feature(0,0,CV_32FC1), labels(0,0,CV_32FC1);
  for (int i=0; i<useful_index.size(); ++i) {
    feature.push_back(feature_->row(useful_index[i]));
    labels.push_back(train_label.at<int>(useful_index[i]));
  }
  
  kernel2_.train(feature, labels);
  //kernel_.Train(feature, labels);

  return 0;
}

bool PCAClassifier::Identify(cv::Mat &image, std::string *id) {
  // FOR TEST ONLY
  // *id = "+";
  // return true;
  cv::Size size = image.size();
  cv::Mat temp = image.reshape(1, 1);
  cv::Mat test_data(1, size.width * size.height, CV_32FC1);
  //cv::Mat row = test_data.row(0);
  temp.convertTo(test_data, CV_32FC1);
  //test_data.row(0) = image.reshape(size.width*size.height);
  //image.convertTo(test_data.row(0), CV_64FC1, 1, 0);
  //image.convertTo(temp, CV_64FC1, 1, 0);

  //cv::Mat feature = pca_.project(test_data);
  cv::Mat feature = test_data;
  int res = kernel2_.predict(feature);
  //double res = kernel_.find_nearest(feature, 1);
  //std::cout << "Res " << res << std::endl;
  *id = id_table_reverse_[static_cast<int>(res)]; 
  //std::cout << "Identified as " << *id << std::endl;

  //return false;
  return true;
}

bool PCAClassifier::Identify(cv::Mat& image, std::map<std::string, double>* res) {
  cv::Size size = image.size();
  cv::Mat temp = image.reshape(1, 1);
  cv::Mat test_data(1, size.width * size.height, CV_32FC1);
  //cv::Mat row = test_data.row(0);
  temp.convertTo(test_data, CV_32FC1);
  //test_data.row(0) = image.reshape(size.width*size.height);
  //image.convertTo(test_data.row(0), CV_64FC1, 1, 0);
  //image.convertTo(temp, CV_64FC1, 1, 0);

  //cv::Mat feature = pca_.project(test_data);
  cv::Mat feature = test_data;

  std::map<int, double> raw_res; 
  //kernel_.Identify(feature, &raw_res);
  kernel2_.predict(feature, &raw_res);

  std::map<int, double>::iterator it;
  for (it=raw_res.begin(); it!=raw_res.end(); ++it) {
    (*res)[id_table_reverse_[static_cast<int>(it->first)]] = it->second; 
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
