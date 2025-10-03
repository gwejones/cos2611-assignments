/**
 * @file 50052578_FP.cpp
 * @brief Smart City Route Management System
 *
 * Author: Garreth William Emile Jones
 * Student Number: 50052578
 *
 * This program implements a route management system for a fictional smart city,
 * allowing users to visualize intersections and roads, compute shortest routes,
 * and manage custom routes interactively.
 *
 * Features:
 * - Loads intersection and road data from embedded CSV strings.
 * - Represents the city road network as a weighted directed graph.
 * - Supports breadth-first traversal and Dijkstra's shortest path algorithm.
 * - Displays a text-based map of the city with labeled intersections.
 * - Allows users to list all roads, view and manage custom routes.
 *
 * Main Components:
 * - Intersection: Represents a city intersection with ID, name, latitude, and
 * longitude.
 * - Edge: Represents a road segment between intersections, with direction and
 * length.
 * - Graph<T>: Generic graph class supporting node/edge management and
 * algorithms.
 * - MapLabel: Used for rendering intersection labels on the map.
 *
 * User Interface:
 * - Interactive menu for listing roads, routes, adding and deleting routes.
 * - Visual map display in the console.
 *
 * Algorithms:
 * - Breadth-First Traversal: Finds all reachable intersections from a given
 * start.
 * - Dijkstra's Algorithm: Computes the shortest path between intersections.
 *
 * Data Structures:
 * - Uses STL containers: vector, set, forward_list, unordered_map,
 * priority_queue.
 *
 * Sources:
 * - Road network dataset generated using ChatGPT.
 * - Reference implementation of Dijkstra's algorithm from
 * https://www.geeksforgeeks.org/
 */

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

#pragma region Data Initialization

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

#pragma endregion

#pragma region ADT Definitions

// Models a directed, weighted edge in a graph
struct Edge {
  int tail;
  int head;
  float weight;
  // Overload the less-than operator, implemented to enable
  // comparison between two Edge objects, which is essential
  // for storing edges in ordered containers like std::set
  bool operator<(const Edge &other) const {
    if (tail != other.tail) {
      return tail < other.tail;
    }
    return head < other.head;
  }
};

// Generic template, which is a fundamental building block
// for representing nodes in a graph data structure
template <typename T> struct Node {
  T value;
  forward_list<Edge> edges;
};

// Structure to support algorithms that require tracking the
// shortest distance to nodes in a graph, such as Dijkstra's algorithm
struct MinDistance {
  float distance;
  int node;
  // Overloaded less-than operator. This operator is implemented
  // so that when MinDistance objects are stored in a priority_queue,
  // the queue will prioritize objects with smaller distance values
  bool operator<(const MinDistance &other) const {
    return distance > other.distance; // lower distance means higher priority
  }
};

// Represents a label associated with a specific column, for use in the
// map visualization.
struct MapLabel {
  int col;
  string label;
  // Overloaded less-than operator. This operator is implemented so that
  // when MapLabel objects are stored in a priority queue, those with
  // lower column values are treated as having higher priority and printed
  // first.
  bool operator<(const MapLabel &other) const {
    return col > other.col; // lower col value means higher priority
  }
};

/**
 * @brief A generic Graph class supporting basic graph operations and
 * algorithms.
 *
 * @tparam T The type of value stored in each node.
 *
 * This class provides methods to add nodes and edges, retrieve nodes, perform
 * breadth-first traversal, and compute the single-source shortest path using
 * Dijkstra's algorithm.
 *
 * Public Methods:
 * - Node<T> getNode(int key) const:
 *      Retrieves the node associated with the given key.
 *
 * - vector<Node<T>> getAllNodes() const:
 *      Returns a vector containing all nodes in the graph.
 *
 * - void addNode(int key, const T value):
 *      Adds a node with the specified key and value to the graph.
 *
 * - void addEdge(const Edge edge):
 *      Adds an edge to the graph.
 *
 * - forward_list<T> breadthFirstTraversal(int sourceNodeKey):
 *      Performs a breadth-first traversal starting from the specified node key.
 *      Returns a forward_list of node values in the order they were visited.
 *
 * - forward_list<int> computeSSSP(int sourceNodeKey, int destNodeKey):
 *      Computes the shortest path from the source node to the destination node
 * using Dijkstra's algorithm. Returns a forward_list of node keys representing
 * the shortest path.
 *
 * Private Members:
 * - unordered_map<int, Node<T>> adj:
 *      Adjacency list representation of the graph, mapping node keys to Node
 * objects.
 */
