/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: base_ambiguous_classifier.h
#
=========================================*/
#ifndef CLASSIFIER_BASE_AMBIGUOUS_CLASSIFIER_H_
#define CLASSIFIER_BASE_AMBIGUOUS_CLASSIFIER_H_

#include <iostream>
#include <cv.h>

#include "dataset/album.h"
#include "classifier/photo_result.h"

namespace FaceRecognition {

typedef std::vector< std::pair<cv::Mat*, PhotoResult*> > AmbiguousImageList;

class BaseAmbiguousClassifier {
  public:
    BaseAmbiguousClassifier() {};
    virtual ~BaseAmbiguousClassifier() {};

    virtual int Reset() {return 0;};

    virtual int Train(const AmbiguousImageList& image_list)
    {std::cout << "Dummy Training." << std::endl; return 0;};
    virtual int Train(SocialNetwork::Album& training_album) {};

    virtual bool IsTrained() {return false;};

    virtual bool Identify(const cv::Mat& image, PhotoResult* res)
    {std::cout << "Dummy Identify." << std::endl; return false;};
    virtual bool Identify(const SocialNetwork::Album& testing_album, SocialNetwork::Album& result_album) {};
};

}

#endif  // CLASSIFIER_BASE_AMBIGUOUS_CLASSIFIER_H_
