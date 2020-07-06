#include "TM4C123GH6PM.h"
#include "bsp.h"
#include <intrinsics.h>
#include <stdbool.h>

int main() {  
    BSP_init();
    
    while (1) {
      if( BSP_getSW1Pressed() == true ){
        BSP_ledRedOn();
      }
      else{
        BSP_ledRedOff();
      }
      if( BSP_getSW2Pressed() == true ){
        BSP_ledBlueOn();
      }
      else{
        BSP_ledBlueOff();
      }
    }
    //return 0;
}