template <typename T> class Graph {
public:
  /**
   * @brief Retrieves the node associated with the given key.
   *
   * @param key The key identifying the node to retrieve.
   * @return Node<T> The node corresponding to the specified key.
   * @throws std::out_of_range if the key does not exist in the adjacency map.
   */
  Node<T> getNode(int key) const { return adj.at(key); }

  /**
   * @brief Retrieves all nodes in the graph.
   *
   * Iterates through the adjacency map and collects all nodes into a vector.
   *
   * @return A vector containing all nodes of type Node<T> present in the graph.
   */
  vector<Node<T>> getAllNodes() const {
    vector<Node<T>> nodes;
    for (auto it = adj.begin(); it != adj.end(); ++it) {
      nodes.push_back(it->second);
    }
    return nodes;
  }

  /**
   * @brief Adds a new node to the adjacency list with the specified key and
   * value.
   *
   * This function creates a new node in the graph, initializing its associated
   * edge list. The node is identified by the given key and stores the provided
   * value.
   *
   * @param key The unique identifier for the node to be added.
   * @param value The value to associate with the new node.
   */
  void addNode(int key, const T value) {
    forward_list<Edge> *edges = new forward_list<Edge>();
    adj[key] = {value, *edges};
  }

  /**
   * @brief Adds an edge to the adjacency list of the specified tail node.
   *
   * This function inserts the given edge at the front of the edge list
   * for the node identified by edge.tail.
   *
   * @param edge The Edge object to be added to the graph.
   */
  void addEdge(const Edge edge) { adj[edge.tail].edges.push_front(edge); }

  /**
   * @brief Performs a breadth-first traversal (BFS) of the graph starting from
   * the specified source node.
   *
   * This function visits all reachable nodes from the given source node in BFS
   * order. It returns a forward_list containing the values of the visited nodes
   * in the order they were traversed.
   *
   * @tparam T The type of the node value stored in the graph.
   * @param sourceNodeKey The key (identifier) of the node from which to start
   * the BFS traversal.
   * @return std::forward_list<T> A list of node values visited during BFS, in
   * traversal order.
   *
   * @note The function prints debug information to standard output during
   * traversal.
   */
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

  /**
   * @brief Computes the Single Source Shortest Path (SSSP) between two nodes
   * using Dijkstra's algorithm.
   *
   * This function calculates the shortest path from the source node to the
   * destination node in a weighted directed graph. It uses Dijkstra's algorithm
   * with a min-heap priority queue to efficiently find the minimum distances.
   * The function also reconstructs the shortest path from the source to the
   * destination node.
   *
   * @param sourceNodeKey The key (identifier) of the source node.
   * @param destNodeKey The key (identifier) of the destination node.
   * @return forward_list<int> A forward list containing the node keys
   * representing the shortest path from the source node to the destination node
   * (inclusive).
   *
   * @note The function prints debug information about the algorithm's progress
   * and the computed shortest path.
   * @note Assumes that the adjacency list 'adj' and the Edge structure are
   * properly defined and accessible.
   * @throws std::out_of_range If there is no path from source to destination
   * (i.e., pred.at() fails).
   */
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
  /**
   * @brief Adjacency list representation of the graph.
   *
   * Stores nodes in an unordered_map where the key is the node identifier (int)
   * and the value is the corresponding Node<T> object. This structure allows
   * efficient lookup, insertion, and deletion of nodes in the graph.
   */
  unordered_map<int, Node<T>> adj;
};

// Domain-specific data structure for modelling the intersections in the city's
// road network. Stores both identifying information and geographic location.
struct Intersection {
  int id;         // uniquely identifies the intersection
  string name;    // the intersection’s name
  float lat, lon; // latitude and longitude coordinates
  //  overload the output stream operator, which enables easy
  //  printing of Intersection objects to output streams
  friend ostream &operator<<(ostream &os, const Intersection &i) {
    os << "[" << i.id << "] " << i.name;
    return os;
  }
};

#pragma endregion

#pragma region Helper Functions

