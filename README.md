# COS2611 Assignments

## Module Purpose

To show learners how abstract data types and data structures can be implemented and used in an object-oriented programming language. The module covers recursion, linked lists, dynamic memory allocation, binary trees, and graphs.

## Assignment 1: Public Transport Queue System

### Compile and Run

Compile from repository root:

```bash
g++ -std=c++17 assignment_1/50052578A1.cpp -o assignment_1/as1
```

Run:

```bash
./assignment_1/as1
```

Note: this program currently reads from `C:\data\taxiData.txt`.

### Objective

Using C++ and the linear data structures of queues and stacks, this project simulates a queue-based minibus taxi rank system—a common public transport method across Africa. The simulation will involve three types of taxis:

1. Short-distance (S)
2. Long-distance (L)
3. City routes (C)

Passengers arrive at the taxi rank and queue according to their destination. When a taxi becomes available, they board in an orderly manner based on their position in the queue. Each passenger requires some processing time to complete the onboarding process, which includes boarding the taxi and settling in. To test your implementation, download the taxiData.txt file, which contains passenger arrival data. Note that a different dataset will be used during marking.

Your implementation must incorporate linear data structures, specifically queues and stacks, to efficiently manage passenger boarding and taxi departures.

Important: The data file is located at:

Windows: C:\\data\\taxiData.txt
Mac/Linux: Update the path accordingly to match your system’s file structure.  However, for the purpose of marking, the path must be in the format as for Windows, suggestion add a try-catch to prevent the program from crashing.

Data Format:
Each row in taxiData.txt represents a time instance at the taxi rank and contains three comma-separated values:

`timecount`, `route` (S or L or C), `boarding_time`

Example:
1,S,3

At time instance 1, a passenger arrives for the Short-distance (S) taxi.
If no one is currently boarding, the passenger takes 3 time units to board.
If another passenger is already boarding, they must wait until the taxi is available.
Assumptions:

All taxis have a capacity of five (5) passengers.
A taxi will leave when full.
Components
Passenger Structure
Each passenger is represented as a structure containing:

Destination Type: Short-distance, Long-distance, or City route.
Processing Time: Time taken for the passenger to board and for the taxi to fill up.
Queue and Time Simulation
Three separate queues represent the different taxi routes.
The simulation runs for a specified duration.
At each time step:
A passenger may arrive and is assigned a queue based on their route.
If a taxi is available for that route, the next passenger boards.
The taxi departs once full.
Output Format
The simulation output should be displayed in a tabular format where:

Each row represents a time step.
Column Next displays the new passengers.
Columns S, L and C display the number of passengers in each queue.
Columns waiting Q S, waiting Q L, waiting Q C display the passengers in the waiting queue that has to wait for the passenger in front of him/her.
Columns Taxi Capacity for S, L and C indicating the current capacity for the taxis.

## Assignment 2: Sorting Algorithms

### Compile and Run

Compile from repository root:

```bash
g++ -std=c++17 assignment_2/50052578A2P1.cpp -o assignment_2/as2
```

Run:

```bash
./assignment_2/as2
```

### PURPOSE

This project aims to allow you, as a 2nd-year Computer Science student, the opportunity not only to implement a C++ solution but also to critically peer-review alternative solutions presented by your peers (other students).

### OUTCOME TO BE COVERED

Implementing sorting algorithms and analysing their time complexity for various input sizes is fundamental in algorithm analysis. 

Download from Additional Resources --> Assessment 2 the File--> COS2611_As2_P1_Skeleton .cpp file to get started.  

Refrain from using an AI tool to help you generate the code - you must understand the complexities of the different sorting algorithms and how to generate the respective reports and reflect on them.  We will use AI in a follow-up assessment to assist us in developing code.

### SPECIFICATIONS FOR THIS PROJECT

You are required to choose two sorting algorithms from a specified list: 

The first algorithm must be Bubble Sort, while the second algorithm can be either Merge Sort or Heap Sort.

1. Write a C++ program where you implement the two sorting algorithms you have chosen - ensure that they are part of the same program and can be called using methods, functions and/or procedures. 

2. Continue in this C++ program and generate random input arrays of different sizes, specifically 100, 1000, and 5000 elements.

3. Utilize the implemented TWO sorting algorithms to sort these randomly generated arrays. 

4. Generate a report to indicate:

- The input size (number of elements).
- The execution time for the two different sorting algorithms.
- The time complexity formulas for the two different sorting algorithms. 

## Assignment 3: Transport Connectivity in Southern Africa

### Compile and Run

Compile from repository root:

```bash
g++ -std=c++17 assignment_3/50052578_As3.cpp -o assignment_3/as3
```

Run:

```bash
./assignment_3/as3
```

