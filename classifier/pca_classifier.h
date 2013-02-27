/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 27 Feb 2013 12:42:12 PM
#
# Filename: pca_classifier.h
#
=========================================*/

#ifndef CLASSIFIER_PCA_CLASSIFIER_H_
#define CLASSIFIER_PCA_CLASSIFIER_H_

#include "classifier/base_classifier.h"
#include "classifier/bayes_classifier.h"
#include <map>
#include <ml.h>

#include <iostream>

namespace FaceRecognition {
class PCAClassifier : public BaseClassifier {
 public:
  PCAClassifier() 
    : feature_(NULL),
      kernel2_(new InnerBayesClassifier) {};
  virtual ~PCAClassifier();
 
//   int SetParameters() {};
  virtual int Reset();
  virtual int Train(const ImageList& image_list);
  virtual int TrainWithUpdatedLabels(const ImageList& image_list);
  virtual bool Identify(cv::Mat& image, std::string *id);
  virtual bool Identify(cv::Mat& image, std::map<std::string, double>* res);
  virtual bool Identify(const cv::Mat& image, PhotoResult* res);
  //   bool Verify() {};
// 
//   int Load() {};
//   int Save() {};

 private:
   //CvNormalBayesClassifier kernel2_;
   //CvKNearest kernel_;
   InnerBayesClassifier * kernel2_;
   cv::Mat* feature_;
   // cv::PCA pca_;
   std::map<std::string, int> id_table_;
   std::map<int, std::string> id_table_reverse_;
};
}

#endif  // CLASSIFIER_PCA_CLASSIFIER_H_
