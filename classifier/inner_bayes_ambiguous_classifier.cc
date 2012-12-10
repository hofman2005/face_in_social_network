#include "classifier/inner_bayes_ambiguous_classifier.h"
#include <assert.h>

namespace FaceRecognition {
InnerBayesAmbiguousClassifier::~InnerBayesAmbiguousClassifier() {
  clear();
}

void InnerBayesAmbiguousClassifier::clear()
{
    if( cls_labels )
    {
        for( int cls = 0; cls < max_labels; cls++ )
        {
            cvReleaseMat( &count[cls] );
            cvReleaseMat( &sum[cls] );
            cvReleaseMat( &productsum[cls] );
            cvReleaseMat( &avg[cls] );
            cvReleaseMat( &inv_eigen_values[cls] );
            cvReleaseMat( &cov_rotate_mats[cls] );
        }
    }

    cvReleaseMat( &cls_labels );
    cvReleaseMat( &var_idx );
    cvReleaseMat( &c );
    cvFree( &count );
}


bool InnerBayesAmbiguousClassifier::train(cv::Mat& train_data,
                                          cv::Mat& score_table) {
  assert(train_data.rows == score_table.rows);

  bool result = false;

  int nclasses = score_table.cols;
  int var_count = train_data.cols;

  for (int cls=0; cls<nclasses; ++cls) {
    count[cls]            = cvCreateMat( 1, var_count, CV_64FC1 );
    sum[cls]              = cvCreateMat( 1, var_count, CV_64FC1 );
    productsum[cls]       = cvCreateMat( var_count, var_count, CV_64FC1 );
    avg[cls]              = cvCreateMat( 1, var_count, CV_64FC1 );
    inv_eigen_values[cls] = cvCreateMat( 1, var_count, CV_64FC1 );
    cov_rotate_mats[cls]  = cvCreateMat( var_count, var_count, CV_64FC1 );
    cvZero( count[cls] );
    cvZero( sum[cls] );
    cvZero( productsum[cls] );
    cvZero( avg[cls] );
    cvZero( inv_eigen_values[cls] );
    cvZero( cov_rotate_mats[cls] );
  }

  int nsamples = score_table.rows;

  cov = cvCreateMat( _var_count, _var_count, CV_64FC1 );

  // Calculate count, sum, productsum
  for (int s = 0; s < nsamples; ++s) {
    for (int cls = 0; cls < nclasses; ++cls) {
      double score = score_table.at<double>(s, cls);
      if (score < 1e-10)
        continue;

      int* count_data = count[cls]->data.i;
      double* sum_data = sum[cls]->data.db;
      double* prod_data = productsum[cls]->data.db;
      const double* train_vec = train_data.ptr<double>(s);

        for (int c1=0; c1 < var_count; ++c1) {
        double val1 = train_vec[c1];
        sum_data[c1] += val1 * score;
        count_data[c1] += score;
        // for (int c2=c1; c2 < var_count; ++c2) {
        //   prod_data[c2] += train_vec[c2] * val1;
        // }
      }
    }
  }

  // Calculate avg, covariance matrix, c
  for (int cls = 0; cls < nclasses; ++cls) {
    std::cout << "." << std::flush;
    double det = 1;
    int i, j;
    CvMat* w = inv_eigen_values[cls];
    int* count_data = count[cls]->data.i;
    double* avg_data = avg[cls]->data.db;
    double* sum1 = sum[cls]->data.db;

    for (int j=0; j<var_count; ++j) {
      double n = count_data[j];
      avg_data[j] = n ? sum1[j] / n : 0.;
    }

    // count_data = count[cls]->data.i;
    // avg_data = avg[cls]->data.db;
    // sum1 = sum[cls]->data.db;

    for (int i=0; i<var_count; ++i) {
    }

  }
  std::cout << endl;
  result = true;

  if (!result || cvGetErrStatus() < 0)
    clear();

  cvReleaseMat( &cov );
  cvReleaseMat( &__cls_labels );
  cvReleaseMat( &__var_idx );
  cvFree( &train_data );

  return result;
}

bool InnerBayesAmbiguousClassifier::predict(cv::Mat& test_data) {
  return false;
}

}
