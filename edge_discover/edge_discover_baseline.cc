/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: edge_discover_baseline.cc
#
=========================================*/
#include "edge_discover/edge_discover_baseline.h"
#include <assert.h>
#include "classifier/photo_result.h"

namespace SocialNetwork {
using namespace std;

void EdgeDiscoverBaseline::Run(const SocialGraph& graph,
                               const AlbumMap& album_map,
                               map<pair<string, string>, double>& result) {
  VertexIterator vi, vi_end;
  int count = 1;
  int total = num_vertices(graph);
  total = total * (total-1) / 2;
  for (tie(vi, vi_end) = vertices(graph);
      vi != vi_end; 
      ++vi) {
    VertexIterator vii = vi;
    ++vii;
    const Album& album_1 = album_map.find(graph[*vi].person_id)->second;
    for (; vii != vi_end; ++vii) {
      cout << "Examining " << count << " of " << total << ": " << flush;
      if (edge(*vi, *vii, graph).second == 0) {
        const Album& album_2 = album_map.find(graph[*vii].person_id)->second;
        double dist1 = CalcDistance(album_1, album_2);
        double dist2 = CalcDistance(album_2, album_1);
        double dist = (dist1 + dist2) / 2.0;
        result[make_pair(graph[*vi].person_id, graph[*vii].person_id)] = dist;
        cout << graph[*vi].person_id << " " << graph[*vii].person_id << " " << dist << endl;
      }
      else {
        cout << endl;
      }
      ++count;
    }
  }
}

double EdgeDiscoverBaseline::CalcDistance(const Album& album_1, const Album& album_2) {
  map<string, int> hash_table;
  int count_1 = 0;
  for (int i=0; i<album_1.size(); ++i) {
    string id = album_1[i].DecideId();
    ++ hash_table[id];
    if (id != "-")
      ++count_1;
  }
  int hit_count_1 = 0;
  int total_count = 0;
  for (int i=0; i<album_2.size(); ++i) {
    string test_id = album_2[i].DecideId();
    if (test_id != "-") {
      if (hash_table.count(test_id) > 0) {
        ++hit_count_1;
      }
      ++total_count;
    }
  }
  if (count_1 == 0) return 1;
  else return 1.0 - static_cast<double>(hit_count_1) / static_cast<double>(count_1);
  //else return 1.0 - static_cast<double>(hit_count_1) / static_cast<double>(total_count);
}
}
