#pragma once

#include "dataInterface.h"
#include "logging.h"

#include <string>
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
#include <math.h>

#include <ugpio/ugpio.h>

const int SAMPLE_PERIOD = 16;
const int MAX_RECORDING_POINTS = (1000/SAMPLE_PERIOD)*10;

const int MOVING_AVERAGE_WINDOW = MAX_RECORDING_POINTS/13;
const int GRADIENT_PEAK_THRESHOLD = 5;

const int BUFFER_SIZE = 50;
const int LED1_PIN = 18;
const int LED2_PIN = 19;
const char SERIAL_PORT[11] = "/dev/ttyS1";
speed_t BAUD_RATE = B19200;

int _fd;

logging* _logger;
char* temp2;

dataInterface::dataInterface(logging* logger){
  _logger = logger;
  _logger->log("Initializing data interface...");


  if (!gpio_is_requested(LED1_PIN)){
    if(gpio_request(LED1_PIN, NULL) < 0){
      temp2 = new char[80];
      sprintf(temp2, "GPIO pin %i request failed", LED1_PIN);
	    _logger->error(temp2);
	  }
  }

  if (gpio_direction_output(LED1_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Failed to set GPIO pin %i to output", LED1_PIN);
	  _logger->error(temp2);
  }

  if(gpio_set_value(LED1_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning off LED on GPIO pin %i", LED1_PIN);
    _logger->warn(temp2);
  }

  if (!gpio_is_requested(LED2_PIN)){
    if(gpio_request(LED2_PIN, NULL) < 0){
      temp2 = new char[80];
      sprintf(temp2, "GPIO pin %i request failed", LED2_PIN);
	    _logger->error(temp2);
	  }
  }

  if (gpio_direction_output(LED2_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Failed to set GPIO pin %i to output", LED2_PIN);
	  _logger->error(temp2);
  }

  if(gpio_set_value(LED2_PIN, 1) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning on LED on GPIO pin %i", LED2_PIN);
    _logger->warn(temp2);
  }

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
}

dataInterface::~dataInterface(){
  if(gpio_set_value(LED1_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning off LED on GPIO pin %i", LED1_PIN);
    _logger->warn(temp2);
  }

  if(gpio_set_value(LED2_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning off LED on GPIO pin %i", LED2_PIN);
    _logger->warn(temp2);
  }

  _logger->log("Closing serial device...");
  close(_fd);
  _logger->log("Freeing GPIO...");
  gpio_free(LED1_PIN);
}


void dataInterface::record(){
  if(gpio_set_value(LED2_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning off LED on GPIO pin %i", LED2_PIN);
    _logger->warn(temp2);
  }

  if(gpio_set_value(LED1_PIN, 1) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning on LED on GPIO pin %i", LED1_PIN);
    _logger->warn(temp2);
  }

  int chunk[MAX_RECORDING_POINTS];
  memset(chunk, 0, MAX_RECORDING_POINTS);

  char buff[BUFFER_SIZE];

  int ind = 0;
  while(ind < MAX_RECORDING_POINTS){
    if(read(_fd, buff, BUFFER_SIZE) < 0){
      _logger->error("Failed to read from serial port");
    };
    chunk[ind] = atoi(buff);
    //printf("Point: %i\n", chunk[ind]);
	  ind++;
  }

  if(gpio_set_value(LED1_PIN, 0) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning off LED on GPIO pin %i", LED1_PIN);
    _logger->warn(temp2);
  }

  _logger->recordData(chunk, MAX_RECORDING_POINTS, SAMPLE_PERIOD, getHeartRate(chunk));
  _logger->log("Successfully recorded data");

  if(gpio_set_value(LED2_PIN, 1) < 0){
    temp2 = new char[80];
    sprintf(temp2, "Error turning on LED on GPIO pin %i", LED2_PIN);
    _logger->warn(temp2);
  }
}

int dataInterface::getHeartRate(int chunk[]){
  _logger->log("Finding heart rate...");
  float smoothed[MAX_RECORDING_POINTS];
  int tempsum;
  int window;

  //Apply moving average filter

  for (int i=0; i<MAX_RECORDING_POINTS; i++){
    tempsum = 0;
    for (int j=i; j>=0 && ((i-j) < MOVING_AVERAGE_WINDOW); j--){
      tempsum += chunk[j];
      window = j;
    }
    smoothed[i] = (float)chunk[i] - (float)tempsum/window;
  }

  float gradient[MAX_RECORDING_POINTS-1];

  //Find gradients of each interval
  for (int i=1; i<MAX_RECORDING_POINTS; i++){
    gradient[i-1] = (smoothed[i] - smoothed[i-1])/(float)SAMPLE_PERIOD;
  }

  int numpeaks = 0;
  int peaks[MAX_RECORDING_POINTS/2];

  //find all maximums greater than given threshold
  for (int i=1; i<MAX_RECORDING_POINTS-1; i++){
    if(gradient[i] > gradient[i-1] && gradient[i] > gradient[i+1] && gradient[i] >= GRADIENT_PEAK_THRESHOLD){
      peaks[numpeaks] = i;
      numpeaks++;
    }
  }

  //Sort by peak height
  sortPeakHeight(peaks, gradient, numpeaks);

  //calculate distance
  float distances[numpeaks-5];
  int* sorted;
  float minvariance = 99999999;
  float minavgdist = -1;
  float tempvariance;
  float tempavgdist;

  //Select set with minimum variance
  for (int i=5; i<numpeaks; i++){
    sorted = sortSet(peaks, i);
    for(int j=0;j<i;j++){
    }

    tempavgdist = 0;
    tempvariance = 0;
    for(int j=0; j<i-1; j++){
      distances[j] = (sorted[j+1] - sorted[j]) * SAMPLE_PERIOD;
      tempavgdist += distances[j];
    }
    tempavgdist /= i-1;

    for(int j=0; j<i-1; j++){
      tempvariance += pow(distances[j]-tempavgdist, 2);
    }
    tempvariance /= i-1;
    
    if(tempvariance < minvariance){
      minvariance = tempvariance;
      minavgdist = tempavgdist;
    }
    delete[] sorted;
  }
  return (int)(60000/minavgdist);
}


void dataInterface::sortPeakHeight(int peaks[], const float gradient[], const int numpeaks){
  int max;
  for(int i=0; i<numpeaks; i++){
    max = i;
    for(int j=i; j<numpeaks; j++){
      if (gradient[j] > gradient[max]){
        max = j;
      }
    }
    mySwap(peaks[i], peaks[max]);
  }
}

int* dataInterface::sortSet(const int arr[], const int len){
  int* sorted = new int[len];
  for (int i=0; i<len; i++){
    sorted[i] = arr[i];
  }
  int min;
  for(int i=0; i<len; i++){
    min = i;
    for(int j=i; j<len; j++){
      if (sorted[j] < sorted[min]){
        min = j;
      }
    }
    mySwap(sorted[i], sorted[min]);
  }
  return sorted;
}

void dataInterface::mySwap(int &a, int &b){
  int temp = a;
  a = b;
  b = a;
}