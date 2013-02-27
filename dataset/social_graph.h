/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 26 Feb 2013 05:03:47 PM
#
# Filename: social_graph.h
#
=========================================*/

#ifndef DATASET_SOCIAL_GRAPH_H_
#define DATASET_SOCIAL_GRAPH_H_

#include <fstream>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <map>
#include <string>
#include "dataset/photo.h"

namespace SocialNetwork {
class PersonProperty;
typedef boost::adjacency_list<boost::listS, 
                              boost::vecS, 
                              boost::undirectedS, 
                              PersonProperty> _SocialGraph;

class PersonProperty {
 public:
  void SetColor(const int value);
  void SetColor(const int r, const int g, const int b);
  int GetColor();
  void GetColor(int *r, int *g, int *b);

  void SetAccuracy(const std::string& value) {accuracy=value;};
 
 public:
  std::string person_id;
  std::string color;
  std::string style;
  std::string accuracy;
};

typedef boost::graph_traits<_SocialGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<_SocialGraph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<_SocialGraph>::adjacency_iterator AdjacencyIterator;
typedef boost::graph_traits<_SocialGraph>::edge_descriptor Edge;

// TODO
// This is not a safe practice. Needs to refactor the code.
class SocialGraph : public _SocialGraph {
  public:
    SocialGraph() : _SocialGraph() {};
    SocialGraph(int a) : _SocialGraph(a) {};

    std::map<std::string, Vertex> label_map_;
};

class PropertyWriter {
 public:
  PropertyWriter(const SocialNetwork::SocialGraph &g) : g_(g) {};
  void operator() (std::ostream& out,
                   const SocialNetwork::Vertex& v);

 private:
  const SocialNetwork::SocialGraph &g_;
};

class BfsFaceRecognitionVisitor : public boost::default_bfs_visitor {
 public:
  BfsFaceRecognitionVisitor(const std::string &image_prefix,
                            AlbumMap *album_map,
                            std::map<std::string, int> *step_map)
    : image_prefix_(image_prefix),
      album_map_(album_map),
      step_map_(step_map),
      step_(0) {};

  void discover_vertex(Vertex current, const SocialGraph &graph);
  void examine_vertex(Vertex current, const SocialGraph &graph);

  // int GetStep(std::string person_id);
 
 private:
  const std::string &image_prefix_;
  std::map<std::string, int> *step_map_;
  AlbumMap *album_map_; 
  int step_;
};


void ReadGraphFromFile(const std::string &input_file, 
                       SocialGraph *graph);
void ReadGraphFromFile(const char* input_file, 
                       SocialGraph *graph);
void WriteGraphToFile(const SocialGraph &graph, 
                      std::string output_file);
void WriteGraphToFile(const SocialGraph &graph, 
                      const char* output_file);


void ReadAlbumMapFromFile(const std::string &input_file,
                          AlbumMap *album);
void ReadAlbumMapFromFile(const char* input_file,
                          AlbumMap *album);
void WriteAlbumMapToFile(const AlbumMap &album,
                         std::string output_file);
void WriteAlbumMapToFile(const AlbumMap &album,
                         const char* output_file);


const int AnalysisImageList(const std::string &filename,
                            AlbumMap *image_map); 
}

#endif  // DATASET_SOCIAL_GRAPH_H_
