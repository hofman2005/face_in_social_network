/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 01 May 2012 03:28:28 AM
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

// void Photo::SetTrueId(const std::string& id) {
//   true_id_ = id;
// }

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

int Photo::ReadFromStream(std::ifstream& in) {
  in >> true_id_ >> image_file_ >> assigned_id_ >> assigned_by_;
  // // Check if there is record for scores
  // char first;
  // in.get(first);
  // if (first == '\n') {
  //   return 0;
  // } else {
  //   in.putback(first);
  // }
  // std::stringbuf buf;
  // in.get(buf, '\n');
  // std::stringstream ss(buf.str());
  // std::string id;
  // double dist;
  // res_.clear();
  // ss >> id >> dist;
  // while (ss.good()) {
  //   res_[id] = dist;
  //   ss >> id >> dist;
  // }
  in >> res_;
  return 0;
}

int Photo::WriteToStream(std::ofstream& out) const {
  out << true_id_ << " "
      << image_file_ << " "
      << assigned_id_ << " "
      << assigned_by_ << " ";
  // PhotoRes::const_iterator it;
  // for (it=res_.begin(); it!=res_.end(); ++it) {
  //   out << it->first << " " << it->second << " ";
  // }
  // out << std::endl;

  out << res_;
  return 0;
}

const std::string& Photo::DecideId() {
  // if (assigned_by_ == "God") return assigned_id_;
  // std::vector< std::pair<std::string, double> > vec(res_.begin(), res_.end());
  // std::sort(vec.begin(), vec.end(), IntCmp());
  // std::string final_id = "";
  // if (vec.size() >= 1) final_id = vec[0].first;
  // assigned_id_ = final_id;
  // assigned_by_ = "Final";
  // return final_id;
  return res_.GetSortedDecision(0);
}
}
