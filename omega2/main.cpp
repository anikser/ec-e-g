#include "dataInterface.h"
#include "logging.h"

#include <stdio.h>
#include <ugpio/ugpio.h>
#include <unistd.h>

const int BUTTON_PIN = 11;
logging* logger;
char* temp;

int initButton(){
  if(!(gpio_is_requested(BUTTON_PIN))){
    temp = new char[80];
    sprintf(temp, "Exporting GPIO pin %i", BUTTON_PIN);
    logger->log(temp);
    if(gpio_request(BUTTON_PIN, NULL) < 0){
      temp = new char[80];
      sprintf(temp, "GPIO pin %i request failed", BUTTON_PIN);
      logger->error(temp);
      return -1;
    }
  }

  if(gpio_direction_input(BUTTON_PIN) < 0){
    temp = new char[80];
    sprintf(temp, "Failed to set pin %i to input", BUTTON_PIN);
    logger->error(temp);
    return -1;
  }
}


int main() {
  logger = new logging();
  logger->log("Program Start");
  initButton();
  dataInterface* interface = new dataInterface(logger);

  int val;
  bool cont = true;

  logger->log("Initialized program");
  while(cont){
    val = gpio_get_value(BUTTON_PIN);
    if(val){
      interface->record();
      //cont = false;
    }
    usleep(10000);
  }

  logger->log("Freeing GPIO pin...");
  gpio_free(BUTTON_PIN);
  logger->log("Freeing data interface...");
  delete interface;
  logger->log("Freeing logger...");
  delete logger;
  printf("Freed logger.\n");
  return 0;
}