Note: this program is interactive. Enter a negative start-city index to exit.

### Note on GenAI and XAI Usage

You are encouraged to use Generative AI tools (GenAI) to assist you in this project. However, you must apply Explainable AI (XAI) practices throughout your code by providing clear explanations and comments for each section to demonstrate your understanding. All prompts used with GenAI must be included at the end of your submission, along with a short reflection on what worked, what did not, and any changes you made to the AI-generated outputs. Remember, while AI can assist you, your explanations, understanding, and application of the concepts will be assessed.

### Background

Southern Africa’s development relies heavily on interconnected transport routes, including roads between cities and towns across countries such as South Africa, Namibia, Botswana, Zimbabwe, and Mozambique. Efficient transport planning requires understanding these connections, visualising them, and performing traversals to determine reachability and optimal routes.

### Project Brief

You are tasked to develop a Graph-based program in C++ that models a simplified transport network of Southern Africa. Your program should:

Represent cities/towns as vertices and roads as weighted edges (distances).
Allow input of connections using adjacency lists and display the adjacency matrix representation.
Implement Breadth-First Search (BFS) to show reachable cities from a selected starting city.
Implement Dijkstra’s algorithm to find the shortest distance and path between any two selected cities.
Display the graph structure clearly, labelling each city.
Follow Explainable AI (XAI) coding practices: comment thoroughly to explain each logical section to a non-programmer.

## Final Project: Smart City Route Management System

### Compile and Run

Compile from repository root:

```bash
g++ -std=c++17 project/50052578_FP.cpp -o project/final_project
```

Run:

```bash
./project/final_project
```

Note: this program is menu-driven. Select `5` to exit.

### Background

Cities require efficient transport routes to reduce travel times, congestion, and costs. The transport authority has requested a Route Management System that will:

Represent the city’s road network as a data structure.
Optimise routes between different areas.
Provide transparent, explainable output (XAI), showing why each route or decision was chosen.
You are tasked with designing and implementing this system in C++.

### Project Goals

Your system should:

Represent the city transport network (nodes = intersections, edges = routes).
Provide functions for adding, deleting, and updating routes.
Use appropriate data structures and algorithms (from the topics covered this year).
Implement a graph-based algorithm (e.g., Dijkstra) to find shortest or most efficient routes.
Produce XAI-friendly output, clearly explaining why a certain route or optimisation was selected.
Optionally, include AI-based predictions (e.g., identifying potential congestion areas).
Include code comments at the point of each explanation, indicating where the XAI output should appear.

### Topics You May Apply

You should decide which tools are most appropriate. Examples include:

Graphs – core representation of the city network.
Dijkstra’s Algorithm – shortest path computation.
Linked Lists / STL List – dynamic storage of route updates.
Sets / Multisets – unique vs. duplicate route storage.
Stacks / Queues / Deque – undo/redo of changes or batch route updates.
Sorting & Searching – rank routes by distance, time, or cost.
Binary Trees – store routes in hierarchical transport zones.
Vectors & Iterators – manipulate large collections of routes.
Functors / STL Algorithms – customise sorting or filtering.
Recursion – traversal or pathfinding.

### System Requirements

Your system must:

Be implemented in C++.
Include at least three different data structures from the above list.
Implement at least one graph-based algorithm (e.g., Dijkstra).
Allow the city to:
 Add new routes.
 Remove or update existing routes.
Find optimal routes between two points.
Sort or search for routes (by length, travel time, etc.).
Provide XAI output. Examples:
“Route A → B was chosen because it is 2 km shorter than Route C → D.”
“Shortest path was calculated using Dijkstra’s algorithm. Nodes visited: 5. Total cost: 12.”
“Routes were sorted by travel time using a custom comparator functor.”
Code comments: At every point in the program where an explanation is output, add (compulsory) a comment describing the reasoning. Example:

// XAI: This code will provide explanation as to why or how a node was selected next (shortest known distance) - you may also include a short section explaining the algorithm that you have used. Short - one or two sentences. In other words - be transparent in your code and explain why you are doing something specifically - don't hide the reasoning behind "complicated" code for the next programmer.

### Optional: AI Integration

You may include AI (e.g., predicting congestion based on time of day). If you do:
Clearly document where and how AI was applied.
Justify why AI was suitable.
Provide XAI output and corresponding code comments explaining the AI’s reasoning.

### Deliverables

Source Code (C++) with proper comments, including XAI comments where explanations appear.
Documentation (comments at the END of your code), where you explain to the examiner:
Problem analysis and chosen solution approach.
Which data structures/algorithms were used and why.
Where XAI principles are applied.
Where AI was used (if applicable).
Menu-driven interface for the city official to interact with.
