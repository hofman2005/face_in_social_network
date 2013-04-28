/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
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

  static void SetPrefix(std::string& prefix) {
    image_prefix_ = prefix;
  };

  inline const std::string& GetTrueId() const {return true_id_;};
  inline const std::string& GetFilename() const {return image_file_;};
  inline const std::string& GetAssignedId() const {return assigned_id_;};
  inline const std::string& GetAssignedBy() const {return assigned_by_;};
  inline FaceRecognition::PhotoResult& GetPhotoRes() {return res_;}; 
  inline const FaceRecognition::PhotoResult& GetPhotoRes() const {
    return res_;
  };

  //void SetTrueId(const std::string &id);
  void SetAssignedId(const std::string &id, const std::string &by);
  const std::string DecideId() const;

  cv::Mat GetImage(const std::string &prefix);
  cv::Mat GetImage() const;
  cv::Mat GetFeature() const;
  // void ReleaseImage();
  
  int WriteToStream(std::ofstream& out) const;
  int ReadFromStream(std::istringstream& in);

 private:
  std::string true_id_;
  std::string image_file_;
  // cv::Mat *image_;
  std::string assigned_id_;
  std::string assigned_by_;

  // Some meta information used for belief propagation?
  // PhotoRes res_;
  FaceRecognition::PhotoResult res_;

  static std::string image_prefix_;
};

// typedef std::vector<Photo> Album;
// typedef std::map< std::string, Album > AlbumMap;

}

// #include "dataset/album.h"

#endif  // DATASET_PHOTO_H_
