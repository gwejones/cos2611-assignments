// 50052578A2P1.cpp
//
// 50052578
// Jones GWE

#include <iostream>
#include <ostream>
#include <vector>
using namespace std;

/**
 * @brief Performs bubble sort on vector of int.
 *
 * Sorts in ascending order.
 *
 * @param[out] arr The vector to be sorted. It is modifed in place.
 */
void bubbleSort(vector<int> &arr) {
  int temp;
  for (int lastIndex = arr.size() - 1; lastIndex > 0; lastIndex--)
    for (int i = 0; i < lastIndex; i++)
      if (arr[i] > arr[i + 1]) {
        temp = arr[i];
        arr[i] = arr[i + 1];
        arr[i + 1] = temp;
      }
}

// Helper function to perform the merge part of merge sort.
// Merges two sorted subvectors from `working` into `arr`.
void merge(vector<int> &arr, int iBegin, int iMiddle, int iEnd,
           vector<int> &working) {

  int i = iBegin, j = iMiddle;

  // while more elements remain to be merged
  for (int k = iBegin; k < iEnd; k++) {
    // choose smallest element from the current index of the two sorted
    // subvectors and copy into the current index of the destination
    if (i < iMiddle && (j >= iEnd || working[i] <= working[j]))
      arr[k] = working[i++];
    else
      arr[k] = working[j++];
  }
}

// Helper function to perform the split (recursive) part of merge sort
void splitMerge(vector<int> &arr, int iBegin, int iEnd, vector<int> &working) {
  if ((iEnd - iBegin) <= 1)
    return;
  int iMiddle = (iEnd + iBegin) / 2;
  // split and recursively sort both subvectors
  splitMerge(working, iBegin, iMiddle, arr);
  splitMerge(working, iMiddle, iEnd, arr);
  // merge sorted subvectors into single sorted vector
  merge(arr, iBegin, iMiddle, iEnd, working);
}

/**
 * @brief Performs merge sort on vector of int.
 *
 * Sorts in ascending order.
 *
 * @param[out] arr The vector to be sorted.
 */
void mergeSort(vector<int> &arr) {
  // Working copy which contains the partially sorted vector
  vector<int> working = arr;
  // We use these indices to keep track of a specific span of the vector, so
  // that we dont need to create new vectors on every iteration of the recursive
  // call. This so as to prevent costly copies done during assignments.
  int iBegin = 0, iEnd = arr.size();
  splitMerge(arr, iBegin, iEnd, working);
}

// Function to generate a random array of a given size
vector<int> generateRandomArray(int size) {
  vector<int> arr(size);
  srand(time(0));
  for (int i = 0; i < size; ++i) {
    arr[i] = rand() % 1000; // adjust the range to play around with it
  }
  return arr;
} // generateRandomArray

void measureSortingTimeFirstSortingAlgorithm(vector<int> &arr,
                                             double &timeExecuted) {

  // measure start time
  clock_t startTime = clock();

  bubbleSort(arr);

  // measure end time
  clock_t endTime = clock();

  // calculate executation time in seconds for first sorting algorithm
  timeExecuted = float(endTime - startTime) / CLOCKS_PER_SEC;

} // measureSortingTimeFirstSortingAlgorithm

void measureSortingTimeSecondSortingAlgorithm(vector<int> &arr,
                                              double &timeExecuted) {

  // measure start time
  clock_t startTime = clock();

  mergeSort(arr);

  // measure end time
  clock_t endTime = clock();

  // calculate executation time in seconds for second sorting algorithm
  timeExecuted = float(endTime - startTime) / CLOCKS_PER_SEC;

} // measureSortingTimeSecondSortingAlgorithm

int main() {
  cout << "COS2611 Assessment 2 Project 1\n";

  double timeExecuted1, timeExecuted2;
  vector<int> inputSizes = {100, 1000, 5000};

  for (int size : inputSizes) {
    cout << "\nInput size: " << size << endl;

    vector<int> arr1 = generateRandomArray(size),
                arr2 = generateRandomArray(size);

    // measure the sorting time for bubble sort
    measureSortingTimeFirstSortingAlgorithm(arr1, timeExecuted1);
    cout << "Bubble sort:\n";
    cout << "Execution time: " << fixed << timeExecuted1 << " seconds" << endl;

    // measure the sorting time for merge sort
    measureSortingTimeSecondSortingAlgorithm(arr2, timeExecuted2);
    cout << "Merge sort:\n";
    cout << "Execution time: " << fixed << timeExecuted2 << " seconds" << endl;
  }

  cout << "\nTime complexity formula for Bubble sort: O(n^2)\n";
  cout << "Time complexity formula for Merge sort: O(n log(n))\n";

  return 0;
}
