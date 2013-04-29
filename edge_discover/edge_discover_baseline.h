/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: edge_discover_baseline.h
#
=========================================*/
#ifndef __EDGE_DISCOVER_BASELINE_H__
#define __EDGE_DISCOVER_BASELINE_H__

#include <map>
#include <string>
#include "dataset/social_graph.h"

namespace SocialNetwork {

class EdgeDiscoverBaseline {
  public:
    void Run(const SocialGraph& graph,
             const AlbumMap& album_map,
             std::map<std::pair<std::string, std::string>, double>& result);

  private:
    double CalcDistance(const Album& album_1, const Album& album_2);
};

}

#endif  // __EDGE_DISCOVER_BASELINE_H__ 
