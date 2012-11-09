/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 09 Nov 2012 04:09:11 PM
#
# Filename: social_graph.cc
#
=========================================*/

#include "dataset/social_graph.h"
#include <sstream>

namespace SocialNetwork {

void PersonProperty::SetColor(const int value) {
  char rgb[8];
  if (0 <= value && value <= 255) {
    sprintf(rgb, "#%2.2x%2.2x%2.2x", value, value, value);
    color = rgb;
  }
}

void PersonProperty::SetColor(const int r, const int g, const int b) {
  char rgb[8];
  if (0 <= r && r <= 255 &&
      0 <= g && g <= 255 &&
      0 <= b && b <= 255) {
    sprintf(rgb, "#%2.2x%2.2x%2.2x", r, g, b);
    color = rgb;
  }
}

int PersonProperty::GetColor() {
  int r_, g_, b_;
  GetColor(&r_, &g_, &b_);
  return (r_ + g_ + b_) / 3;
}

void PersonProperty::GetColor(int *r, int *g, int *b) {
  *r = atoi(color.substr(1,2).c_str());
  *g = atoi(color.substr(3,2).c_str());
  *b = atoi(color.substr(5,2).c_str());
}

void PropertyWriter::operator() (std::ostream& out, 
                                 const SocialNetwork::Vertex& v) {
  out << "[color=\"" << g_[v].color
      << "\",style=\"" << g_[v].style
      << "\",accuracy=\"" << g_[v].accuracy
      << "\"]";
}

void WriteGraphToFile(const SocialGraph &graph, 
                      std::string output_file) {
  WriteGraphToFile(graph, output_file.c_str());
}

void WriteGraphToFile(const SocialGraph &graph, 
                      const char* output_file) {
  PropertyWriter pw(graph);
  boost::default_writer vw;
  boost::default_writer gw;
  
  std::ofstream out(output_file);
  boost::write_graphviz(out, graph, pw, vw, gw, 
                        get(&PersonProperty::person_id, graph));
  out.close();
}

void ReadGraphFromFile(const std::string &input_file,
                       SocialGraph *graph) {
  boost::dynamic_properties dp;
  
  dp.property("label", get(&PersonProperty::person_id, *graph));
  dp.property("color", get(&PersonProperty::color, *graph));
  dp.property("style", get(&PersonProperty::style, *graph));
  dp.property("accuracy", get(&PersonProperty::accuracy, *graph));

  std::ifstream in(input_file.c_str());
  boost::read_graphviz(in, *graph, dp, "label");
  in.close();
}

void ReadAlbumMapFromFile(const std::string &input_file,
                          AlbumMap *album) {
  std::ifstream in(input_file.c_str());
  std::string content, file, assigned_id, assigned_by;
  std::string current_id;
  char first;
  while (true) {
    // in >> content;
    in.get(first);
    if (!in.good()) break;
    if (first == '\n') in.get(first); // Skip the \n at the end of each line.
    //if (content[0]=='#') {
    if (first == '#') {
      //current_id = content.substr(1, content.size()-1);
      in >> current_id;
    } else {
      //in >> file >> assigned_id >> assigned_by;
      //Photo *photo = new Photo(content, file, assigned_id, assigned_by);
      in.putback(first);
      Photo *photo = new Photo;
      getline(in, content);
      std::istringstream iss_content(content);
      if(photo->ReadFromStream(iss_content) == 0) {
        (*album)[current_id].push_back(*photo);
      }
    }
  }
}

void WriteAlbumMapToFile(const AlbumMap &album,
                         std::string output_file) {
  WriteAlbumMapToFile(album, output_file.c_str());
}

void WriteAlbumMapToFile(const AlbumMap &album,
                         const char* output_file) {
  std::ofstream out(output_file);
  for (AlbumMap::const_iterator album_iter = album.begin();
      album_iter != album.end();
      ++album_iter) {
    out << "#" << album_iter->first << std::endl;
    const Album &album = album_iter->second;
    for (Album::const_iterator photo_iter = album.begin();
         photo_iter != album.end();
         ++photo_iter) {
//       out << photo_iter->GetTrueId() << " "
//           << photo_iter->GetFilename() << " "
//           << photo_iter->GetAssignedId() << " "
//           << photo_iter->GetAssignedBy() << std::endl;
         photo_iter->WriteToStream(out);
    }
  }
  out.close();
}

const int AnalysisImageList(const std::string &filename,
                            AlbumMap *image_map) {
  std::ifstream input(filename.c_str());
  std::string old_id, id, file;

  while(input.good()) {
    input >> id >> file;
    Photo *photo = new Photo(id, file);
    (*image_map)[id].push_back(*photo);
  }
  input.close();

  return 0;
}
}