/**
 * @brief Clears the console screen.
 *
 * This function prints a newline and then clears the console screen using the
 * appropriate system command depending on the operating system. On Windows, it
 * uses "cls", and on Unix-based systems, it uses "clear".
 */
void clearScreen() {
  cout << endl;
#ifdef _WIN32
  system("cls"); // For Windows
#else
  system("clear"); // For Unix-based systems
#endif
}

/**
 * @brief Pauses program execution until the user presses the Enter key.
 *
 * Displays a prompt message to the user and waits for the user to press Enter.
 * Useful for creating a pause, allowing the user to read output before
 * continuing.
 */
void pressEnterToContinue() {
  cout << endl << "Press <Enter> to continue...";
  cin.ignore();
  cin.get();
}

/**
 * @brief Reads intersection and road data from CSV strings and populates a
 * graph.
 *
 * This function parses two CSV-formatted strings containing intersection and
 * road data. It adds nodes to the graph for each intersection and edges for
 * each road. If a road is not one-way, a reciprocal edge is also added to
 * represent bidirectional travel.
 *
 * @param intersectionsCsv CSV string containing intersection data.
 *        Expected columns: id, name, latitude, longitude.
 * @param roadsCsv CSV string containing road data.
 *        Expected columns: tail, head, length, type, is_one_way.
 * @param graph Reference to a Graph<Intersection> object to be populated.
 */
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

/**
 * @brief Displays a visual representation of a map based on the given graph of
 * intersections.
 *
 * This function prints a map to the standard output, where each intersection
 * from the graph is mapped to a position on a grid according to its latitude
 * and longitude. The map includes borders and labels for each intersection,
 * centered at their respective positions.
 *
 * @param graph The graph containing Intersection nodes to be displayed on the
 * map.
 *
 * The function:
 * - Extracts all intersections from the graph.
 * - Determines the latitude and longitude boundaries to scale the map.
 * - Maps each intersection to a row and column on the grid.
 * - Handles label placement and border drawing.
 * - Outputs the formatted map to the console.
 */
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
    col -= oss.str().length() / 2;
    // add map label to specific row, in order of ascending column number
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

/**
 * @brief Displays the program title centered according to the maximum column
 * width.
 *
 * Uses the value of MAX_COLS and the length of PROGRAM_TITLE to calculate the
 * appropriate padding for centering the title in the output.
 */
void displayTitle() {
  cout << setw((MAX_COLS + PROGRAM_TITLE.length()) / 2) << PROGRAM_TITLE
       << endl;
}

/**
 * @brief Lists all roads in the given graph of intersections.
 *
 * Iterates through all nodes (intersections) in the graph and prints each
 * intersection, followed by its outgoing edges (roads) with their respective
 * weights (distance in km) and the destination intersection.
 *
 * @param graph The graph containing intersections and roads to be listed.
 */
void listRoads(const Graph<Intersection> &graph) {
  cout << "All Roads:\n";
  vector<Node<Intersection>> allNodes = graph.getAllNodes();
  for (auto n : allNodes) {
    cout << n.value << endl;
    for (auto e : n.edges)
      cout << "\t-(" << e.weight << "km)-> " << graph.getNode(e.head).value
           << endl;
  }
}

/**
 * @brief Lists all routes with their lengths and paths.
 *
 * Iterates through a set of routes, where each route is represented as a
 * forward_list of intersection keys. For each route, calculates the total
 * length by summing the weights of the edges between consecutive intersections.
 * Outputs the route number, total length, and the path taken (with edge
 * weights) to the standard output.
 *
 * @param routes A set of routes, each represented as a forward_list of
 * intersection keys.
 * @param graph The graph containing intersections and edges used to retrieve
 * node and edge information.
 */
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

/**
 * @brief Presents a menu to add a new route between intersections in a graph.
 *
 * This function prompts the user to select a starting intersection and a
 * destination intersection. It displays all reachable destinations from the
 * selected starting intersection, validates user input, and computes the
 * shortest path (route) between the two intersections using the graph's SSSP
 * algorithm. The user can then choose to add the computed route to the set of
 * existing routes.
 *
 * @param existingRoutes Reference to a set of routes (each route is a
 * forward_list of intersection IDs) to which new routes may be added.
 * @param graph Reference to the Graph object containing Intersection nodes and
 * connectivity information.
 */
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

