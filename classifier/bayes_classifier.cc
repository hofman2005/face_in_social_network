/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: bayes_classifier.cc
#
=========================================*/
#include "classifier/bayes_classifier.h"

namespace FaceRecognition {

BayesClassifier::~BayesClassifier() {
  if (kernel_) delete kernel_;
}

int BayesClassifier::Reset() {
  id_table_.clear();
  id_table_reverse_.clear();

  if (kernel_) delete kernel_;
  kernel_ = new InnerBayesClassifier();

  return 0;
}

int BayesClassifier::Train(SocialNetwork::Album& training_album) {
  int num = training_album.size();
  
  // Load images
  cv::Mat feature(0, 0, CV_32FC1), labels(0,1,CV_32SC1);
  int max_id = id_table_.size()+1;
  for (int i=0; i<num; ++i) {
    // Set ID.
    const std::string& ori_id = training_album[i].GetAssignedId();
    if ( ori_id == "-" ) continue;

    std::map<std::string, int>::iterator it = id_table_.find(ori_id);
    if (it == id_table_.end()) {
      id_table_[ori_id] = max_id;
      id_table_reverse_[max_id] = ori_id;
      ++max_id;
    }
    
    labels.push_back(id_table_[ori_id]);

    // Load the image.
    cv::Mat data = training_album[i].GetImage();
    if (i==0) {
      feature.create(0, data.rows*data.cols, CV_32FC1); 
    }
    cv::Mat temp = data.reshape(1,1);
    feature.push_back(cv::Mat(1, data.rows*data.cols, CV_32FC1));
    cv::Mat row = feature.row(feature.rows-1);
    temp.convertTo(row, CV_32FC1);
  }
  // PrepareTrainingData(training_album);

  if (id_table_.size() < 2) {
    std::cout << "Only one class is labeled." << std::endl;
    return 0;
  }

  kernel_->train(feature, labels);

  return 0;
}

bool BayesClassifier::Identify(const cv::Mat& image, PhotoResult* res) {
  if (id_table_.size() < 2) {
    return false;
  }

  cv::Size size = image.size();
  cv::Mat temp = image.reshape(1,1);
  cv::Mat test_data(1, size.width*size.height, CV_32FC1);
  temp.convertTo(test_data, CV_32FC1);

  cv::Mat feature = test_data;
  std::map<int, double> raw_res;
  kernel_->predict(feature, &raw_res);
  
  for (std::map<int, double>::const_iterator it = raw_res.begin();
      it != raw_res.end();
      ++it) {
    const std::string& id = id_table_reverse_.find(it->first)->second;
    double score = it->second;
    res->AddRecord(id, score);
  }

  return true;
}


bool BayesClassifier::Identify(const SocialNetwork::Album& testing_album,
                      SocialNetwork::Album& result_album) {
  return false;
};
}
