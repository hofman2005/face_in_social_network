/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: edge_discover.cc
#
=========================================*/
#include "edge_discover/edge_discover.h"
#include <assert.h>
#include <opencv/cv.h>
#include "classifier/photo_result.h"

namespace SocialNetwork {
using namespace std;
void EdgeDiscover::Run(const SocialGraph& graph,
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

double EdgeDiscover::CalcDistance(const Album& album_1, const Album& album_2) {
  classifier_->Reset();
  int res = classifier_->Train(album_1);
  if (res < 0) return 1e10;

  Album album(album_2);
  string assigned_by;
  cv::Mat image;
  for (int i=0; i<album_2.size(); ++i) {
    assigned_by = album[i].GetAssignedBy();  
    if ( ! (assigned_by == "Amb" || assigned_by == "-") ) 
      continue;
    image = album[i].GetImage();
    FaceRecognition::PhotoResult& res = album[i].GetPhotoRes();
    res.ClearRecord();
    classifier_->Identify(image, &res);
  }

  return KLDistance(album, album_2);
}

double EdgeDiscover::KLDistance(const Album& album_1, const Album& album_2) {
  double dist = 0;
  int count = 0;
  // assert(album_1.size() == album_2.size());
  for (int i=0; i<min(album_1.size(), album_2.size()); ++i) {
    if (album_1[i].GetFilename() != album_2[i].GetFilename()) {
      continue;
    }
    ++count;
    dist += PhotoResultDistance(album_1[i].GetPhotoRes(), album_2[i].GetPhotoRes());
  }
  return dist / count;
}
}
