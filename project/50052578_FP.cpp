// COS2611 Final Project
// Full name: Garreth William Emile Jones
// Student Number: 50052578

#include <climits>
#include <cstdlib>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <queue>
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

// generic data structure for storing the edges of a weighted graph
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

// generic data structure for storing the nodes in a weighted graph
template <typename T> struct Node {
  T value;
  forward_list<Edge> edges;
};

// generic data structure for keeping track of minimum distances to a given node
// in a priority queue
struct MinDistance {
  float distance;
  int node;
  // Overload operator< for comparison (defines priority)
  // Argument on the right side of operator< has higher priority
  bool operator<(const MinDistance &other) const {
    return distance > other.distance; // lower distance means higher priority
  }
};

// domain specific data structure for storing a label in a given row of a map
// these are stored in a priority queue so that objects with lower col
// values (higher priority) are rendered first, left to right
struct MapLabel {
  int col;
  string label;
  // overload operator< for comparison (defines priority)
  // argument on the right side of operator< has higher priority
  bool operator<(const MapLabel &other) const {
    return col > other.col; // lower col value means higher priority
  }
};

// generic ADT for storing and performing operations on weighted graph
template <typename T> class Graph {
public:
  Node<T> getNode(int key) const { return adj.at(key); }

  vector<Node<T>> getAllNodes() const {
    vector<Node<T>> nodes;
    for (auto it = adj.begin(); it != adj.end(); ++it) {
      nodes.push_back(it->second);
    }
    return nodes;
  }

  void addNode(int key, const T value) {
    forward_list<Edge> *edges = new forward_list<Edge>();
    adj[key] = {value, *edges};
  }

  void addEdge(const Edge edge) { adj[edge.tail].edges.push_front(edge); }

  forward_list<T> breadthFirstTraversal(int sourceNodeKey) {
    forward_list<T> traversalList;    // list of visited nodes, in BFS order
    unordered_map<int, bool> visited; // keeps track of visited nodes
    queue<int> q;                     // queue required for BFS
    int numNodesVisited = 0;          // keeps track of number of nodes visited

    visited[sourceNodeKey] = true;
    q.push(sourceNodeKey);
    cout << "[XAI] BFS traversal starting from node " << sourceNodeKey
         << "...\n";
    while (!q.empty()) {
      // Dequeue a node and visit it
      int currentNode = q.front();
      q.pop();
      cout << "[XAI] visiting node " << currentNode << "...\n";
      numNodesVisited++;
      traversalList.push_front(getNode(currentNode).value);
      // Get all adjacent nodes of the dequeued node. If an adjacent
      // node has not been visited, mark as visited and enqueue.
      for (Edge e : adj[currentNode].edges) {
        if (!visited[e.head]) {
          visited[e.head] = true;
          q.push(e.head);
        }
      }
    }
    cout << "[XAI] visited " << numNodesVisited << " nodes in total...\n";
    traversalList.reverse(); // ensure first element in linked list is first
                             // visited node
    return traversalList;
  }

  forward_list<int> computeSSSP(int sourceNodeKey, int destNodeKey) {
    cout << "[XAI] Using Dijkstra's algorithm to compute shortest path "
            "from node "
         << sourceNodeKey << " to node " << destNodeKey << "...\n";
    unordered_map<int, float>
        dist; // keep track of minimum distances to each node
    unordered_map<int, int>
        pred; // keep track of each node's predessor on the shortest path
    priority_queue<MinDistance> pq; // min-heap
    dist[sourceNodeKey] = 0;
    pq.push({0, sourceNodeKey}); // push source with distance 0

    while (!pq.empty()) {
      float d = pq.top().distance; // current shortest distance to 'u'
      int u = pq.top().node;       // current vertex 'u'
      pq.pop();

      // if we found a shorter path to 'u' already, skip this entry
      if (d > dist[u]) {
        continue;
      }

      // explore neighbors of 'u'
      for (const Edge e : adj[u].edges) {
        int v = e.head;
        float weight = e.weight;

        auto it = dist.find(v); // check if shortest distance exists
        // if 'v' does not have a shortest distance yet, or a shorter path to
        // 'v' is found through 'u', update shortest distance to 'v'
        if (it == dist.end() || dist[u] + weight < dist[v]) {
          float newDist = dist[u] + weight;
          cout << "[XAI] relaxing distance to node " << v << ", d=" << newDist
               << "...\n";
          dist[v] = newDist;
          pred[v] = u;
          pq.push(
              {dist[v], v}); // push updated distance and node to priority queue
        }
      }
    }

    // print shortest distances
    for (auto d : dist) {
      std::cout << "[XAI] shortest distance to node " << d.first << " is "
                << d.second << "...\n";
    }

    // recover the shortest path from predessesor map
    forward_list<int> shortestPath;
    int currentNodeKey = destNodeKey;
    // walk backward from dest to source
    while (currentNodeKey != sourceNodeKey) {
      shortestPath.push_front(currentNodeKey);
      currentNodeKey = pred.at(currentNodeKey);
    }

    // print shortest path
    cout << "[XAI] shortest path is " << sourceNodeKey;
    for (int &n : shortestPath) {
      cout << "->" << n;
    }
    cout << endl;

    shortestPath.push_front(sourceNodeKey); // add starting node at front
    return shortestPath;
  }

private:
  unordered_map<int, Node<T>> adj;
};

