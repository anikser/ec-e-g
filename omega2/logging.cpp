#pragma once

#include "logging.h"

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

std::ofstream* logfile;

logging::logging(){
  char* path = new char[80];
  sprintf(path, "/root/%s%s", currentDateTime(), ".log");
  printf("Initializing logging.\n");
  logfile =  new std::ofstream(path);
  printf("Initialized logging.\n");
}

logging::~logging(){
  //logfile->close();
}

int logging::log(const char output[]){
  *logfile << "(1) ";
  write(output);
  return 0;
}

int logging::warn(const char output[]){
  *logfile << "(2) Warning: ";
  write(output);
  return 0;
}

int logging::error(const char output[]){
  *logfile << "(3) Error: ";
  write(output);
  return 0;
}

const char* logging::currentDateTime() {
  time_t now;
  time(&now);
  struct tm* tstruct = localtime(&now);
  //std::cout << tstruct->tm_mday << std::endl; 

  char* buf = new char[80];
  
  //strftime not properly working
  //strftime(buf, sizeof(buf), "%Y_%m_%d_%X", tstruct);
  sprintf(buf, "%i-%i-%i_%i:%i:%i", tstruct->tm_year+1900, tstruct->tm_mon+1, tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
  return buf;
}

int logging::write(const char output[]){
  *logfile << output << std::endl;
  return 0;
}