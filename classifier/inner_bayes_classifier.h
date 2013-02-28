#ifndef CLASSIFIER_INNER_BAYES_CLASSIFIER_H_
#define CLASSIFIER_INNER_BAYES_CLASSIFIER_H_

#include <opencv/cv.h>
#include <opencv/ml.h>

namespace FaceRecognition {
// using namespace cv;

#define CV_ROW_SAMPLE 1
#define CV_VAR_CATEGORICAL  1
#define CV_TYPE_NAME_ML_NBAYES      "opencv-ml-bayesian"


class InnerBayesClassifier : public CvStatModel
{
public:
    InnerBayesClassifier();
    virtual ~InnerBayesClassifier();

    InnerBayesClassifier( const CvMat* trainData, const CvMat* responses,
                          const CvMat* varIdx=0, const CvMat* sampleIdx=0 );
    
    virtual bool train( const CvMat* trainData, const CvMat* responses,
        const CvMat* varIdx = 0, const CvMat* sampleIdx=0, bool update=false );
   
    virtual float predict( const CvMat* samples, CV_OUT CvMat* results=0 ) const;
    virtual void clear();

    //////////////////////
    InnerBayesClassifier( const cv::Mat& trainData, const cv::Mat& responses,
                     const cv::Mat& varIdx=cv::Mat(), const cv::Mat& sampleIdx=cv::Mat() );
    virtual bool train( const cv::Mat& trainData, const cv::Mat& responses,
                       const cv::Mat& varIdx = cv::Mat(), const cv::Mat& sampleIdx=cv::Mat(),
                       bool update=false );
    virtual float predict( const cv::Mat& samples, CV_OUT cv::Mat* results=0 ) const;
    virtual int predict( const cv::Mat& samples, std::map<int, double>* res) const;
    virtual int predict( const CvMat* samples, std::map<int, double>* res) const;
    //////////////////////
  
    virtual void write( CvFileStorage* storage, const char* name ) const;
    virtual void read( CvFileStorage* storage, CvFileNode* node );

protected:
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

#endif  // CLASSIFIER_INNER_BAYES_CLASSIFIER_H_

