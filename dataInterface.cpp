#pragma once
#include "dataInterface.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <ctime>

#include <ugpio/ugpio.h>

const int SAMPLE_PERIOD = 16;
const int MAX_RECORDING_POINTS = (1000/16)*10;
const int BUFFER_SIZE = 50;
const int LED1_PIN = 18;
const int LED2_PIN = 19;
const char SERIAL_PORT[11] = "/dev/ttyS1";
speed_t BAUD_RATE = B19200;

int _fd;

int _current;
bool _recording;


dataInterface::dataInterface(){
  printf("Initializing data interface...\n");
  _recording = false;

  _fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
  
  struct termios settings;
  tcgetattr(_fd, &settings);
  
  cfsetispeed(&settings, BAUD_RATE);
  settings.c_cflag &= ~PARENB;
  settings.c_cflag &= ~CSTOPB;
  settings.c_cflag &= ~CSIZE;
  settings.c_cflag |= CS8 | CLOCAL;
  settings.c_lflag = ICANON;
  settings.c_iflag &= ~IGNCR;

  tcsetattr(_fd, TCSANOW, &settings);
  tcflush(_fd, TCIFLUSH);

  fcntl(_fd, F_SETFL, 0);

  if (!gpio_is_requested(LED1_PIN)){
    if(gpio_request(LED1_PIN, NULL) < 0){
	    printf("GPIO pin %i request failed\n", LED1_PIN);
	  }
  }
  if (gpio_direction_output(LED1_PIN, 0) < 0){
	  printf("Failed to set pin %i to output\n", LED1_PIN);
  }
  printf("Initialized interface\n");
}

dataInterface::~dataInterface(){
  close(_fd);
  gpio_free(LED1_PIN);
}


int dataInterface::record(){
  _recording = true;

  if(gpio_set_value(LED1_PIN, 1) < 0){
    printf("Error turning on LED\n");
    return -1;
  }

  int chunk[MAX_RECORDING_POINTS];
  memset(chunk, 0, MAX_RECORDING_POINTS);

  char buff[BUFFER_SIZE];

  int ind = 0;
  while(ind < MAX_RECORDING_POINTS){
    if(read(_fd, buff, BUFFER_SIZE) < 0){
      printf("Failed to read from serial port\n");
    };
    chunk[ind] = atoi(buff);
    printf("Point: %i\n", chunk[ind]);
	  ind++;
  }
  
  writeCSV(chunk, "temp.csv");

  if(gpio_set_value(LED1_PIN, 0) < 0){
    printf("Error turning off LED\n");
    return -1;
  }
  return getHeartRate(chunk);
}

int dataInterface::writeCSV(int chunk[], const char filename[]){
  std::ofstream out;
  out.open(filename);
  for (int i=0; i<MAX_RECORDING_POINTS; i++){
    out << i*SAMPLE_PERIOD << "," << chunk[i] << std::endl;
  }
  out.close();
  return 0;
}


int dataInterface::getHeartRate(int chunk[]){
  return 100;
}