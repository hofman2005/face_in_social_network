/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 08 Mar 2012 05:00:15 PM
#
# Filename: classifier.h
#
=========================================*/

#ifndef CLASSIFIER_BASE_CLASSIFIER_H_
#define CLASSIFIER_BASE_CLASSIFIER_H_

#include <string>
#include <utility>
#include <vector>
#include <cv.h>

namespace FaceRecognition {
typedef std::vector< std::pair<cv::Mat*, // Images
                               std::string> // Image ID
                   > ImageList;

class BaseClassifier {
 public:
  BaseClassifier() {};
  virtual ~BaseClassifier() {};

  virtual int SetParameters() {return 0;};
  virtual int Train(const ImageList &image_list) {return 0;};
  virtual bool Identify(cv::Mat &image, std::string *id) {return false;};
  virtual bool Verify() {return false;};

  virtual int Load(std::string &file_name) {return 0;};
  virtual int Save(std::string &file_name) {return 0;};
};
}

#endif  // CLASSIFIER_BASE_CLASSIFIER_H_ 
