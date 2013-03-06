/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Filename: album.h
#
=========================================*/
#ifndef __ALBUM_H__
#define __ALBUM_H__

#include <vector>
#include "dataset/photo.h"

namespace SocialNetwork {
class Album {
  public:
    typedef std::vector<Photo>::iterator iterator;
    typedef std::vector<Photo>::const_iterator const_iterator;

    iterator begin() {return album_.begin();};
    iterator end() {return album_.end();};
    const_iterator begin() const {return album_.begin();};
    const_iterator end() const {return album_.end();};

    size_t size() const {return album_.size();};

    Photo& operator[](int i) {return album_[i];};
    const Photo& operator[] (int i) const {
      return album_[i];
    };

    void push_back(const Photo& photo) {album_.push_back(photo);};

  private:
    std::vector<Photo> album_;
};

typedef std::map< std::string, Album > AlbumMap;
}

#endif  // __ALBUM_H__
