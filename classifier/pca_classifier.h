/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Mar 2012 12:03:25 AM
#
# Filename: pca_classifier.h
#
=========================================*/

#ifndef CLASSIFIER_PCA_CLASSIFIER_H_
#define CLASSIFIER_PCA_CLASSIFIER_H_

#include "classifier/base_classifier.h"
#include <map>
#include <ml.h>

namespace FaceRecognition {
class PCAClassifier : public BaseClassifier {
 public:
  PCAClassifier() {};
  ~PCAClassifier() {};
 
//   int SetParameters() {};
   int Train(const ImageList &image_list);
   bool Identify(cv::Mat &image, std::string *id);
//   bool Verify() {};
// 
//   int Load() {};
//   int Save() {};

 private:
   CvNormalBayesClassifier kernel_;
   //CvKNearest kernel_;
   cv::PCA pca_;
   std::map<std::string, int> id_table_;
   std::map<int, std::string> id_table_reverse_;
};
}

#endif  // CLASSIFIER_PCA_CLASSIFIER_H_
