#pragma once

class dataInterface {
  public:
    dataInterface();
    ~dataInterface();
    int record();
  private:
    int getHeartRate(int chunk[]);
    int writeCSV(int chunk[], const char filename[]);
};