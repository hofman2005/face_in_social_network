/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 10 Dec 2012 05:13:34 PM
#
# Filename: pca_ambiguous_classifier.h
#
=========================================*/
#ifndef CLASSIFIER_PCA_AMBIGUOUS_CLASSIFEIR_H_
#define CLASSIFIER_PCA_AMBIGUOUS_CLASSIFEIR_H_

#include "classifier/base_ambiguous_classifier.h"
#include "classifier/inner_bayes_ambiguous_classifier.h"

namespace FaceRecognition{
  class PCAAmbiguousClassifier : public BaseAmbiguousClassifier {
    public:
      PCAAmbiguousClassifier() : feature_(NULL) {};
      ~PCAAmbiguousClassifier();

      int Train(const AmbiguousImageList& image_list);
      bool Identify(const cv::Mat& image,
                    PhotoResult* res);
    private:
      InnerBayesAmbiguousClassifier kernel_;
      cv::Mat* feature_;
      cv::PCA pca_;
      std::map<std::string, int> id_table_;
      std::map<int, std::string> id_table_reverse_;
  };
}


#endif  // CLASSIFIER_PCA_AMBIGUOUS_CLASSIFEIR_H_
