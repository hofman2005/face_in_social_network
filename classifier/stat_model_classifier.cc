/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: stat_model_classifier.cc
#
=========================================*/
#include "classifier/stat_model_classifier.h"

namespace FaceRecognition {
StatModelClassifier::StatModelClassifier(Type type) {
  switch (type) {
    case ANN_MLP: kernel_ = new CvANN_MLP; break;
    case Boost:   kernel_ = new CvBoost; break;
    case DTree:   kernel_ = new CvDTree; break;
    case EM:      kernel_ = new CvEM; break;
    case GBTrees: kernel_ = new CvGBTrees; break;
    case KNearest: kernel_ = new CvKNearest; break;
    case NormalBayes: kernel_ = new CvNormalBayesClassifier; break;
    case RTrees:  kernel_ = new CvRTrees; break;
    case SVM:     kernel_ = new CvSVM; break;
    case InnerBayes: kernel_ = new InnerBayesClassifier; break;
    default: throw BadClassifierType(); break;
  }
}

StatModelClassifier::~StatModelClassifier() {
  if (kernel_) delete kernel_;
}

int StatModelClassifier::Reset() {
  kernel_->clear();
  id_table_.clear();
  id_table_reverse_.clear();
  feature_.create(0,0,CV_32FC1);
  labels_.create(0,0,CV_32FC1);
  return 0;
}

int StatModelClassifier::Train(SocialNetwork::Album& training_album) {
  PrepareTrainingData(training_album);

  if (id_table_.size() < 2) {
    std::cout << "Only one class is labeled." << std::endl;
    return 0;
  }

  Train();

  return 0;
}

int StatModelClassifier::Train() {
  if (CvNormalBayesClassifier *p = dynamic_cast<CvNormalBayesClassifier*>(kernel_)) {
    p->train(feature_, labels_);
  }
  else if (InnerBayesClassifier *p = dynamic_cast<InnerBayesClassifier*>(kernel_)) {
    p->train(feature_, labels_);
  }
  return 0;
}

int StatModelClassifier::PrepareTrainingData(SocialNetwork::Album& training_album) {
  int num = training_album.size();
  
  feature_.create(0, 0, CV_32FC1), labels_.create(0,1,CV_32SC1);
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
    
    labels_.push_back(id_table_[ori_id]);

    // Load the image.
    cv::Mat data = training_album[i].GetImage();
    if (i==0) {
      feature_.create(0, data.rows*data.cols, CV_32FC1); 
    }
    cv::Mat temp = data.reshape(1,1);
    feature_.push_back(cv::Mat(1, data.rows*data.cols, CV_32FC1));
    cv::Mat row = feature_.row(feature_.rows-1);
    temp.convertTo(row, CV_32FC1);
  }

  return 0;
}

bool StatModelClassifier::Identify(const cv::Mat& image, PhotoResult* res) {
  if (id_table_.size() < 2) {
    return false;
  }

  // Prepare the feature
  cv::Size size = image.size();
  cv::Mat temp = image.reshape(1,1);
  cv::Mat test_data(1, size.width*size.height, CV_32FC1);
  temp.convertTo(test_data, CV_32FC1);

  std::map<int, double> raw_res;
  // kernel_->predict(test_data, &raw_res);
  if (InnerBayesClassifier *p = dynamic_cast<InnerBayesClassifier*>(kernel_)) {
    p->predict(test_data, &raw_res);
  }

  // Save result
  for (std::map<int, double>::const_iterator it = raw_res.begin();
      it != raw_res.end();
      ++it) {
    const std::string& id = id_table_reverse_.find(it->first)->second;
    double score = it->second;
    res->AddRecord(id, score);
  }

  return true;
}
}
