#include <forward_list> // singly-linked list implementation from STL
#include <iostream>
#include <string>

using namespace std;

// --- Section: Data Initialization ---

constexpr int NUM_CITIES = 7;

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

// adjacency matrix with all the links between cities and their corresponding
// distances
int adjacencyMatrix[NUM_CITIES][NUM_CITIES];

int main() { return 0; }