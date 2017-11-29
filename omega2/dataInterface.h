#pragma once
#include "logging.h"

class dataInterface {
  public:
    dataInterface(logging* logger);
    ~dataInterface();
    int record();
  private:
    int writeCSV(int chunk[], const char filename[]);
    
    int getHeartRate(int chunk[]);
    void sortPeakHeight(int peaks[], const float gradient[], const int numpeaks);
    int* sortSet(const int arr[], const int len);
    void mySwap(int& a, int& b);
    
};