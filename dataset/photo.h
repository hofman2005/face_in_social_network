/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 08 Mar 2012 03:41:09 PM
#
# Filename: photo.h
#
=========================================*/

#ifndef DATASET_PHOTO_H_
#define DATASET_PHOTO_H_

#include <string>
#include <cv.h>

namespace SocialNetwork {
class Photo {
 public:
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

  inline const std::string &GetTrueId() const {return true_id_;};
  inline const std::string &GetFilename() const {return image_file_;};
  inline const std::string &GetAssignedId() const {return assigned_id_;};
  inline const std::string &GetAssignedBy() const {return assigned_by_;};

  void SetAssignedId(const std::string &id, const std::string &by);

  cv::Mat GetImage(const std::string &prefix);
  // void ReleaseImage();

 private:
  std::string true_id_;
  std::string image_file_;
  // cv::Mat *image_;
  std::string assigned_id_;
  std::string assigned_by_;
};

}

#endif  // DATASET_PHOTO_H_
