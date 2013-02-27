#include "classifier/photo_result.h"
#include <algorithm>
#include <sstream>
#include <fstream>

namespace FaceRecognition {
int PhotoResult::AddRecord(const std::string& id, const double score, MergeType merge_type) {
  // Merge
  if (score_map_.find(id) != score_map_.end()) {
    if (merge_type == TAKE_MAX) {
      if (score > score_map_[id]) {
        score_map_[id] = score;
        cache_dirty_ = true;
      }
    } else if (merge_type == TAKE_MEAN) {
      double prev_score = score_map_[id] * score_count_[id];
      ++ score_count_[id];
      score_map_[id] = (prev_score + score) / score_count_[id];
    } else if (merge_type == TAKE_MIN) {
      if (score_map_.count(id) > 0 && score < score_map_[id]) {
        score_map_[id] = score;
        cache_dirty_ = true;
      }
    }
  } else {
    score_map_[id] = score;
    score_count_[id] = 1;
    cache_dirty_ = true;
  }

  // Keep the size down
  if (score_map_.size() > 20) {
    sort_score();
    score_map_.clear();
    score_map_.insert(cache_sorted_score_.begin(), cache_sorted_score_.begin()+20);

    std::map<std::string, int> score_count_new;
    for (std::map<std::string, double>::const_iterator it = score_map_.begin();
        it != score_map_.end();
        ++it) {
      score_count_new[it->first] = score_count_[it->first];
    }
    score_count_.swap(score_count_new);
  }

  // score_map_[id] += score;
  // cache_dirty_ = true;
  record_.push_back(FullRecord(id, score, ""));
  
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

  // DEBUG
  // out << "% DEBUG_BEGIN" << endl;
  // out << "%" ;
  // for (vector<FullRecord>::const_iterator itt = record_.begin(); 
  //     itt != record_.end();
  //     ++itt) {
  //   out << itt->id << " " << itt->score << " ";
  // }
  // out << endl;
  // out << "% DEBUG_END" << endl;
  
  return out;
}
}
