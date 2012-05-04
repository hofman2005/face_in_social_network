#ifndef CLASSIFIER_BAYES_CLASSIFIER_H_
#define CLASSIFIER_BAYES_CLASSIFIER_H_

#include "classifier/inner_bayes_classifier.h"
#include "classifier/base_classifier.h"

namespace FaceRecognition {
class BayesClassifier : public BaseClassifier {
 public:
  int Train(const ImageList& image_list);
  int TrainWithUpdatedLabels(const ImageList& image_list);
  bool IsTrained();

  bool Identify(cv::Mat& image, std::string* id);
  bool Identify(cv::Mat& image, std::map<std::string, double>* pres);
  bool Identify(cv::Mat& image, PhotoResult* pres);

 private:
  InnerBayesClassifier classifier_;
};
}

#endif  // CLASSIFIER_BAYES_CLASSIFIER_H_
