// COS2611 Final Project
// Full name: Garreth William Emile Jones
// Student Number: 50052578

#include <climits>
#include <cstdlib>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// --- Section: Data Initialization ---

const string PROGRAM_TITLE = "Smart City Route Management System";
const int MAX_ROWS = 40;
const int MAX_COLS = 80;

const string intersectionData = R"(
id,name,latitude,longitude,district
1,Central Park,40.7128,-74.0060,Downtown
2,Oak Street,40.7135,-74.0050,Old Town
3,Riverfront,40.7145,-74.0075 East Bank
4,Hilltop,40.7155,-74.0085,Northside
5,Maple Ave,40.7162,-74.0048,West End
6,Pine Junction,40.7170,-74.0035,Northside
7,Crestview,40.7182,-74.0020,Uptown
8,Riverside,40.7190,-74.0055,East Bank
9,Brookside,40.7201,-74.0090,Southside
10,Canyon Road,40.7210,-74.0070,Valley
11,Lighthouse Point,40.7225,-74.0065,Waterfront
12,Grammar Square,40.7130,-74.0080,Old Town
)";

const string roadData = R"(
from,to,length_km,road_type,one_way
1,2,0.8,street,false
2,3,0.5,street,false
3,4,0.7,avenue,true
4,5,1.0,street,false
5,6,0.6,avenue,false
6,7,0.9,street,true
7,8,0.5,road,false
8,9,0.8,street,false
9,10,1.1,avenue,false
10,11,0.9,road,true
12,1,0.7,avenue,false
2,5,1.1,road,false
4,8,1.2,street,false
)";

// --- Section: Define ADT ---

struct Edge {
  int tail;
  int head;
  float weight;
  // Overload the less-than operator for the purpose of determining the
  // uniqueness of routes. Required for set operations.
  bool operator<(const Edge &other) const {
    if (tail != other.tail) {
      return tail < other.tail;
    }
    return head < other.head;
  }
};

template <typename T> struct Node {
  T value;
  forward_list<Edge> edges;
};

struct Intersection {
  int id;
  string name;
  float lat, lon;
  friend ostream &operator<<(ostream &os, const Intersection &i) {
    os << "[" << i.id << "] " << i.name;
    return os;
  }
};

