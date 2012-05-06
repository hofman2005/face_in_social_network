#include "classifier/photo_result.h"
#include <algorithm>
#include <sstream>

namespace FaceRecognition {
int PhotoResult::AddRecord(const std::string& id, const double score) {
  // Merge
  if (score_map_.find(id) != score_map_.end()) {
    if (score < score_map_[id]) {
      score_map_[id] = score;
      cache_dirty_ = true;
    }
  } else {
    score_map_[id] = score;
    cache_dirty_ = true;
  }
  
  return 0;
}

double PhotoResult::GetRecord(const std::string& id) const {
  std::map<std::string, double>::const_iterator it = score_map_.find(id);
  if (it == score_map_.end()) {
    throw PhotoResultError();
  } else {
    return it->second;
  }
}

// Rank begin with 0.
const std::string PhotoResult::GetSortedDecision(const int rank,
                                                  double* score,
                                                  std::string* id) {
  if (cache_dirty_) sort_score();
  if (cache_sorted_score_.size() == 0) {
    if(score) *score = 0;
    if (id) *id = "-";
    return "-";
  } else {
    if (score) *score = cache_sorted_score_[rank].second;
    if (id) *id = cache_sorted_score_[rank].first;
    return cache_sorted_score_[rank].first;
  }
}

int PhotoResult::sort_score() {
  using namespace std;
  cache_sorted_score_.assign(score_map_.begin(), score_map_.end());
  std::sort(cache_sorted_score_.begin(), cache_sorted_score_.end(), IntCmp());
  cache_dirty_ = false;
}

std::istream& PhotoResult::ReadFromStream(std::istream& in) {
  using namespace std;
  char first;
  in.get(first);
  if (first == '\n') {
    return in;
  } else {
    in.putback(first);
  }
  stringbuf buf;
  in.get(buf, '\n');
  stringstream ss(buf.str());
  string id;
  double score;
  score_map_.clear();
  ss >> id >> score;
  if (ss.good()) cache_dirty_ = true;
  while (ss.good()) {
    score_map_[id] = score;
    ss >> id >> score;
  }
  return in;
}

std::ostream& PhotoResult::WriteToStream(std::ostream& out) const {
  using namespace std;
  map<string, double>::const_iterator it;
  for (it=score_map_.begin(); it!=score_map_.end(); ++it) {
    out << it->first << " " << it->second << " ";
  }
  out << endl;
  return out;
}
}
