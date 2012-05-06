/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 01 May 2012 03:16:47 AM
#
# Filename: photo.h
#
=========================================*/

#ifndef DATASET_PHOTO_H_
#define DATASET_PHOTO_H_

#include <fstream>
#include <string>
#include <map>
#include <cv.h>

#include "classifier/photo_result.h"

namespace SocialNetwork {
class Photo {
  typedef std::map<std::string, double> PhotoRes;
 public:
  Photo() {};
  Photo(const std::string &id, const std::string &image_file)
    : true_id_(id), 
      image_file_(image_file), 
      // image_(NULL), 
      assigned_id_("-"), 
      assigned_by_("-") {};
  Photo(const std::string &id,
        const std::string &image_file,
        const std::string &assigned_id,
        const std::string &assigned_by)
    : true_id_(id), 
      image_file_(image_file), 
      // image_(NULL), 
      assigned_id_(assigned_id), 
      assigned_by_(assigned_by) {};
  ~Photo();

  inline const std::string& GetTrueId() const {return true_id_;};
  inline const std::string& GetFilename() const {return image_file_;};
  inline const std::string& GetAssignedId() const {return assigned_id_;};
  inline const std::string& GetAssignedBy() const {return assigned_by_;};
  inline FaceRecognition::PhotoResult& GetPhotoRes() {return res_;}; 

  //void SetTrueId(const std::string &id);
  void SetAssignedId(const std::string &id, const std::string &by);
  const std::string DecideId();

  cv::Mat GetImage(const std::string &prefix);
  // void ReleaseImage();
  
  int WriteToStream(std::ofstream& out) const;
  int ReadFromStream(std::ifstream& in);

 private:
  std::string true_id_;
  std::string image_file_;
  // cv::Mat *image_;
  std::string assigned_id_;
  std::string assigned_by_;

  // Some meta information used for belief propagation?
  // PhotoRes res_;
  FaceRecognition::PhotoResult res_;
};

// struct IntCmp {
//   bool operator()(const std::pair<std::string, double>& lhs, 
//                   const std::pair<std::string, double>& rhs) {
//     return lhs.second < rhs.second;
//   };
// };
}

#endif  // DATASET_PHOTO_H_
