//*****************************************************************************
//
// @file interrupt_handlers.c
// @author Kushal Patel
// @brief Board Support Package. API to abstract hardware for user code.
//
//*****************************************************************************

#include "TM4C123GH6PM.h"
#include "bsp.h"

volatile uint32_t l_tickCtr;
volatile uint32_t adcResult = 0;

__stackless void assert_failed (char const *file, int line) {
    /* TBD: damage control */
    NVIC_SystemReset(); /* reset the system */
}

void ADC1Seq3_IRQHandler(void)
{
    adcResult = ADC1->SSFIFO3;
    ADC1->ISC = (1<<3);
}

void SysTick_Handler(void) { 
    ++l_tickCtr;
    //BSP_ledBlueToggle();
}

void Timer0A_IRQHandler( void )
{
    TIMER0->ICR |= (1<<0);
    //BSP_ledRedToggle();
}