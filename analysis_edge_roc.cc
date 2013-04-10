/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: analysis_edge_roc.cc
#
=========================================*/
#include <boost/program_options.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "dataset/social_graph.h"

using namespace std;

struct EdgeDiscoverResult {
  string id1, id2;
  double score;
  EdgeDiscoverResult() {};
  EdgeDiscoverResult(string& id1_, string& id2_, double score_)
    : id1(id1_), id2(id2_), score(score_) {};
};

struct EdgeDiscoverResultCmp {
  bool operator()(const EdgeDiscoverResult& a, const EdgeDiscoverResult& b) {
    return a.score < b.score;
  };
};

int main(int argc, char **argv) {
  namespace sn = SocialNetwork;
  namespace po = boost::program_options;

  // Parse options
  string graph_ori_filename;
  string graph_filename;
  string edge_result_filename;
  string output_filename;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("original-graph", po::value<string>(&graph_ori_filename), 
     "The original graph file.")
    ("reduced-graph", po::value<string>(&graph_filename), 
     "The reduced graph file.")
    ("edge-result", po::value<string>(&edge_result_filename),
     "The edge result file.")
    ("output,o", po::value<string>(&output_filename), 
     "Output")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, options), vmap);
  po::notify(vmap);

  if (vmap.count("help")) {
    cout << options << endl;
    return 0;
  }
  if (!vmap.count("original-graph") || 
      !vmap.count("reduced-graph") || 
      !vmap.count("output")) {
    cout << options << endl;
    return 0;
  }

  // Load graph and album
  sn::SocialGraph graph_ori(0);
  cout << "Loading graph file: " << graph_ori_filename << endl;
  sn::ReadGraphFromFile(graph_ori_filename, &graph_ori);

  sn::SocialGraph graph(0);
  cout << "Loading graph file: " << graph_filename << endl;
  sn::ReadGraphFromFile(graph_filename, &graph);

  // Build the table of lost edges
  set<string> preserved_edges, lost_edges;
  sn::EdgeIterator ei, ei_end;
  for (boost::tie(ei, ei_end) = boost::edges(graph_ori);
      ei != ei_end;
      ++ei) {
    sn::Vertex s = boost::source(*ei, graph_ori);
    sn::Vertex t = boost::target(*ei, graph_ori);
    lost_edges.insert(graph_ori[s].person_id + graph_ori[t].person_id);
  }
  for (boost::tie(ei, ei_end) = boost::edges(graph);
      ei != ei_end;
      ++ei) {
    sn::Vertex s = boost::source(*ei, graph);
    sn::Vertex t = boost::target(*ei, graph);
    if (lost_edges.count(graph[s].person_id + graph[t].person_id) > 0) {
      lost_edges.erase(graph[s].person_id + graph[t].person_id);
    }
    if (lost_edges.count(graph[t].person_id + graph[s].person_id) > 0) {
      lost_edges.erase(graph[t].person_id + graph[s].person_id);
    }
    preserved_edges.insert(graph[s].person_id + graph[t].person_id);
  }

  // Load edge result
  ifstream edge_result_file(edge_result_filename.c_str());
  string id1, id2;
  double score;
  vector<EdgeDiscoverResult> result;  
  while (edge_result_file.good()) {
    edge_result_file >> id1 >> id2 >> score;
    // cout << id1 << " " << id2 << " " << score << endl;
    result.push_back(EdgeDiscoverResult(id1, id2, score));
  }
  sort(result.begin(), result.end(), EdgeDiscoverResultCmp());

  vector<double> tpr(1), fpr(1);
  for (int i=0; i<result.size(); ++i) {
    if (preserved_edges.count(result[i].id1 + result[i].id2) > 0 ||
        preserved_edges.count(result[i].id2 + result[i].id1) > 0) {
      continue;
    }
    else if (lost_edges.count(result[i].id1 + result[i].id2) > 0 ||
             lost_edges.count(result[i].id2 + result[i].id1) > 0) {
      tpr.push_back(tpr.back()+1);
      fpr.push_back(fpr.back());
    }
    else {
      tpr.push_back(tpr.back());
      fpr.push_back(fpr.back()+1);
    }
  }
  for (int i=0; i<tpr.size(); ++i) {
    tpr[i] /= tpr.back();
    fpr[i] /= fpr.back();
  }

  // Output
  ofstream out_file(output_filename.c_str());
  for (int i=0; i<tpr.size(); ++i) {
    out_file << tpr[i] << " " << fpr[i] << endl;
  }
}
