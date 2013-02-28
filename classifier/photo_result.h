/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: photo_result.h
#
=========================================*/
#ifndef CLASSIFIER_PHOTO_RESULT_H_
#define CLASSIFIER_PHOTO_RESULT_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>


namespace FaceRecognition {
class PhotoResult {
  private:
    struct FullRecord {
      double score;
      std::string id;
      std::string source;
      FullRecord() {};
      FullRecord(const std::string& _id, const double _score, const std::string& _source)
        : score(_score), id(_id), source(_source) {};
    };
 public:
  enum MergeType {TAKE_MAX, TAKE_MEAN, TAKE_MIN};
  PhotoResult() : cache_dirty_(false) {};
  int AddRecord(const std::string& id, const double score, MergeType merge_type=TAKE_MIN);
  int AddRecord(const std::string& id, const double score, const std::string& source);
  double GetRecord(const std::string& id) const;
  inline int GetNumRecord() const {return score_map_.size();};
  
  const std::string GetSortedDecision(const int rank,
                                      double* score = NULL,
                                      std::string* id = NULL); // Score are sorted in ascending order.
  
  std::istream& ReadFromStream(std::istream& in);
  std::ostream& WriteToStream(std::ostream& out) const;

  class PhotoResultError {};
 
 private:
  int sort_score();
  std::map<std::string, double> score_map_;
  std::map<std::string, int> score_count_;
  std::vector< std::pair<std::string, double> > cache_sorted_score_;
  bool cache_dirty_;
  struct IntCmp {
    bool operator()(const std::pair<std::string, double>& lhs, 
                    const std::pair<std::string, double>& rhs) {
      return lhs.second < rhs.second;
    };
  };
  std::vector<FullRecord> record_;
};

inline std::istream& operator>> (std::istream& in, PhotoResult& photo_result) {
  return photo_result.ReadFromStream(in);
};

inline std::ostream& operator<< (std::ostream& out, const PhotoResult& photo_result) {
  return photo_result.WriteToStream(out);
};
 
}

#endif  // CLASSIFIER_PHOTO_RESULT_H_