template <typename T> class Graph {
public:
  T getNodeValue(int key) const {
    T value = adj.at(key).value;
    return value;
  }

  vector<T> getAllNodeValues() const {
    vector<T> nodeValues;
    for (auto it = adj.begin(); it != adj.end(); ++it) {
      nodeValues.push_back(it->second.value);
    }
    return nodeValues;
  }

  void addNode(int key, const T value) {
    forward_list<Edge> *edges = new forward_list<Edge>();
    adj[key] = {value, *edges};
  }

  void addEdge(const Edge edge) { adj[edge.tail].edges.push_front(edge); }

  string nodeToString(int key) {
    ostringstream oss;
    oss << adj[key].value;
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

void pressEnterToContinue() {
  cout << "Press <Enter> to continue..." << endl;
  cin.ignore();
  cin.get();
}

void readData(const string intersectionsCsv, const string roadsCsv,
              Graph<Intersection> &graph) {

  // Add intersections to graph
  string line;
  stringstream ids(intersectionsCsv);
  getline(ids, line); // ignore newline
  getline(ids, line); // ignore column headers
  while (getline(ids, line)) {
    Intersection intersection;
    stringstream ls(line);
    string inStr;
    getline(ls, inStr, ','); // read intersection id
    int id = stoi(inStr);
    intersection.id = id;
    getline(ls, inStr, ','); // read instersectio name
    intersection.name = inStr;
    getline(ls, inStr, ','); // read intersection lattitude
    intersection.lat = stof(inStr);
    getline(ls, inStr, ','); // read intersection longitude
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
    e.tail = stoi(inStr);
    getline(ls, inStr, ','); // read road ending intersection
    e.head = stoi(inStr);
    getline(ls, inStr, ','); // read road length
    e.weight = stof(inStr);
    getline(ls, inStr, ','); // ignore road type
    graph.addEdge(e);
    getline(ls, inStr); // read road is one-way
    if (inStr != "true") {
      // if the road is not one-way, add a reciprocal edge to represent the road
      // in the other direction
      Edge eRec = {e.head, e.tail, e.weight};
      graph.addEdge(eRec);
    }
  }
}

void showMap(const Graph<Intersection> &graph) {
  char **screenBuffer = new char *[MAX_ROWS];
  for (int i = 0; i < MAX_ROWS; i++)
    screenBuffer[i] = new char[MAX_COLS];

  // add intersections to screenbuffer
  for (int row = 0; row < MAX_ROWS; row++)
    for (int col = 0; col < MAX_COLS; col++)
      screenBuffer[row][col] = ' ';

  vector<Intersection> intersections = graph.getAllNodeValues();

  // get row & col boundaries of map
  float minLat = (float)INT_MAX;
  float maxLat = -(float)INT_MAX;
  float minLon = (float)INT_MAX;
  float maxLon = -(float)INT_MAX;
  for (Intersection i : intersections) {
    if (i.lat < minLat)
      minLat = i.lat;
    if (i.lat > maxLat)
      maxLat = i.lat;
    if (i.lon < minLon)
      minLon = i.lon;
    if (i.lon > maxLon)
      maxLon = i.lon;
  }

  // write each intersection name to screenbuffer
  for (Intersection i : intersections) {
    // map lattitude value to screen row
    int row =
        (MAX_ROWS - 1) - (i.lat - minLat) / (maxLat - minLat) * (MAX_ROWS - 1);
    // map longitude value to screen row
    int col = (i.lon - minLon) / (maxLon - minLon) * (MAX_COLS - 1);
    ostringstream oss;
    oss << i;
    for (char c : oss.str()) {
      screenBuffer[row][col] = c;
      if (++col == MAX_COLS)
        break;
    }
  }

  // print screenbuffer
  for (int row = 0; row < MAX_ROWS; row++) {
    for (int col = 0; col < MAX_COLS; col++)
      cout << screenBuffer[row][col];
    cout << endl;
  }

  // deallocate screenbuffer memory
  for (int i = 0; i < MAX_ROWS; ++i) {
    delete[] screenBuffer[i];
  }
  delete[] screenBuffer;
}

void listRoutes(const set<forward_list<Edge>> routes,
                const Graph<Intersection> &graph) {
  int routeNum = 1;
  cout << "Num\tLength\tRoute\n";
  for (auto it = routes.begin(); it != routes.end(); ++it) {
    float totalLength = 0;
    ostringstream oss;
    for (Edge e : *it) {
      totalLength += e.weight;
      oss << " -(" << setprecision(2) << e.weight << "km)-> "
          << graph.getNodeValue(e.head);
    }
    cout << routeNum++ << ".\t" << setprecision(2) << totalLength << "km\t"
         << graph.getNodeValue((*it).front().tail) << oss.str();
    cout << endl;
  }
}

int getMenuSelection() {
  int selection;
  cout << "Menu:\n";
  cout << "-----\n";
  cout << "1.\tList roads\n";
  cout << "2.\tList routes\n";
  cout << "3.\tAdd route\n";
  cout << "4.\tDelete route\n";
  cout << "5.\tExit\n";
  cout << "> ";
  if (!(cin >> selection))
    return INT_MAX;
  return selection;
}

// --- Section: Main Program ---

int main() {
  Graph<Intersection> distanceGraph;
  readData(intersectionData, roadData, distanceGraph);
  set<forward_list<Edge>> routes;

  // Edge e1 = {1, 2, 1.1};
  // Edge e2 = {2, 3, .7};
  // forward_list<Edge> route1 = {e1, e2};
  // routes.insert(route1);

  bool exit = false;
  while (!exit) {
    clearScreen();
    cout << setw((MAX_COLS + PROGRAM_TITLE.length()) / 2) << PROGRAM_TITLE
         << endl;
    cout << setw((MAX_COLS + PROGRAM_TITLE.length()) / 2)
         << string(PROGRAM_TITLE.length(), '-') << endl
         << endl;
    showMap(distanceGraph);
    switch (getMenuSelection()) {
    case 1:
      break;
    case 2:
      listRoutes(routes, distanceGraph);
      pressEnterToContinue();
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
 * 1. ChatGPT was used to generate the road network dataset.
 * The prompt used was "Generate a dataset in CSV format for a
 * fictional town's road network which can be used to populate
 * a graph. There should be at least 10 nodes."
 */
