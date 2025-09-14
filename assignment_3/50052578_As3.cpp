// COS2611 Assignment 3
// Full name: Garreth William Emile Jones
// Student Number: 50052578

#include <climits>
#include <forward_list> // singly-linked list implementation from STL
#include <iomanip>
#include <iostream>
#include <ostream>
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
  // from vertex, that is, source, to every other vertex
  // in the graph.
  // Postcondition: The weight of the shortest path from
  //  vertex to every other vertex in the
  //  graph is determined.
  void shortestPath(int vertex);

  // Function to print the shortest weight from vertex
  // to the other vertex in the graph.
  // Postcondition: The weight of the shortest path from
  //  vertex to every other vertex in the
  //  graph is printed.
  void printShortestDistance(int vertex);

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
  cout << "Adjacency Matrix (Distances in km):" << endl;
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

// --- Section: Main Function ---
int main() {
  weightedGraphType graph =
      weightedGraphType::fromWeightAdjacencyList(distances, NUM_CITIES);

  printAdjacencyMatrix(graph);

  cout << endl << "Select index of starting city: ";
  int startIndex;
  cin >> startIndex;

  return 0;
}

// --- Section: Reflections ---

/*
 * No generative AI tools were used for this assignment.
 * The follwing sources were consulted:
 * 1. Malik, D.S. (2018). C++ Programming: Program Design Including Data
 * Structures. Cengage Learning.
 *
 */
