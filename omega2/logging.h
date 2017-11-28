#pragma once

#include <string>

class logging {
  public:
    logging();
    ~logging();
    const std::string currentDateTime(); 
    int log(std::string output);
    int warn(std::string output);
    int error(std::string output);
  private:
    int write(std::string output);
};