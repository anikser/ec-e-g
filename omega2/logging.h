#pragma once

#include <string>

class logging {
  public:
    logging();
    ~logging();

    void recordData(const int chunk[], const int len, const int period, const int heartRate);
    void log(const char output[]);
    void warn(const char output[]);
    void error(const char output[]);
  private:
    const char* currentDateTime(); 
    void write(const char output[], const int severity);
    void writeCSV(const int chunk[], const int len, const int period, const char* timestamp);
};