#ifndef CLASSIFIER_INNER_BAYES_AMBIGUOUS_CLASSIFIER_H_ 
#define CLASSIFIER_INNER_BAYES_AMBIGUOUS_CLASSIFIER_H_

#include <cv.h>

namespace FaceRecognition {
  class InnerBayesAmbiguousClassifier {
    public:
      bool train(cv::Mat& train_data,
                 cv::Mat& score_table);
      bool predict(cv::Mat& test_data);

    private:
      int     var_count, var_all;
      CvMat*  var_idx;
      CvMat*  cls_labels;
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