// domain-specific data structure for storing the intersections in the city's
// road network
struct Intersection {
  int id;
  string name;
  float lat, lon;
  friend ostream &operator<<(ostream &os, const Intersection &i) {
    os << "[" << i.id << "] " << i.name;
    return os;
  }
};

// --- Section: Helper Functions ---

void clearScreen() {
  cout << endl;
#ifdef _WIN32
  system("cls"); // For Windows
#else
  system("clear"); // For Unix-based systems
#endif
}

void pressEnterToContinue() {
  cout << endl << "Press <Enter> to continue...";
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
  vector<Intersection> intersections;
  for (Node<Intersection> n : graph.getAllNodes())
    intersections.push_back(n.value);

  // stores the labels for each row on the map
  vector<priority_queue<MapLabel, vector<MapLabel>>> rows(MAX_ROWS);

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

  // draw left and right borders around map
  for (int r = 0; r < MAX_ROWS; r++) {
    rows[r].push({0, "|"});
    rows[r].push({MAX_COLS - 1, "|"});
  }

  // write each intersection name to appropriate map row
  for (Intersection i : intersections) {
    // map latitude value to screen row
    int row =
        (MAX_ROWS - 1) - (i.lat - minLat) / (maxLat - minLat) * (MAX_ROWS - 1);
    // map longitude value to screen row
    int col = (i.lon - minLon) / (maxLon - minLon) * (MAX_COLS - 1);
    ostringstream oss;
    oss << i;
    // add map label to specific row, in order of ascending column number
    col -= oss.str().length() / 2;
    rows[row].push({col, oss.str()});
  }

  // display each row
  cout << '+' << string(MAX_COLS - 2, '-') << '+' << endl; // top border
  for (auto row : rows) {
    int col = 0;
    while (!row.empty() && col < MAX_COLS) {
      while (col++ < row.top().col)
        cout << ' ';
      cout << row.top().label;
      col += row.top().label.size() - 1;
      row.pop();
      if (!row.empty() && row.top().col < col)
        row.pop(); // remove a label if weve passed its col number
    }
    cout << endl;
  }
  cout << '+' << string(MAX_COLS - 2, '-') << '+' << endl; // bottom border
}

void displayTitle() {
  cout << setw((MAX_COLS + PROGRAM_TITLE.length()) / 2) << PROGRAM_TITLE
       << endl;
}

