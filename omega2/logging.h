#pragma once

#include <string>

class logging {
  public:
    logging();
    ~logging();
    int log(std::string output);
    int warn(std::string output);
    int error(std::string output);
  private:
    std::string currentDateTime(); 
    int write(std::string output);
};