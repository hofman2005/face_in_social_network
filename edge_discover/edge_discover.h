/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: edge_discover.h
#
=========================================*/
#ifndef __EDGE_DISCOVER_H__
#define __EDGE_DISCOVER_H__

#include <map>
#include <string>
#include "dataset/social_graph.h"
#include "classifier/base_classifier.h"

namespace SocialNetwork {
class EdgeDiscover {
  public:
    EdgeDiscover() : classifier_(NULL) {};
    void SetClassifier(FaceRecognition::BaseClassifier* classifier) {
      classifier_ = classifier;
    }
    
    void Run(const SocialGraph& graph, 
             const AlbumMap& album_map, 
             std::map<std::pair<std::string, std::string>, double>& result);

  private:
    double CalcDistance(const Album& album_1, const Album& album_2);
    double KLDistance(const Album& album_1, const Album& album_2);
    FaceRecognition::BaseClassifier* classifier_;
};
}

#endif  // __EDGE_DISCOVER_H__
