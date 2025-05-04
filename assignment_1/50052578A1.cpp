// Name: G.W.E Jones
// Student Number: 50052578

#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <stack>
#include <vector>

using namespace std;

const string FILENAME = "C:\\data\\taxiData.txt";
const int TIME_MARGIN = 15; // simulation will continue for this many units
                            // after last arrival of a passenger

struct Passenger {
  char destination_type;
  int processing_time;
};
struct Arrival {
  int time;
  Passenger passenger;
};
struct Taxi {
  int capacity;
  stack<Passenger> passengers; // currently coarding passenger at top of queue
};

/**
 * @brief Load arrivals data.
 *
 * Loads passenger arrivals from file into and arrivals queue.
 *
 * @return A queue containing arrivals in chronological order.
 */
static queue<Arrival> loadData() {
  queue<Arrival> arrival_queue;
  string row;
  ifstream infile(FILENAME);
  if (infile.fail())
    throw runtime_error("Failed to open file \'" + FILENAME + "\'");
  while (getline(infile, row) && row.length() > 1) {
    stringstream ss(row);
    string cell;
    vector<string> rowcells;
    while (getline(ss, cell, ','))
      rowcells.push_back(cell);
    struct Passenger passenger = {rowcells[1][0], stoi(rowcells[2])};
    struct Arrival arrival = {stoi(rowcells[0]), passenger};
    arrival_queue.push(arrival);
  }
  return arrival_queue;
}

/**
 * @brief Process passenger arrivals.
 *
 * Processes new passengers arriving at the taxi rank at the current time.
 * Pushes eached passenger in to the appropriate waiting queue for
 * their desired route.
 *
 * @param t Current time instance.
 * @param arrival_queue Passenger arrival queue.
 * @param s_taxi_queue Waiting queue for short-distance taxi.
 * @param l_taxi_queue Waiting queue for long-distance taxi.
 * @param c_taxi_queue Waiting queue for city taxi.
 *
 * @return A vector of passengers which have arrived during current time
 * instance.
 */
static vector<Passenger> processArrivals(int t, queue<Arrival> &arrival_queue,
                                         queue<Passenger> &s_taxi_queue,
                                         queue<Passenger> &l_taxi_queue,
                                         queue<Passenger> &c_taxi_queue) {
  vector<Passenger> arrivals;
  while (!arrival_queue.empty() && arrival_queue.front().time <= t) {
    Arrival arrival = arrival_queue.front();
    switch (arrival.passenger.destination_type) {
    case 'S':
      s_taxi_queue.push(arrival.passenger);
      break;
    case 'L':
      l_taxi_queue.push(arrival.passenger);
      break;
    default:
      c_taxi_queue.push(arrival.passenger);
    }
    arrival_queue.pop();
    arrivals.push_back(arrival.passenger);
  }
  return arrivals;
}

/**
 * @brief Process taxxi boarding.
 *
 * Processes taxi boarding logic for the current time instance.
 * A full taxi will depart and be replaced with a new taxi.
 * Remaining boarding time for currently boarding taxi is decremented.
 * A new passenger starts boarding if there is one waiting in the queue
 * and the current one has finished boarding.
 *
 * @param taxi The taxi.
 * @param ttaxi_queue The taxi queue.
 */
static void processBoarding(Taxi &taxi, queue<Passenger> &taxi_queue) {
  // taxi departed in the last time instance, a new one arrives
  if ((taxi.passengers.size() == 5) && !taxi.passengers.top().processing_time)
    taxi = {5, stack<Passenger>()};
  // passenger continues boarding
  if (!taxi.passengers.empty() && taxi.passengers.top().processing_time)
    if (!--taxi.passengers.top().processing_time)
      // passenger has completed boarding, decrement capacity
      taxi.capacity--;
  // new passenger starts boarding
  if ((taxi.passengers.empty() || !taxi.passengers.top().processing_time) &&
      taxi_queue.size() && taxi.passengers.size() < 5) {
    taxi.passengers.push(taxi_queue.front());
    taxi_queue.pop();
  }
}

/**
 * @brief Display taxi.
 *
 * Displays the passenger currently boarding a taxi, or the string "Depart"
 * if the taxi will depart during the current time instance.
 *
 * @param taxi The taxi.
 *
 * @return String display of passenger boarding taxi.
 */
static string toString(const Taxi taxi) {
  if (taxi.passengers.empty())
    return "";
  if (!taxi.passengers.top().processing_time && taxi.passengers.size() == 5)
    return "Depart";
  stringstream ss_builder;
  ss_builder << taxi.passengers.top().destination_type << taxi.passengers.size()
             << '(' << taxi.passengers.top().processing_time << ')';
  return ss_builder.str();
}

/**
 * @brief Display passenger queue.
 *
 * Displays the front passenger in a list of waiting passengers.
 *
 * @param queue Vector containing passengers in queue.
 * @param taxiSize Current size of taxi for which the passengers wait.
 *
 * @return String display of passenger at the top of the queue.
 */
static string toString(const queue<Passenger> queue, const size_t taxiSize) {
  if (queue.empty())
    return "";
  stringstream ss_builder;
  ss_builder << queue.front().destination_type << taxiSize + 1 << '('
             << queue.front().processing_time << ')';
  return ss_builder.str();
}

/**
 * @brief Display arrivals.
 *
 * Displays a list of passengers arriving at the current time, given
 * a vector of them.
 *
 * @param current_arrivals List of current arrivals.
 *
 * @return String display of list of current arrivals.
 */
static string toString(const vector<Passenger> current_arrivals) {
  if (current_arrivals.empty())
    return "";
  stringstream ss_builder;
  for (u_int i = 0; i < current_arrivals.size(); i++) {
    if (i > 0)
      ss_builder << ' ';
    ss_builder << current_arrivals[i].destination_type << '('
               << current_arrivals[i].processing_time << ')';
  }
  return ss_builder.str();
}

int main() {
  queue<Arrival> arrival_queue =
      loadData(); // queue of passengers arriving at taxi rank
  queue<Passenger> s_taxi_queue, l_taxi_queue,
      c_taxi_queue; // queue of passengers for each taxi route
  struct Taxi s = {5, stack<Passenger>()}, l = {5, stack<Passenger>()},
              c = {5, stack<Passenger>()}; // a taxi for each route
  int t = 0;                               // current time instance
  int last_arrival_time = 0; // last time at which a new passenger arrived
  vector<Passenger> current_arrivals; // arrivals for current time instance

  cout << "T\t" << setw(20) << left << "Next" << '\t' << "S\t" << "L\t" << "C\t"
       << "WQS\t" << "WQL\t" << "WQC\t"
       << "CS\t" << "CL\t" << "CC\t" << endl;

  while (t <= last_arrival_time + TIME_MARGIN) {
    if (arrival_queue.size())
      last_arrival_time = t;
    current_arrivals = processArrivals(t, arrival_queue, s_taxi_queue,
                                       l_taxi_queue, c_taxi_queue);
    processBoarding(s, s_taxi_queue);
    processBoarding(l, l_taxi_queue);
    processBoarding(c, c_taxi_queue);

    cout << t << '\t' << setw(20) << left << toString(current_arrivals) << '\t'
         << toString(s) << '\t' << toString(l) << '\t' << toString(c) << '\t'
         << toString(s_taxi_queue, s.passengers.size()) << '\t'
         << toString(l_taxi_queue, l.passengers.size()) << '\t'
         << toString(c_taxi_queue, c.passengers.size()) << '\t' << s.capacity
         << '\t' << l.capacity << '\t' << c.capacity << endl;

    t++;
  }
  return 0;
}