#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "dataset/social_graph.h"

int main(int argc, char ** argv) {
  // namespace sn = SocialNetwork;
  using namespace std;
  using namespace SocialNetwork;

  if (argc != 3) {
    cout << "Usage: %prog INPUT_ALBUM OUTPUT_PREFIX" << endl;
    return 0;
  }

  string album_filename(argv[1]);
  AlbumMap album_map;
  ReadAlbumMapFromFile(album_filename, &album_map);

  vector<int> hist_self, hist_friends;
  for (AlbumMap::const_iterator it = album_map.begin();
      it != album_map.end();
      ++it) {
    const string& current_id = it->first;
    int count_self=0, count_friends=0;
    for (Album::const_iterator itt = it->second.begin();
        itt != it->second.end();
        ++itt) {
      if (current_id == itt->GetTrueId()) {
        ++count_self;
      } else {
        ++count_friends;
      }
    }

    if (count_self >= hist_self.size())
      hist_self.resize(count_self+1);
    ++ hist_self[count_self];

    if (count_friends >= hist_friends.size())
      hist_friends.resize(count_friends+1);
    ++ hist_friends[count_friends];
  }

  string output_prefix(argv[2]);
  string self_record = output_prefix + "self.dat";
  ofstream self(self_record.c_str());
  for (int i=0; i<hist_self.size(); ++i)
    self << i << " " << hist_self[i] << endl;
  
  string friends_record = output_prefix + "friends.dat";
  ofstream friends(friends_record.c_str());
  for (int i=0; i<hist_friends.size(); ++i)
    friends << i << " " << hist_friends[i] << endl;
}
