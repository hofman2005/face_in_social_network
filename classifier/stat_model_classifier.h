/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: knn_classifier.h
#
=========================================*/
#ifndef __CLASSIFIER_StatModel_CLASSIFIER_H__
#define __CLASSIFIER_StatModel_CLASSIFIER_H__

#include "classifier/base_classifier.h"
#include <opencv/cv.h>
#include <opencv/ml.h>
#include "classifier/inner_bayes_classifier.h"

namespace FaceRecognition {
class StatModelClassifier : public BaseClassifier {
  public:
    enum Type {ANN_MLP, Boost, DTree, EM, GBTrees, KNearest, NormalBayes, RTrees, SVM, InnerBayes};
    StatModelClassifier(Type type);
    virtual ~StatModelClassifier();

    virtual int Reset();
    virtual int Train(SocialNetwork::Album& training_album);
    virtual bool Identify(const cv::Mat& image, PhotoResult* res);

    class BadClassifierType {};

  private:
    int PrepareTrainingData(SocialNetwork::Album& training_album);
    int Train();
    bool Identify();

    CvStatModel * kernel_;
    std::map<std::string, int> id_table_;
    std::map<int, std::string> id_table_reverse_;
    cv::Mat feature_, labels_;
};
}

#endif  // __CLASSIFIER_StatModel_CLASSIFIER_H__
