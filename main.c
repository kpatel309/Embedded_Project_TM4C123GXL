#include "TM4C123GH6PM.h"
#include "bsp.h"
#include <intrinsics.h>
#include <stdbool.h>

volatile static uint32_t adcResult = 0;

void ADC1Seq3_IRQHandler(void)
{
  adcResult = ADC1->SSFIFO3;
  ADC1->ISC = (1<<3);
}

 int main() {  
    BSP_init();
    
    while (1) {
      if(adcResult>2048){
        BSP_ledGreenOn(); 
      }
      else{
        BSP_ledGreenOff();
      }
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
