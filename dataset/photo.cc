/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: photo.cc
#
=========================================*/

#include "dataset/photo.h"
#include <highgui.h>
#include <fstream>
#include <string>

namespace SocialNetwork {
  std::string Photo::image_prefix_ = "";

Photo::~Photo() {
}

void Photo::SetAssignedId(const std::string &id, 
                          const std::string &by) {
  assigned_id_ = id;
  assigned_by_ = by;
}

cv::Mat Photo::GetImage(const std::string &prefix) {
    return cv::imread(prefix+image_file_, 0);
    // return GetFeature(prefix);
}

cv::Mat Photo::GetImage() {
  // return cv::imread(image_prefix_+image_file_, 0);
  return GetFeature();
}

cv::Mat Photo::GetFeature() {
  std::string filename = image_prefix_+image_file_+".fea";
  std::ifstream file(filename.c_str()); 
  file.seekg(0, std::ios::end);
  int length = file.tellg();
  cv::Mat feature(1, length/sizeof(double), CV_64FC1);
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(feature.data), length);
  return feature;
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
