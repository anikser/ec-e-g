#pragma once

class logging {
  public:
    logging();
    ~logging();
    int log();
    int warn();
    int error();
  private:
    int write();
};