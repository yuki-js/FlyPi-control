#include <unistd.h>
#include "control.h"
#include <stdlib.h>
#include <stdio.h>

void *control(){
  if (gpioInitialise() < 0){
    return;
  }
  
  while(1){
    puts("LOOP");
    sleep(3);
  }
}
