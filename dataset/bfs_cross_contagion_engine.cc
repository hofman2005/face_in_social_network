/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Apr 2012 05:16:39 PM
#
# Filename: cross_contagion_engine.cc
#
=========================================*/

#include "bfs_cross_contagion_engine.h"

namespace SocialNetwork {
int BfsCrossContagionEngine::Init(const std::string& start_id) {
  // Initialize the queue
  vertex_status_.clear();
  vertex_status_.resize(num_vertices(*graph_), UNVISITED);

  VertexIterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(*graph_); vi!=vi_end; ++vi) {
    //if (graph[*vi].person_id == "Infection_Source") {
    if ((*graph_)[*vi].person_id == start_id) {
      break;
    }  
  }
  // Check if found the Infection_Source
  assert(vi!=vi_end);

  // Add the start point to the queue
  assert(Q_.size() == 0);
  Q_.push(*vi);
  return 0;
}

int BfsCrossContagionEngine::Run() {
  AdjacencyIterator adj, adj_end;
  while (Q_.size() > 0) {
    Vertex& v = Q_.front();
    Q_.pop();
    // Discover unvisited neighbors
    for (tie(adj, adj_end) = adjacent_vertices(v, *graph_); adj != adj_end; ++adj) {
      if (vertex_status_[*adj] == UNVISITED) {
        Q_.push(*adj);
        vertex_status_[*adj] = IN_QUEUE;
      }
    }
    // Process the vertex
    // Cross contagion
    vertex_status_[v] = VISITED;
  }
  return 0;
}
}
