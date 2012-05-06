#include "dataset/social_graph.h"
#include "classifier/pca_classifier.h"

namespace FaceRecognition {
namespace sn = SocialNetwork;
namespace fn = FaceRecognition;

int TrainOnSingleAlbum(const std::string& image_prefix,
                       sn::Album* album,
                       BaseClassifier* classifier);

int TestOnSingleAlbum(const std::string& image_prefix, 
                      BaseClassifier* classifier,
                      sn::Album* album);
}

