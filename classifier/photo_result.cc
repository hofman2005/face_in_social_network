/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: photo_result.cc
#
=========================================*/
#include "classifier/photo_result.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>
#include <cmath>

namespace FaceRecognition {
using namespace std;
int PhotoResult::AddRecord(const string& id, const double score, MergeType merge_type) {
  AddRecord(id, score, ".");
  
  return 0;
}

void PhotoResult::ClearRecord() {
  cache_dirty_ = true;
  score_map_.clear();
  cache_sorted_score_.clear();
  record_.clear();
}

int PhotoResult::AddRecord(const string& id, const double score, const string& source) {
  record_.push_back(FullRecord(id, score, source));
  cache_dirty_ = true;
  return 0;
}

int PhotoResult::GetNumRecord() const {
  if (cache_dirty_ == true) sort_score();
  return score_map_.size();
}

double PhotoResult::GetRecord(const string& id) const {
  map<string, double>::const_iterator it = score_map_.find(id);
  if (it == score_map_.end()) {
    throw PhotoResultError();
  } else {
    return it->second;
  }
}

// Rank begin with 0.
const string PhotoResult::GetSortedDecision(const int rank,
                                            double* score,
                                            string* id) const {
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

int PhotoResult::sort_score() const {
  score_map_.clear();
  Vote();

  cache_sorted_score_.assign(score_map_.begin(), score_map_.end());
  sort(cache_sorted_score_.begin(), cache_sorted_score_.end(), IntCmp());

  // Prune();

  cache_dirty_ = false;
}

// Prune the result, save space.
// Prune is based on the result of sort_score()
void PhotoResult::Prune() {
  score_map_.clear();
  const size_t NUM=20;
  score_map_.insert(cache_sorted_score_.begin(), 
      cache_sorted_score_.begin() + min(cache_sorted_score_.size(), NUM));
  record_.clear();
  for (vector<pair<string, double> >::const_iterator it = cache_sorted_score_.begin();
      it != cache_sorted_score_.end();
      ++it) {
    record_.push_back(FullRecord(it->first, it->second, "Prune"));
  }
}

int PhotoResult::Vote() const {
  // Vote_Min();
  Vote_kNN();
  return 0;
}

int PhotoResult::Vote_Min() const {
  for (vector<FullRecord>::const_iterator it = record_.begin();
      it != record_.end(); 
      ++it) {
    if (score_map_.count(it->id) == 0) {
      score_map_[it->id] = it->score;
    }
    else {
      score_map_[it->id] = min(score_map_[it->id], it->score);
    }
  }
  return 0;
}

// Not compatible with Prune()
int PhotoResult::Vote_kNN() const {
  vector<FullRecord> record(record_);
  sort(record.begin(), record.end(), FullRecordCmp()); 
  // map<string, double> knn_score_map_;
  vector<FullRecord>::const_iterator it = record.begin();
  int count = 0;
  const int MAX_KNN = 5;
  while (it != record.end() && count < MAX_KNN) {
    score_map_[it->id] ++;
    ++it;
    ++count;
  }
  // cache_sorted_score_.assign(knn_score_map_.begin(), knn_score_map_.end());
  // for (vector<pair<string, double> >::iterator itt = cache_sorted_score_.begin();
  //     itt != cache_sorted_score_.end();
  //     ++itt) {
  //   itt->second = 1. / itt->second;
  // }
  // sort(cache_sorted_score_.begin(), cache_sorted_score_.end(), IntCmp());
  
  if (score_is_distance_) {
    for (map<string, double>::iterator itt = score_map_.begin();
        itt != score_map_.end();
        ++itt) {
      itt->second = 1. / itt->second;
    }
  }

  // cache_dirty_ = false;

  return 0;
}

istream& PhotoResult::ReadFromStream(istream& in) {
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

  FullRecord rec;
  ss >> rec.id >> rec.score >> rec.source;

  if (ss.good()) cache_dirty_ = true;
  while (ss.good()) {
    record_.push_back(rec);
    ss >> rec.id >> rec.score >> rec.source;
  }
  return in;
}

ostream& PhotoResult::WriteToStream(ostream& out) const {
  using namespace std;
  for (vector<FullRecord>::const_iterator it = record_.begin();
      it != record_.end();
      ++it) {
    out << it->id << " " << it->score << " " << it->source << " ";
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

double PhotoResultDistance(const PhotoResult& res1,
                           const PhotoResult& res2) {
  map<string, double> score_map_1, score_map_2;
  double total_score_1 = 0, total_score_2 = 0;
  string id;
  double score;
  for (int i=0; i<res1.GetNumRecord(); ++i) {
    res1.GetSortedDecision(i, &score, &id);
    if (res1.ScoreIsDistance()) 
      score = 1./score;
    score_map_1[id] = score;
    total_score_1 += score;
    if (score_map_2.count(id) == 0)
      score_map_2[id] = 0;
  }
  for (int i=0; i<res2.GetNumRecord(); ++i) {
    res2.GetSortedDecision(i, &score, &id);
    if (res2.ScoreIsDistance())
      score = 1./score;
    score_map_2[id] = score;
    total_score_2 += score;
    if (score_map_1.count(id) == 0) 
      score_map_1[id] = 0;
  }
  // Laplacian smooth
  const double alpha = 0.1;
  int size = score_map_1.size();
  for (map<string, double>::iterator it = score_map_1.begin();
      it != score_map_1.end();
      ++it) {
    it->second = (it->second + alpha) / (it->second + size * alpha);
  }
  for (map<string, double>::iterator it = score_map_2.begin();
      it != score_map_2.end();
      ++it) {
    it->second = (it->second + alpha) / (it->second + size * alpha);
  }
  double dist = 0.0;
  for (map<string, double>::iterator it = score_map_1.begin();
      it != score_map_1.end();
      ++it) {
    dist += fabs(it->second - score_map_2[it->first]);
  }
  return dist;
}
}
