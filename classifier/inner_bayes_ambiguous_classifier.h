#ifndef CLASSIFIER_INNER_BAYES_AMBIGUOUS_CLASSIFIER_H_ 
#define CLASSIFIER_INNER_BAYES_AMBIGUOUS_CLASSIFIER_H_

#include <map>
#include <cv.h>
#include "inner_bayes_classifier.h"

namespace FaceRecognition {
  class InnerBayesAmbiguousClassifier {
    public:
      virtual ~InnerBayesAmbiguousClassifier();

      virtual bool train(const cv::Mat& train_data,
                         const cv::Mat& score_table);
      virtual bool predict(const cv::Mat& test_data,
                           std::map<int, double>* res);
      virtual void clear();

    protected:
      int nclasses;
      int     var_count, var_all;
      CvMat*  var_idx;
      CvMat*  cls_labels;
      int max_labels;
      CvMat** count;
      CvMat** sum;
      CvMat** productsum;
      CvMat** avg;
      CvMat** inv_eigen_values;
      CvMat** cov_rotate_mats;
      CvMat*  c;
      const char* default_model_name;
  };
}

#endif  // CLASSIFIER_INNER_BAYES_AMBIGUOUS_CLASSIFIER_H_
