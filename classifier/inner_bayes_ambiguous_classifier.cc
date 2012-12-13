#include "classifier/inner_bayes_ambiguous_classifier.h"
#include <assert.h>

namespace FaceRecognition {
InnerBayesAmbiguousClassifier::~InnerBayesAmbiguousClassifier() {
  clear();
}

void InnerBayesAmbiguousClassifier::clear()
{
  for( int cls = 0; cls < nclasses; cls++ )
  {
    cvReleaseMat( &count[cls] );
    cvReleaseMat( &sum[cls] );
    cvReleaseMat( &productsum[cls] );
    cvReleaseMat( &avg[cls] );
    cvReleaseMat( &inv_eigen_values[cls] );
    cvReleaseMat( &cov_rotate_mats[cls] );
  }
  nclasses = 0;

  // cvReleaseMat( &cls_labels );
  // cvReleaseMat( &var_idx );
  cvReleaseMat( &c );
  cvFree( &count );
}


bool InnerBayesAmbiguousClassifier::train(const cv::Mat& train_data,
                                          const cv::Mat& score_table) {
  const float min_variation = FLT_EPSILON;

  assert(train_data.rows == score_table.rows);

  bool result = false;

  clear();
  std::cout << "Start Kernel Training." << std::endl;

  nclasses = score_table.cols;
  var_count = train_data.cols;

  const size_t mat_size = sizeof(CvMat*);
  size_t data_size = nclasses*6*mat_size;
  count = (CvMat**)cvAlloc( data_size );  
  memset( count, 0, data_size );

  sum             = count      + nclasses;
  productsum      = sum        + nclasses;
  avg             = productsum + nclasses;
  inv_eigen_values= avg        + nclasses;
  cov_rotate_mats = inv_eigen_values         + nclasses;

  c = cvCreateMat( 1, nclasses, CV_64FC1 );

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

  CvMat * cov = cvCreateMat( var_count, var_count, CV_64FC1 );

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

      for (int c1=0; c1 < var_count; ++c1, prod_data += var_count) {
        double val1 = train_vec[c1];
        sum_data[c1] += val1 * score;
        count_data[c1] += score;
        for (int c2=c1; c2 < var_count; ++c2) {
          prod_data[c2] += train_vec[c2] * val1 * score;
        }
      }
    }
  }

  // Calculate avg, covariance matrix, c
  for (int cls = 0; cls < nclasses; ++cls) {
    std::cout << "." << std::flush;
    double det = 1;
    // int i, j;
    CvMat* w = inv_eigen_values[cls];
    int* count_data = count[cls]->data.i;
    double* avg_data = avg[cls]->data.db;
    double* sum1 = sum[cls]->data.db;

    cvCompleteSymm( productsum[cls], 0 );

    for (int j=0; j<var_count; ++j) {
      double n = count_data[j];
      avg_data[j] = n ? sum1[j] / n : 0.;
    }

    // count_data = count[cls]->data.i;
    // avg_data = avg[cls]->data.db;
    // sum1 = sum[cls]->data.db;

    for (int i=0; i<var_count; ++i) {
      double* avg2_data = avg[cls]->data.db;
      double* sum2 = sum[cls]->data.db;
      double* prod_data = productsum[cls]->data.db + i*var_count;
      double* cov_data = cov->data.db + i*var_count;
      double s1val = sum1[i];
      double avg1 = avg_data[i];
      double count = count_data[i];

      for(int j = 0; j <= i; j++ ) {
        double avg2 = avg2_data[j];
        double cov_val = prod_data[j] - avg1 * sum2[j] - avg2 * s1val + avg1 * avg2 * count;
        // cov_val = (count > 1) ? cov_val / (count - 1) : cov_val;
        cov_val = cov_val / count;
        cov_data[j] = cov_val;
      }
    }

    cvCompleteSymm( cov, 1 );

    // Fix singular matrix
    for (int t = 0; t < cov->rows; ++t) {
      *(cov->data.db + t * cov->rows + t) += 1;
    }

    cvSVD( cov, w, cov_rotate_mats[cls], 0, CV_SVD_U_T );
    cvMaxS( w, min_variation, w);
    for( int j = 0; j < var_count; j++ ) {
      det *= w->data.db[j];
    }

    cvDiv( NULL, w, w );
    c->data.db[cls] = det > 0 ? log(det) : -700;
  }
  std::cout << std::endl;

  result = true;

  if (!result || cvGetErrStatus() < 0)
    clear();

  cvReleaseMat( &cov );
  //cvReleaseMat( &__cls_labels );
  //cvReleaseMat( &__var_idx );

  return result;
}

bool InnerBayesAmbiguousClassifier::predict(const cv::Mat& test_data,
                                            std::map<int, double>* res) {
  // Only process one sample
  assert(test_data.rows == 1);

  int ival;
  int cls = -1;
  double opt = FLT_MAX;
  const double SCALE = 100000.0;
  // CvMat diff = cvMat( 1, var_count, CV_64FC1, &buffer[0] );
  // CvMat diff = cvMat( 1, var_count, CV_64FC1 );
  CvMat * diff = cvCreateMat( 1, var_count, CV_64FC1 );

  for(int i = 0; i < nclasses; i++ )
  {

    double cur = c->data.db[i];
    CvMat* u = cov_rotate_mats[i];
    CvMat* w = inv_eigen_values[i];

    const double* avg_data = avg[i]->data.db;
    // const float* x = (const float*)(samples->data.ptr + samples->step*k);
    const double* x = test_data.ptr<double>();

    // cov = u w u'  -->  cov^(-1) = u w^(-1) u'
    for(int j = 0; j < var_count; j++ ) {
      // diff.data.db[j] = avg_data[j] - x[vidx ? vidx[j] : j];
      diff->data.db[j] = avg_data[j] - x[j];
    }

    cvGEMM( diff, u, 1, 0, 0, diff, CV_GEMM_B_T );
    for(int j = 0; j < var_count; j++ )
    {
      double d = diff->data.db[j];
      cur += d*d*w->data.db[j];
    }

    cur += c->data.db[i];
    
    cur = exp(- cur / SCALE);

    // if( cur < opt )
    // {
    //   cls = i;
    //   opt = cur;
    // }
    /* probability = exp( -0.5 * cur ) */

    if (res != NULL) {
      // (*res)[cls_labels->data.i[i]] = cur;
      (*res)[i] = cur;
      //std::cout << i << " " << cur << std::endl;
    }
  }

  cvReleaseMat( &diff );

  return true;
}

}
