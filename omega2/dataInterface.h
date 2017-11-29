#pragma once
#include "logging.h"

class dataInterface {
  public:
    dataInterface(logging* logger);
    ~dataInterface();
    int record();
  private:
    int writeCSV(int chunk[], const char filename[]);
    /*
    int getHeartRate(int chunk[]);
    int* sortPeakHeight(float peaks[], int numpeaks);
    int quicksortHelper(float& arr, left, right);
    void swap(int& a, int& b);
    */
};