/**
 * @brief Displays a menu to delete a route from the set of existing routes.
 *
 * This function lists all existing routes, prompts the user to select a route
 * to delete, and removes the selected route from the set. The function ensures
 * the selection is valid before performing the deletion.
 *
 * @param existingRoutes Reference to a set containing forward_list<int>
 * representing routes.
 * @param graph The graph object containing Intersection nodes, used for
 * displaying routes.
 */
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

/**
 * @brief Displays a menu to the user and retrieves their selection.
 *
 * Presents a list of options to the user via standard output and reads the
 * user's selection from standard input. If the input is invalid (not an
 * integer), returns INT_MAX.
 *
 * @return int The user's menu selection (1-5), or INT_MAX if input is invalid.
 */
int getMenuSelection() {
  int selection;
  cout << "Menu:\n";
  cout << "\t1. List roads\n";
  cout << "\t2. List routes\n";
  cout << "\t3. Add route\n";
  cout << "\t4. Delete route\n";
  cout << "\t5. Exit\n";
  cout << "> ";
  if (!(cin >> selection))
    return INT_MAX;
  return selection;
}

#pragma endregion

#pragma region Main Program

/**
 * @brief Entry point of the program.
 *
 * Initializes the distance graph and loads intersection and road data.
 * Manages a set of user-defined routes using a menu-driven interface.
 * The menu allows users to:
 *   1. List all roads in the graph.
 *   2. List all saved routes.
 *   3. Add a new route.
 *   4. Delete an existing route.
 * The program continues to display the menu until the user chooses to exit.
 *
 * @return int Returns 0 upon successful completion.
 */
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
    case 1: // list roads
      listRoads(distanceGraph);
      pressEnterToContinue();
      break;
    case 2: // list routes
      listRoutes(routes, distanceGraph);
      pressEnterToContinue();
      break;
    case 3: // add route
      addRouteMenu(routes, distanceGraph);
      pressEnterToContinue();
      break;
    case 4: // delete route
      deleteRouteMenu(routes, distanceGraph);
      pressEnterToContinue();
      break;
    default:
      exit = true;
    }
  }
  return 0;
}

#pragma endregion

#pragma region Report

/*
Problem Analysis:

1. The program implements a Smart City Route Management System for a fictional
city. Its main goal is to help users visualize the city's road network, compute
optimal routes, and manage custom routes interactively.
2. Key Features
 - Data Loading: Loads intersection and road data from embedded CSV strings.
 - Graph Representation: Models the city as a weighted, directed graph where
intersections are nodes and roads are edges.
 - User Interface: Provides an interactive menu for users to list roads,
view/manage routes, and display a text-based map of the city.
 - Custom Route Management: Users can add and delete custom routes.

Chosen Solution:

1. Graph Structure: Efficient for representing and querying road networks.
2. Set of Lists for Routes: Ensures uniqueness and efficient management of
user-defined routes.
3. Menu-Driven UI: Simplifies user interaction and makes the program accessible.
4. Separation of Concerns: Functions are modular, handling specific tasks (data
loading, display, route management).

Data Structures Used:

1. Graph<Intersection>
  - Represents the road network, with intersections as nodes and roads as edges.
  - Chosen because graphs efficiently model networks and support traversal and
pathfinding.

2. unordered_map<int, Node<T>> adj
  - Stores nodes keyed by intersection ID.
  - Provides fast lookup, insertion, and deletion via hash table.

3. set<forward_list<int>> routes
  - Holds user-defined routes as ordered lists of intersection IDs.
  - forward_list<int> is used for efficient sequence manipulation.
  - set ensures route uniqueness and fast operations.

Algorithms Used:

1. Breadth-First Traversal - Finds all reachable intersections from a given
start.
2. Dijkstra's Algorithm - Computes the shortest path between intersections.

Sources Consulted:

1. ChatGPT was used to generate the road network dataset.
The prompt used was "Generate a dataset in CSV format for a
fictional town's road network which can be used to populate
a graph. There should be at least 10 nodes."
2. A reference implementation of Disjkstra's algorithm was
obtained from
https://www.geeksforgeeks.org/dsa/dijkstras-shortest-path-algorithm-greedy-algo-7/
and was re-implemented in heavily-modifed form.
*/

#pragma endregion
