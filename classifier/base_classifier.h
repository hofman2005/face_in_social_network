/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 25 Feb 2013 04:18:42 PM
#
# Filename: classifier.h
#
=========================================*/

#ifndef CLASSIFIER_BASE_CLASSIFIER_H_
#define CLASSIFIER_BASE_CLASSIFIER_H_

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cv.h>

#include <iostream>

#include "classifier/photo_result.h"

namespace FaceRecognition {
typedef std::vector< std::pair<cv::Mat*,    // Images
                               std::string> // Image ID
                   > ImageList;

class BaseClassifier {
 public:
  BaseClassifier() {};
  virtual ~BaseClassifier() {};

  virtual int Reset() {return 0;};

  virtual int SetParameters() {return 0;};

  virtual int Train(const ImageList& image_list) 
    {std::cout << "Dummy Training." << std::endl;return 0;};

  virtual int TrainWithUpdatedLabels(const ImageList& image_list) 
    {std::cout << "Dummy Training with updated labels." << std::endl; return 0;};

  virtual bool IsTrained() {return false;};

  virtual bool Identify(cv::Mat& image, std::string *id) 
    {std::cout << "Dummy Identifying. " << std::endl; return false;};

  virtual bool Identify(cv::Mat& image, std::map<std::string, double>* res)
    {std::cout << "Dummy Identify ver2." << std::endl; return false;};

  virtual bool Identify(const cv::Mat& image, PhotoResult* res)
    {std::cout << "Dummy Identify ver3." << std::endl; return false;};

  virtual bool Verify() {return false;};

  virtual int Load(std::string &file_name) {return 0;};
  virtual int Save(std::string &file_name) {return 0;};
};
}

#endif  // CLASSIFIER_BASE_CLASSIFIER_H_ 
