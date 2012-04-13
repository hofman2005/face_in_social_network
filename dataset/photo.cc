/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 08 Mar 2012 06:12:17 PM
#
# Filename: photo.cc
#
=========================================*/

#include "dataset/photo.h"
#include <highgui.h>

namespace SocialNetwork {

Photo::~Photo() {
//  ReleaseImage();
}

void Photo::SetAssignedId(const std::string &id, 
                          const std::string &by) {
  assigned_id_ = id;
  assigned_by_ = by;
}

cv::Mat Photo::GetImage(const std::string &prefix) {
//  if (image_) {
//    return image_;
//  } else {
    // Load image
    //return cv::Mat();
    return cv::imread(prefix+image_file_, 0);
//  }
}

// void Photo::ReleaseImage() {
//   if (image_) delete image_;
//   image_ = NULL;
// }

}
