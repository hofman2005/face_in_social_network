#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <deque>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

string getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if(itr!=end && ++itr!=end)
        return string(*itr);
    return string("");
}

int BuildGraph(const string input, 
               map< int, vector<int> > * graph) {
  ifstream in(input.c_str());
  string content;
  while( getline(in, content) ) {
    if (content[0] == '#') 
      continue;
    istringstream iss_content(content);
    int id_1, id_2;
    iss_content >> id_1 >> id_2;
    (*graph)[id_1].push_back(id_2); 
    (*graph)[id_2].push_back(id_1);
  }
}

void ExtractMethod_1(map< int, vector<int> >& graph,
                     const string input,
                     const string output,
                     const int id_num,
                     const int id_start) {
  // Main extraction
  int count = 0;
  deque<int> visit_queue(1, id_start);
  set<int> visited;
  while (count < id_num && visit_queue.size() > 0 ) {
    int current = visit_queue.front();
    visit_queue.pop_front();
    if (visited.count(current) == 1)
      continue;
    visited.insert(current);;
    visit_queue.insert(visit_queue.end()-1, graph[current].begin(), graph[current].end());
    ++count;
  }
  cout << "count = " << count << endl;

  // Write the extracted graph
  ofstream out(output.c_str());
  out << "graph G {" << endl;
  out << "node [shape=point color=red];" << endl;
  ifstream in(input.c_str());
  string content;
  while( getline(in, content) ) {
    if (content[0] == '#') 
      continue;
    istringstream iss_content(content);
    int id_1, id_2;
    iss_content >> id_1 >> id_2;
    if (visited.count(id_1)==1 && visited.count(id_2)==1)
      out << id_1 << "--" << id_2 << ";" << endl;
  }
  out << "}" << endl;
}

void ExtractMethod_2(map< int, vector<int> >& graph,
                     const string output,
                     const int id_num,
                     const int id_start) {
  // Main extraction
  int count = 0;
  deque<int> visit_queue(1, id_start);
  set<int> visited;
  while (count < id_num && visit_queue.size() > 0 ) {
    int current = visit_queue.front();
    visit_queue.pop_front();
    if (visited.count(current) == 1)
      continue;
    visited.insert(current);;
    visit_queue.insert(visit_queue.end()-1, graph[current].begin(), graph[current].end());
    ++count;
  }
  cout << "count = " << count << endl;

  // Write the extracted graph
  ofstream out(output.c_str());
  out << "graph G {" << endl;
  out << "node [shape=point color=red];" << endl;
  visit_queue.clear();
  visit_queue.insert(visit_queue.end()-1, id_start);
  set<int> has_edge, visited_2;
  while( visit_queue.size() > 0 ) {
    int id_1 = visit_queue.front();
    visit_queue.pop_front();
    if (visited_2.count(id_1)==1)
      continue;
    has_edge.clear();
    for (vector<int>::iterator it = graph[id_1].begin();
         it != graph[id_1].end();
         ++it) {
      if (has_edge.count(*it)==1) 
        continue;
      if (visited_2.count(*it)==1)
        continue;
      has_edge.insert(*it);
      visit_queue.insert(visit_queue.end()-1, *it);
      if (visited.count(id_1)==1 && visited.count(*it)==1)
        out << id_1 << "--" << *it << ";" << endl;
    }
    visited_2.insert(id_1);
  }
  out << "}" << endl;
}
  
void ExtractMethod_3(map< int, vector<int> >& graph,
                     const string output,
                     const int id_num,
                     const int id_start) {
  // Main extraction
  int count = 0;
  deque<int> visit_queue(1, id_start);
  set<int> visited;
  set<int> has_edge;
  ofstream out(output.c_str());
  out << "graph G {" << endl;
  out << "node [shape=point color=red];" << endl;
  while (count < id_num && visit_queue.size() > 0) {
    int current = visit_queue.front();
    visit_queue.pop_front();
    if (visited.count(current)==1) 
      continue;
    has_edge.clear();
    for (vector<int>::const_iterator it = graph[current].begin();
         it != graph[current].end();
         ++it) {
      if (visited.count(*it) == 1 || has_edge.count(*it) == 1)
        continue;
      out << current << "--" << *it << ";" << endl;
      has_edge.insert(*it);
      visit_queue.insert(visit_queue.end()-1, *it);
    }
    ++count;
    visited.insert(current);
  }
  cout << "count = " << count << endl;
  out << "}" << endl;
}

void ExtractSubGraph(const string input,
                     const string output,
                     const int id_num,
                     int id_start) {
  // Build the graph
  map< int, vector<int> > graph;
  BuildGraph(input, &graph);

  // Select start
  srand( time(NULL) );
  if (id_start == 0) {
    unsigned int j = rand() << 16 + rand();
    map< int, vector<int> >::iterator it = graph.begin();
    advance(it, j % graph.size());
    id_start = it->first;
  }

  // Choose different method to generate the graph.
  // method_2 is for generating graph with given number of nodes.
  // method_3 is for generating graph for show.
  ExtractMethod_3(graph, output, id_num, id_start);
}

int main(int argc, char ** argv) {
  if (argc==5) {
    string num(getCmdOption(argv, argv+argc, "-n"));
    string start(getCmdOption(argv, argv+argc, "-s"));
    string input(argv[3]);
    string output(argv[4]);
    int id_num = atoi(num.c_str());
    int id_start = atoi(start.c_str());
    ExtractSubGraph(input, output, id_num, id_start);
  } 
  else {
    cout << "Usage: %proc -n NUM [-s START] input output" << endl;
    return 0;
  }
}
