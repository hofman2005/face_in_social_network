/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Mar 2012 09:18:04 AM
#
# Filename: pca_classifier.cc
#
=========================================*/

#include "classifier/pca_classifier.h"
#include <iostream>
#include <highgui.h>

namespace FaceRecognition {
int PCAClassifier::Train(const ImageList &image_list) {
  std::cout << "Train PCA, # of training samples " << image_list.size() << std::endl;

  // Pack feature
  // Assuming every image has the same size
  int num = image_list.size();
  cv::Size size = image_list[0].first->size();
  cv::Mat train_data(num, size.width * size.height, CV_32FC1);

  for (int i=0; i<num; i++) {
    cv::Mat row = train_data.row(i);
    cv::Mat &image = *(image_list[i].first);
    cv::Mat temp = image.reshape(1, 1);
    temp.convertTo(row, CV_32FC1);
  }
  // TEST ONLY
  // cv::Mat o = train_data.row(0).reshape(1, size.height); 
  // cv::imwrite("/tmp/dump.png", o);

  cv::Mat train_label(num, 1, CV_32FC1); 
  int max = 1;
  for (int i=0; i<num; i++) {
    const std::string &ori_id = image_list[i].second;
    //std::cout << "Training data id " << ori_id << std::endl;
    std::map<std::string, int>::iterator it = id_table_.find(ori_id);
    if (it == id_table_.end()) {
      id_table_[ori_id] = max;
      id_table_reverse_[max] = ori_id;
      ++max;
    }
    train_label.at<float>(i) = id_table_[ori_id];
  }
  
  // PCA
  //cv::Mat average_face;
  //cv::PCA pca(train_data, average_face, CV_PCA_DATA_AS_ROW, num/2);
  //cv::PCA pca(train_data, cv::Mat(), CV_PCA_DATA_AS_ROW, num/2);
  pca_(train_data, cv::Mat(), CV_PCA_DATA_AS_ROW, num/2);
  cv::Mat feature(num, num/2, CV_32FC1);
  pca_.project(train_data, feature);

  kernel_.train(feature, train_label);

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
  //image.convertTo(test_data.row(0), CV_32FC1, 1, 0);
  //image.convertTo(temp, CV_32FC1, 1, 0);

  cv::Mat feature = pca_.project(test_data);
  float res = kernel_.predict(feature);
  //float res = kernel_.find_nearest(feature, 1);
  //std::cout << "Res " << res << std::endl;
  *id = id_table_reverse_[static_cast<int>(res)]; 
  //std::cout << "Identified as " << *id << std::endl;

  //return false;
  return true;
}
}
