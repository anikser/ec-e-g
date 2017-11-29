#pragma once

#include <string>

class logging {
  public:
    logging();
    ~logging();
    int log(const char output[]);
    int warn(const char output[]);
    int error(const char output[]);
  private:
    const char* currentDateTime(); 
    int write(const char output[]);
};