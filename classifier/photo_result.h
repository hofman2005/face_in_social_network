#ifndef CLASSIFIER_PHOTO_RESULT_H_
#define CLASSIFIER_PHOTO_RESULT_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>


namespace FaceRecognition {
class PhotoResult {
 public:
  PhotoResult() : cache_dirty_(false) {};
  int AddRecord(const std::string& id, const double score);
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
  std::vector< std::pair<std::string, double> > cache_sorted_score_;
  bool cache_dirty_;
  struct IntCmp {
    bool operator()(const std::pair<std::string, double>& lhs, 
                    const std::pair<std::string, double>& rhs) {
      return lhs.second > rhs.second;
    };
  };
};

inline std::istream& operator>> (std::istream& in, PhotoResult& photo_result) {
  return photo_result.ReadFromStream(in);
};

inline std::ostream& operator<< (std::ostream& out, const PhotoResult& photo_result) {
  return photo_result.WriteToStream(out);
};
 
}

#endif  // CLASSIFIER_PHOTO_RESULT_H_
