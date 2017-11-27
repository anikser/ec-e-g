#include "dataInterface.h"
#include <stdio.h>
#include <ugpio/ugpio.h>
#include <unistd.h>

const int BUTTON_PIN = 11;

int initButton(){
  if(!(gpio_is_requested(BUTTON_PIN))){
    printf("Exporting GPIO pin %i\n", BUTTON_PIN);
    if(gpio_request(BUTTON_PIN, NULL) < 0){
      printf("GPIO pin %i request failed.\n", BUTTON_PIN);
      return -1;
    }
  }
  if(gpio_direction_input(BUTTON_PIN) < 0){
    printf("Failed to set pin %i to input\n", BUTTON_PIN);
    return -1;
  }
}


int main() {
  printf("Program Start\n");

  initButton();
  dataInterface interface = dataInterface();

  int val;
  bool cont = true;

  while(cont){
    val = gpio_get_value(BUTTON_PIN);
    if(val){
      printf("Received value of %i\n", val);
      interface.record();
      
      cont = false;
    }
    usleep(10000);
  }
  
  gpio_free(BUTTON_PIN);
  delete &interface;
  return 0;
}

