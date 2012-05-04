/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 01 May 2012 04:44:03 PM
#
# Filename: baseline.cc
#
# Description: Baseline of non-propagation. Classifiers are trained and tested on each node independently.
#
=========================================*/

#include <boost/program_options.hpp>
#include "dataset/social_graph.h"
#include "classifier/pca_classifier.h"

namespace FaceRecognition {
namespace sn = SocialNetwork;
namespace fn = FaceRecognition;
int TrainOnSingleAlbum(const std::string& image_prefix,
                       sn::Album* album,
                       BaseClassifier* classifier) {
  FaceRecognition::ImageList image_list;
  std::string id;
  // Load images
  for (sn::Album::iterator it = album->begin(); it != album->end(); ++it) {
    id = (*it).GetAssignedId();
    // if ( id == "-") continue;
    cv::Mat *image = new cv::Mat;
    *image  = (*it).GetImage(image_prefix);
    image_list.push_back(std::make_pair(image, id));
  }
  classifier->Train(image_list);
  for (ImageList::iterator it = image_list.begin();
       it!=image_list.end();
       ++it) {
    delete (*it).first;
  }
  return 0;
}

int TestOnSingleAlbum(const std::string& image_prefix, 
                      BaseClassifier* classifier,
                      sn::Album* album) {
  std::string id;
  std::string assigned_by;
  float count = 0;
  float correct_count = 0;
  for (sn::Album::iterator it = album->begin(); it != album->end(); ++it) {
    id = (*it).GetAssignedId();
    //if ( id != "-") continue;
    
    // // Do not change those initial labels
    assigned_by = (*it).GetAssignedBy();
    if (assigned_by == "God") continue;

    cv::Mat image  = (*it).GetImage(image_prefix);

    // // Identification
    // std::string res_id;
    // // if (BaseBehavior<Classifier>::classifier_.Identify(image, &res_id)) {
    // if (classifier->Identify(image, &res_id)) {
    //   // If identification is successful
    //   (*it).SetAssignedId(res_id, (*graph_)[current].person_id); 
    // };

    // Multi candidate identification
    // std::map<std::string, double>& res = it->GetRes();
    // std::map<std::string, double> new_res;
    // classifier->Identify(image, &new_res);
    //  
    // // Merge result
    // for (std::map<std::string, double>::iterator res_it = new_res.begin();
    //      res_it != new_res.end();
    //      ++res_it) {
    //   if (res.find(res_it->first)!=res.end()) {
    //     if (res_it->second < res[res_it->first]) {
    //       res[res_it->first] = res_it->second;
    //     }
    //   } else {
    //     res[res_it->first] = res_it->second;
    //   }
    // }
    fn::PhotoResult& res = it->GetPhotoRes();
    classifier->Identify(image, &res);

    // std::vector< std::pair<std::string, double> > vec(res.begin(), res.end());
    // std::sort(vec.begin(), vec.end(), sn::IntCmp());
    // std::string res_id = vec[0].first;
    // const double threshold = 3.0f;
    // if (vec[1].second / vec[0].second > threshold) {
    //   res_id = vec[0].first;
    //   (*it).SetAssignedId(res_id, (*graph_)[current].person_id); 
    // } else {
    //   res_id = "-";
    // }
    std::string res_id = res.GetSortedDecision(0);
    (*it).SetAssignedId(res_id, "Baseline");

    // Simple analysis
    ++count;
    if (res_id == (*it).GetTrueId()) ++correct_count;
    //if (res_id != "-") ++decision_count;

    //if (res_id != id) ++num_label_changed;
  }
  // std::cout << " Accuracy: " << correct_count/decision_count*100 << "%" << std::endl;
  // std::cout << " Decision made on " << decision_count << " of " << count << std::endl;
  std::cout << " Accuracy: " << correct_count/count*100 << "%" << std::endl;

  // Save the album
  // char output_album_file[80];
  // sprintf(output_album_file, "/tmp/sn_aa.alb");
  // WriteAlbumMapToFile(*album_map, output_album_file);

  return 0;
}

}

int main(int argc, char** argv) {
  namespace sn = SocialNetwork;
  namespace fn = FaceRecognition;
  namespace po = boost::program_options;

  // Parse options
  std::string image_prefix;
  std::string graph_prefix;
  std::string output_prefix;

  po::options_description options("command line options");
  options.add_options() 
    ("help,h", "Use -h or --help to list all arguments")
    ("image-prefix", boost::program_options::value<std::string>(&image_prefix), 
     "Image prefix.")
    ("graph-prefix", boost::program_options::value<std::string>(&graph_prefix), 
     "Graph file prefix. Extension .dot and .alb will be appended.")
    ("output-prefix,o", boost::program_options::value<std::string>(&output_prefix), 
     "Output")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, options), vmap);
  po::notify(vmap);

  if (vmap.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }
  if (!vmap.count("image-prefix") || 
      !vmap.count("graph-prefix") || 
      !vmap.count("output-prefix")) {
    std::cout << options << std::endl;
    return 0;
  }

  // Load graph and album
  sn::SocialGraph graph(0);
  std::string graph_filename = graph_prefix + ".dot";
  std::cout << "Loading graph file: " << graph_filename << std::endl;
  sn::ReadGraphFromFile(graph_filename, &graph);

  sn::AlbumMap album_map;
  std::string album_filename = graph_prefix + ".alb";
  std::cout << "Loading album file: " << album_filename << std::endl;
  sn::ReadAlbumMapFromFile(album_filename, &album_map);

  // Train and test classifier on each vertex.
  sn::VertexIterator vi, vi_end;
  for (tie(vi,vi_end)=vertices(graph);
       vi != vi_end;
       ++vi) {
    fn::PCAClassifier classifier;

    // Train
    if (graph[*vi].person_id == "Infection_Source") continue;
    std::cout << "Training on " << graph[*vi].person_id << std::endl;

    sn::Album& album = album_map[graph[*vi].person_id];
    fn::TrainOnSingleAlbum(image_prefix, &album, &classifier);

    // Test
    std::cout << "Testing on " << graph[*vi].person_id << std::endl;
    fn::TestOnSingleAlbum(image_prefix, &classifier, &album);
  }

  // Save the album
  std::string output_album_file = output_prefix + ".alb";
  sn::WriteAlbumMapToFile(album_map, output_album_file);
}
