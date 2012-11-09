/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Nov 2012 04:30:14 PM
#
# Filename: photo.cc
#
=========================================*/

#include "dataset/photo.h"
#include <highgui.h>

namespace SocialNetwork {

Photo::~Photo() {
}

void Photo::SetAssignedId(const std::string &id, 
                          const std::string &by) {
  assigned_id_ = id;
  assigned_by_ = by;
}

cv::Mat Photo::GetImage(const std::string &prefix) {
    return cv::imread(prefix+image_file_, 0);
}

int Photo::ReadFromStream(std::istringstream& in) {
  if (!in.good()) return -1;

  // Check if it is an empty line.
  char first;
  in.get(first);
  if (first == '\n') {
    return 0;
  } else {
    in.putback(first);
  }
  in >> true_id_ >> image_file_ >> assigned_id_ >> assigned_by_;
  in >> res_;
  return 0;
}

int Photo::WriteToStream(std::ofstream& out) const {
  out << true_id_ << " "
      << image_file_ << " "
      << assigned_id_ << " "
      << assigned_by_ << " ";

  out << res_;
  return 0;
}

const std::string Photo::DecideId() {
  return res_.GetSortedDecision(0);
}
}
