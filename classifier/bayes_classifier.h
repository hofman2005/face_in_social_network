/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: bayes_classifier.h
#
=========================================*/
#ifndef CLASSIFIER_BAYES_CLASSIFIER_H_
#define CLASSIFIER_BAYES_CLASSIFIER_H_

#include "classifier/inner_bayes_classifier.h"
#include "classifier/base_classifier.h"

namespace FaceRecognition {
class BayesClassifier : public BaseClassifier {
  public:
    BayesClassifier()
      : kernel_(new InnerBayesClassifier) {};
    virtual ~BayesClassifier();
    virtual int Reset();
    virtual int Train(SocialNetwork::Album& training_album);
    virtual bool Identify(const SocialNetwork::Album& testing_album,
                         SocialNetwork::Album& result_album);
    // int Train(const ImageList& image_list);
    // int TrainWithUpdatedLabels(const ImageList& image_list);
    // bool IsTrained();

    // bool Identify(cv::Mat& image, std::string* id);
    // bool Identify(cv::Mat& image, std::map<std::string, double>* pres);
    bool Identify(const cv::Mat& image, PhotoResult* pres);

  private:
    // int PrepareTrainingData(SocialNetwork::Album& training_album);
    InnerBayesClassifier * kernel_;
    std::map<std::string, int> id_table_;
    std::map<int, std::string> id_table_reverse_;
    // cv::Mat feature_, labels_;
};
}

#endif  // CLASSIFIER_BAYES_CLASSIFIER_H_
