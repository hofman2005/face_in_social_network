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
  PhotoResult() : score_is_distance_(true), cache_dirty_(false) {};
  void SetScoreIsDistance(bool flag) {score_is_distance_ = flag;};
  bool ScoreIsDistance() const {return score_is_distance_;};
  int AddRecord(const std::string& id, const double score, MergeType merge_type=TAKE_MIN);
  int AddRecord(const std::string& id, const double score, const std::string& source);
  double GetRecord(const std::string& id) const;
  int GetNumRecord() const;
  void ClearRecord();
  
  const std::string GetSortedDecision(const int rank,
                                      double* score = NULL,
                                      std::string* id = NULL) const; // Score are sorted in ascending order.

  std::istream& ReadFromStream(std::istream& in);
  std::ostream& WriteToStream(std::ostream& out) const;

  class PhotoResultError {};
 
 private:
  int sort_score() const;
  int Vote() const;
  int Vote_Min() const;
  int Vote_kNN() const;
  void Prune();

  bool score_is_distance_;
  mutable std::map<std::string, double> score_map_;
  mutable std::vector< std::pair<std::string, double> > cache_sorted_score_;
  mutable bool cache_dirty_;
  struct IntCmp {
    bool operator()(const std::pair<std::string, double>& lhs, 
                    const std::pair<std::string, double>& rhs) const {
      return lhs.second < rhs.second;
    };
  };
  struct IntCmpGreater {
    bool operator()(const std::pair<std::string, double>& lhs, 
                    const std::pair<std::string, double>& rhs) const {
      return lhs.second > rhs.second;
    };
  };
  struct FullRecordCmp {
    bool operator()(const FullRecord& lhs, const FullRecord& rhs) const {
      return lhs.score < rhs.score;
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

double PhotoResultDistance(const PhotoResult& res1, const PhotoResult& res2);
 
}

#endif  // CLASSIFIER_PHOTO_RESULT_H_
