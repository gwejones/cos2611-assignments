// COS2611 Assignment 3
// Full name: Garreth William Emile Jones
// Student Number: 50052578

#include <climits>
#include <forward_list> // singly-linked list implementation from STL
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

using namespace std;

// --- Section: Data Initialization ---

constexpr int NUM_CITIES = 7;
constexpr int COL_WIDTH = 18;

string cities[NUM_CITIES] = {"Cape Town", "Bloemfontein", "Johannesburg",
                             "Pretoria",  "Gaborone",     "Windhoek",
                             "Bulawayo"};

struct weightedEdge {
  int head;   // destination city
  int weight; // distance
};

// adjacency list, storing the links between cities and their corresponding
// distances. Each array element represents a source city where the index of
// the array corresponds to the city at the same index in cities[]. Each
// weighted edge contains the index of the city that edge points to, along with
// the distance from the source city
forward_list<weightedEdge> distances[NUM_CITIES] = {
    {{1, 1001}, {5, 1478}},
    {{0, 1001}, {2, 396}},
    {{1, 396}, {3, 59}, {4, 365}, {5, 1369}},
    {{2, 59}, {4, 362}, {5, 1366}, {6, 806}},
    {{2, 365}, {3, 362}, {5, 1121}, {6, 628}},
    {{0, 1478}, {2, 1369}, {3, 1366}, {4, 1121}},
    {{3, 806}, {4, 628}},
};

// --- Section: Define Weighted Graph Class ---
class weightedGraphType {

public:
  // Function to create the graph and the weight matrix from an adjacency list
  // of weighted edges
  static weightedGraphType
  fromWeightAdjacencyList(const forward_list<weightedEdge> *weightedEdges,
                          int size) {
    weightedGraphType wg(size);

    for (int i = 0; i < size; i++) {
      for (const weightedEdge &edge : weightedEdges[i]) {
        wg.graph[i].push_front(edge.head);
        wg.weights[i][edge.head] = edge.weight;
      }
    }

    return wg;
  }

  // Function to determine the weight of a shortest path
  // from vertex (source) to every other vertex
  // in the graph.
  // Postconditions:
  //  smallestWeight contains the weight of the shortest path from vertex to
  //  every other vertex in the graph is determined The shortest path is
  //  returned.
  forward_list<int> shortestPath(const int vertex, const int dest) {
    vector<int> predecessors(gSize, -1);
    for (int j = 0; j < gSize; j++)
      smallestWeight[j] = weights[vertex][j];
    bool *weightFound;
    weightFound = new bool[gSize];
    for (int j = 0; j < gSize; j++)
      weightFound[j] = false;
    weightFound[vertex] = true;
    smallestWeight[vertex] = 0;
    for (int i = 0; i < gSize - 1; i++) {
      int minWeight = INT_MAX;
      int v = -1; // closest to vertex, for which shortest path has not being
                  // determined
      for (int j = 0; j < gSize; j++)
        if (!weightFound[j])
          if (smallestWeight[j] < minWeight) {
            v = j;
            minWeight = smallestWeight[v];
          }
      if (v == -1)
        break; // Remaining vertices are unreachable
      weightFound[v] = true;
      for (int j = 0; j < gSize; j++)
        if (!weightFound[j])
          if (minWeight + weights[v][j] < smallestWeight[j]) {
            smallestWeight[j] = minWeight + weights[v][j];
            predecessors[j] = v; // Track predecessor
          }
    }
    delete[] weightFound;

    // Reconstruct path from dest to vertex
    forward_list<int> path;
    for (int at = dest; at != -1; at = predecessors[at])
      path.push_front(at);
    return path;
  }

  // Function to print the shortest weight from vertex
  // to the other vertex in the graph.
  // Postcondition: The weight of the shortest path from
  //  vertex to every other vertex in the
  //  graph is printed.
  void printShortestDistance(int vertex);

  // Performs breadth-first traversal from starting index
  // Postcondition: A linked list containing the traversal order is returned
  forward_list<int> breadthFirstTraversal(const int index) const {
    forward_list<int> bft;
    queue<int> queue;
    bool *visited;
    visited = new bool[gSize];
    for (int ind = 0; ind < gSize; ind++)
      visited[ind] = false; // initialize the array visited[] to false
    queue.push(index);
    visited[index] = true;
    bft.push_front(index);
    while (!queue.empty()) {
      int u = queue.front();
      queue.pop();
      for (auto it = graph[u].begin(); it != graph[u].end(); ++it) {
        int w = *it;
        if (!visited[w]) {
          queue.push(w);
          visited[w] = true;
          bft.push_front(w);
        }
      }
    }
    delete[] visited;
    return bft;
  }

