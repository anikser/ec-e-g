#pragma once

#include "logging.h"

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

const char* JOURNAL_FILENAME = "journal.txt";

std::ofstream* logfile;
std::ofstream* journalfile;

logging::logging(){
  char* path = new char[80];
  sprintf(path, "/root/%s%s", currentDateTime(), ".log");
  printf("Initializing logging.\n");
  logfile =  new std::ofstream(path, std::ios_base::out);
  journalfile = new std::ofstream(JOURNAL_FILENAME, std::ios_base::app | std::ios_base::out);
  printf("Initialized logging.\n");
}

logging::~logging(){
  //logfile->close();
}

void logging::recordData(const int chunk[], const int len, const int period, const int heartRate){
  const char* timestamp = currentDateTime();
  writeCSV(chunk, len, period, timestamp);
  *journalfile << timestamp;
  *journalfile << " Data stored in " << timestamp << ".csv. ";
  *journalfile << "Average heartrate: " << heartRate << std::endl;
  printf("Data stored in %s. Average heartrate: %i", timestamp, heartRate);
}


void logging::log(const char output[]){
  write(output, 1);
}

void logging::warn(const char output[]){
  write(output, 2);
}

void logging::error(const char output[]){
  write(output, 3);
}

const char* logging::currentDateTime() {
  time_t now;
  time(&now);
  struct tm* tstruct = localtime(&now);
  char* buf = new char[80];
  //strftime not properly working
  //strftime(buf, sizeof(buf), "%Y_%m_%d_%X", tstruct);
  sprintf(buf, "%i-%i-%i_%i:%i:%i", tstruct->tm_year+1900, tstruct->tm_mon+1, tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
  return buf;
}

void logging::write(const char output[], const int severity){
  *logfile << currentDateTime();
  switch (severity){
    case 1:
      *logfile << " (1) ";
      break;
    case 2:
      *logfile << " (2) Warning: ";
      break;
    case 3:
      *logfile << " (3) Error: ";
  }
  *logfile << output << std::endl;
}

void logging::writeCSV(const int chunk[], const int len, const int period, const char* timestamp){
  char filename[80];
  sprintf(filename, "%s.csv", timestamp);
  std::ofstream out;
  out.open((const char*)filename);
  for (int i=0; i<len; i++){
    out << i*period << "," << chunk[i] << std::endl;
  }
  out.close();
}