void listRoutes(const set<forward_list<int>> routes,
                const Graph<Intersection> &graph) {
  int routeNum = 1;
  cout << "Num\tLength\tRoute\n";
  for (auto it = routes.begin(); it != routes.end(); ++it) {
    forward_list<int> route = *it;
    float totalLength = 0;
    ostringstream oss;
    Node<Intersection> startingNode = graph.getNode(route.front());
    int currentKey = route.front();
    route.pop_front();
    while (!route.empty()) {
      Node<Intersection> currentNode = graph.getNode(currentKey);
      Node<Intersection> nextNode = graph.getNode(route.front());
      forward_list<Edge> edges = currentNode.edges;
      float weight = 0;
      for (Edge e : edges)
        if (e.head == route.front())
          weight = e.weight;
      totalLength += weight;
      oss << " -(" << setprecision(2) << weight << "km)-> " << nextNode.value;
      currentKey = route.front();
      route.pop_front();
    }
    cout << routeNum++ << ".\t" << setprecision(2) << totalLength << "km\t"
         << startingNode.value << oss.str();
    cout << endl;
  }
}

void addRouteMenu(set<forward_list<int>> &existingRoutes,
                  Graph<Intersection> &graph) {

  int startKey, destKey;
  // get starting intersection from user
  cout << "Starting intersection?\n> ";
  cin >> startKey;
  Intersection startIntersection;
  try {
    startIntersection = graph.getNode(startKey).value;
  } catch (const std::out_of_range &e) {
    cout << "Intersection with id=" << startKey << " does not exist.\n";
    return;
  }

  // get destination intersection from user
  forward_list<Intersection> reachableDestinations =
      graph.breadthFirstTraversal(startKey);
  reachableDestinations.pop_front(); // remove starting intersection as its not
                                     // a viable destination
  cout << "Destinations reachable from " << startIntersection << " are:\n";
  for (Intersection d : reachableDestinations)
    cout << d << endl;
  cout << "Destination intersection?\n> ";
  cin >> destKey;
  try {
    Intersection destIntersection = graph.getNode(destKey).value;
  } catch (const std::out_of_range &e) {
    cout << "Intersection with id=" << destKey << " does not exist.\n";
    return;
  }
  bool foundDest = false;
  for (Intersection d : reachableDestinations)
    if (d.id == destKey)
      foundDest = true;
  if (!foundDest) {
    cout << "Intersection with id=" << destKey << " is not reachable.\n";
    return;
  }

  forward_list<int> sr = graph.computeSSSP(startKey, destKey);
  set<forward_list<int>> newRoutes({sr});

  listRoutes(newRoutes, graph);
  int selection = 0;
  cout << "Route to add?\n> ";
  cin >> selection;
  if (selection > 0 && selection <= newRoutes.size()) {
    auto it = newRoutes.begin();
    advance(it, selection - 1);
    existingRoutes.insert(*it);
    cout << "Route added...\n";
  }
}

void deleteRouteMenu(set<forward_list<int>> &existingRoutes,
                     Graph<Intersection> graph) {
  listRoutes(existingRoutes, graph);
  int selection = 0;
  cout << "Route to delete?\n> ";
  cin >> selection;
  if (selection > 0 && selection <= existingRoutes.size()) {
    auto it = existingRoutes.begin();
    advance(it, selection - 1);
    existingRoutes.erase(*it);
    cout << "Route deleted...\n";
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
  set<forward_list<int>> routes;

  bool exit = false;
  while (!exit) {
    clearScreen();
    displayTitle();
    showMap(distanceGraph);
    switch (getMenuSelection()) {
    case 1:
      break;
    case 2:
      listRoutes(routes, distanceGraph);
      pressEnterToContinue();
      break;
    case 3:
      addRouteMenu(routes, distanceGraph);
      pressEnterToContinue();
      break;
    case 4:
      deleteRouteMenu(routes, distanceGraph);
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