  // Constructor
  // Postcondition: gSize = size;
  //  graph is an array of pointers to linked lists.
  //  weights is a two-dimensional array to
  //  store the weights of the edges, initialized to MAX_INT.
  //  smallestWeight is an array to store the
  //  smallest weight from source to vertices.
  weightedGraphType(int size = 0) {
    graph = new forward_list<int>[size];
    gSize = size;
    weights = new int *[size];
    for (int i = 0; i < size; i++) {
      weights[i] = new int[size];
      for (int j = 0; j < size; j++)
        weights[i][j] = INT_MAX;
    }

    smallestWeight = new int[size];
  }

  // Destructor
  // The storage occupied by the vertices and the arrays
  // weights and smallestWeight is deallocated.
  ~weightedGraphType() {
    for (int i = 0; i < gSize; i++) {
      graph[i].clear();
      delete[] weights[i];
    }
    delete[] weights;
    delete smallestWeight;
  }

  friend void printAdjacencyMatrix(const weightedGraphType &graph);

protected:
  int gSize;                // number of vertices
  forward_list<int> *graph; // adjacency list
  int **weights;            // weight adjacency matrix
  int *smallestWeight;      // store the smallest weight from source to vertices
};

// --- Section: Helper Functions ---

// Returns a string representing the city, given its index. This string will
// contain the city name followed by the index value in brackets.
string toStringFromIndex(int index) {
  ostringstream oss;
  oss << cities[index] << " (" << index << ')';
  return oss.str();
}

// Prints the adjacency matrix with the city names as column and row headers.
void printAdjacencyMatrix(const weightedGraphType &graph) {
  cout << endl << "Adjacency Matrix (Distances in km):" << endl;
  cout << setw(COL_WIDTH) << "";
  for (int i = 0; i < graph.gSize; i++)
    cout << setw(COL_WIDTH) << toStringFromIndex(i);
  cout << endl;
  for (int i = 0; i < graph.gSize; i++) {
    cout << setw(COL_WIDTH) << toStringFromIndex(i);
    for (int j = 0; j < graph.gSize; j++)
      cout << setw(COL_WIDTH)
           << (graph.weights[i][j] == INT_MAX ? "x"
                                              : to_string(graph.weights[i][j]));
    cout << endl;
  }
}

// Prints the list of reachable cities as a path, given a list of their indices
// in BFS order.
void printReachableCities(forward_list<int> reachableCities) {
  reachableCities.reverse();
  cout << "\nCities reachable from "
       << toStringFromIndex(reachableCities.front())
       << " in breadth-first traversal order:" << endl;
  cout << toStringFromIndex(reachableCities.front());
  reachableCities.pop_front();
  while (!reachableCities.empty()) {
    cout << " -> " << toStringFromIndex(reachableCities.front());
    reachableCities.pop_front();
  }
  cout << endl << endl;
}

// --- Section: Main Function ---
int main() {
  weightedGraphType graph =
      weightedGraphType::fromWeightAdjacencyList(distances, NUM_CITIES);

  int startIndex = 0;
  int destIndex;
  while (startIndex >= 0) {
    // display adjacency matrix to user
    printAdjacencyMatrix(graph);

    // get desired starting city from user
    cout << endl
         << "Select index of starting city, or negative number to exit: ";
    cin >> startIndex;
    if (startIndex < 0)
      break;

    // show user possible destination cities
    forward_list<int> bft = graph.breadthFirstTraversal(startIndex);
    printReachableCities(bft);

    // get desired destination city from user
    cout << "Select index of destination city: ";
    cin >> destIndex;

    // display shortest path to destination city
    forward_list<int> path = graph.shortestPath(startIndex, destIndex);
  }

  return 0;
}

// --- Section: Reflections ---

/*
 * No generative AI tools were used for this assignment.
 *
 * The following sources were consulted:
 *  1. Malik, D.S. (2018). C++ Programming: Program Design Including Data
 * Structures. Cengage Learning.
 *  2. https://en.cppreference.com/w/cpp/container/forward_list.html
 */
