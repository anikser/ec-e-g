#pragma once

#include "logging.h"

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

std::ofstream logfile;

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y_%m_%d_%X", &tstruct);

    return buf;
}
logging::logging(){
	//get timestamp -> filename
	std::string filename = currentDateTime() + ".log";
	logfile.open(filename);


}

logging::~logging(){
}

int logging::log(std::string output){
  logfile << "(1) Success: ";
  write(output);
  return 0;
}

int logging::warn(std::string output){
  logfile << "(2) Warning: ";
  write(output);
  return 0;
}

int logging::error(std::string output){
  logfile << "(3) Error: ";
  write(output);
  return 0;
}

int logging::write(std::string output){
  logfile << output << std::endl;
  return 0;
}