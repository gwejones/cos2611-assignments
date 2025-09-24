// COS2611 Final Project
// Full name: Garreth William Emile Jones
// Student Number: 50052578

#include <climits>
#include <cstdlib>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;

// --- Section: Data Initialization ---

const string intersectionData = R"(
id,name,lat,lon,population
1,Riverbend,34.0522,-118.2437,1200
2,Oakridge,34.0590,-118.2490,860
3,Pinecrest,34.0655,-118.2550,940
4,Maplewood,34.0720,-118.2605,1020
5,Brookvale,34.0785,-118.2680,780
6,Crestview,34.0860,-118.2765,1100
7,Sunnyside,34.0930,-118.2830,670
8,Willowgate,34.1005,-118.2910,520
9,Fairhaven,34.1070,-118.2980,1500
10,Lakeview,34.1135,-118.3060,1320
11,Greenwood,34.1200,-118.3130,980
12,Hillcrest,34.1265,-118.3200,860
13,Crestline,34.1330,-118.3270,740
14,Redrock,34.1395,-118.3340,690
15,Silverpond,34.1460,-118.3410,820
16,Northpoint,34.1525,-118.3480,900
17,Eastport,34.1590,-118.3550,760
18,Westhaven,34.1655,-118.3620,640
19,Southgate,34.1720,-118.3690,700
20,Meadowbank,34.1785,-118.3760,860
21,Brightwood,34.1850,-118.3830,910
22,Amberfield,34.1915,-118.3900,730
23,Goldendale,34.1980,-118.3970,680
24,Stonebridge,34.2045,-118.4040,720
25,Ironmere,34.2110,-118.4110,690
)";

const string roadData = R"(
from,to,road_type,distance_km,speed_limit_kmh,one_way
1,2,local_street,2.3,30,false
2,3,arterial,1.8,50,false
3,4,local_street,2.5,30,false
4,5,collector,3.1,40,false
5,6,arterial,2.9,50,false
6,7,local_street,3.3,40,false
7,8,collector,1.7,30,false
8,9,local_street,2.4,40,false
9,10,arterial,3.0,50,false
10,11,local_street,2.2,30,false
11,12,local_street,2.6,40,false
12,13,arterial,2.1,50,false
13,14,collector,3.4,40,false
14,15,arterial,2.7,50,false
15,16,local_street,1.9,30,false
16,17,arterial,2.3,50,false
17,18,local_street,2.8,40,false
18,19,local_street,3.2,40,false
19,20,collector,2.0,40,false
20,21,arterial,2.4,50,false
21,22,local_street,3.1,40,false
22,23,arterial,2.9,50,false
23,24,local_street,1.6,30,false
24,25,arterial,2.2,50,false
5,9,local_street,4.5,30,false
12,16,collector,4.0,40,false
18,22,collector,4.3,40,false
7,14,local_street,3.7,30,false
20,24,collector,3.5,40,false
)";

// --- Section: Define ADT ---

struct Edge {
  int head;
  float weight;
};

template <typename T> struct Node {
  T value;
  forward_list<Edge> edges;
};

struct Intersection {
  string name;
  float lat, lon;
  friend ostream &operator<<(ostream &os, const Intersection &i) {
    os << i.name;
    return os;
  }
};

template <typename T> class Graph {

public:
  void addNode(int key, T value) {
    forward_list<Edge> *edges = new forward_list<Edge>();
    adj[key] = {value, *edges};
  }

  void addEdge(int tail, const Edge edge) { adj[tail].edges.push_front(edge); }

  string nodeToString(int key) {
    ostringstream oss;
    oss << "[" << key << "] " << adj[key].value;
    return oss.str();
  }

  void displayAllNodes() {
    for (auto it = adj.begin(); it != adj.end(); ++it) {
      cout << nodeToString(it->first);
      for (Edge e : it->second.edges)
        cout << "\n\t-(" << fixed << setprecision(1) << e.weight << "km)-> "
             << nodeToString(e.head);
      cout << endl;
    }
  }

private:
  unordered_map<int, Node<T>> adj;
};

// --- Section: Helper Functions ---

void clearScreen() {
#ifdef _WIN32
  system("cls"); // For Windows
#else
  system("clear"); // For Unix-based systems
#endif
}

void readData(const string intersectionsCsv, const string roadsCsv,
              Graph<Intersection> &graph) {
  string line;

  // Add intersections to graph
  stringstream ids(intersectionsCsv);
  getline(ids, line); // ignore newline
  getline(ids, line); // ignore column headers
  while (getline(ids, line)) {
    Intersection intersection;
    stringstream ls(line);
    string inStr;
    getline(ls, inStr, ',');
    int id = stoi(inStr);
    getline(ls, inStr, ',');
    intersection.name = inStr;
    getline(ls, inStr, ',');
    intersection.lat = stof(inStr);
    getline(ls, inStr, ',');
    intersection.lon = stof(inStr);
    graph.addNode(id, intersection);
  }

  // Add edges to graph
  stringstream rds(roadsCsv);
  getline(rds, line); // ignore newline
  getline(rds, line); // ignore column headers
  while (getline(rds, line)) {
    Edge e;
    stringstream ls(line);
    string inStr;
    getline(ls, inStr, ','); // read road starting intersection
    int tail = stoi(inStr);
    getline(ls, inStr, ','); // read road ending intersection
    e.head = stoi(inStr);
    getline(ls, inStr, ','); // ignore road type
    getline(ls, inStr, ','); // read road distance
    e.weight = stof(inStr);
    graph.addEdge(tail, e);
  }
}

void showMap(Graph<Intersection> &graph) { graph.displayAllNodes(); }

int showMainMenu() {
  int selection;
  cout << "MENU:\n";
  cout << "1. Display routes\n";
  cout << "2. Add route\n";
  cout << "3. Exit\n";
  cout << "> ";
  if (!(cin >> selection))
    return INT_MAX;
  return selection;
}

// --- Section: Main Program ---

int main() {
  Graph<Intersection> distanceGraph;
  readData(intersectionData, roadData, distanceGraph);
  bool exit = false;
  while (!exit) {
    clearScreen();
    showMap(distanceGraph);
    switch (showMainMenu()) {
    case 1:
      break;
    case 2:
      break;
    default:
      exit = true;
    }
  }
  return 0;
}

// --- Section: Report ---

/*
 *
 * Sources:
 * 1. ChatGPT was used to generate the road network dataset, ie,
 * intersectionData & roadData. The prompt used was "Generate a dataset in CSV
 * format for a fictional town's road network, which can be used to populate a
 * graph. There should be at least 20 nodes."
 */
