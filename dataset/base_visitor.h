/*=========================================
#
# Author: Tao Wu - taowu@umiacs.umd.edu
#
# Last-modified: 12 Apr 2012 01:03:25 PM
#
# Filename: base_visitor.h
#
=========================================*/

#ifndef DATASET_BASE_VISITOR_H_
#define DATASET_BASE_VISITOR_H_

#include "dataset/social_graph.h"

namespace SocialNetwork {
class BaseVisitor {
 public:
  BaseVisitor() {};
  virtual ~BaseVisitor() {};
}
}

#endif

