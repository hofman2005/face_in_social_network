#include "classifier/train_test_on_album.h"

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

    // Multi candidate identification
    fn::PhotoResult& res = it->GetPhotoRes();
    classifier->Identify(image, &res);

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

  return 0;
}

}

