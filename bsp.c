/* Board Support Package */
#include "TM4C123GH6PM.h"
#include "bsp.h"

/* on-board LEDs */
#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)

/* on-board switch */
#define BTN_SW1   (1U << 4)
#define BTN_SW2   (1U << 0 )

/* Watchdog timer 0 */
#define WDTIMER0_ENABLE_BIT ( 1 << 0 )
#define WDTIMER_COUNT ( 0x0FFFFFFF )
#define WD_INTEN ( 1 << 0 )
#define WD_RESEN ( 1 << 1 )
#define WD_INTTYPE ( 1 << 2 )

static uint32_t volatile l_tickCtr;

__stackless void assert_failed (char const *file, int line) {
    /* TBD: damage control */
    NVIC_SystemReset(); /* reset the system */
}

void SysTick_Handler(void) { 
    ++l_tickCtr;
    //GPIOF_AHB->DATA ^= (LED_GREEN);
}

void BSP_init(void){
    /* Enable LEDs and set initial state to all off */
    SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF (Clock Gating)*/
    SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN); /* Enable direction as output for the 3 leds */
    GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN); /* Digital enable */
    BSP_ledAllOff();
    
    /* Initialize systick timer for every .5 seconds */
    SysTick->LOAD = SYS_CLOCK_HZ/2U - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = (1U << 2) | (1U << 1) | 1U;
      
    /* Initialize General-Purpose Timer 0 */
    SYSCTL->RCGCTIMER |= (1<<0);
    
    // 1. Ensure the timer is disabled ( the TnEN bit in the GPTMCTL register is cleared) before making any changes
    TIMER0->CTL &= ~ (1<<0);
    
    // 2. Write the GPTM Configuration Register (GPTMCFG) with a value of 0x000.0000.
    TIMER0->CFG = 0x00000000;
    
    // 3. Configure Timer A0 to periodic mode
    TIMER0->TAMR |= (0x2<<0);
    
    // 4. Configure as count down timer
    TIMER0->TAMR &= ~(1<<4);
    
    // 5. Load the start value 
    TIMER0->TAILR = 0x00F42400; // 16,000,000
    
    // 6. Re-enable the timer.
    TIMER0->CTL |= (1<<0);
    /* configure switch SW1 */
    GPIOF_AHB->DIR &= ~BTN_SW1; /* input */
    GPIOF_AHB->DEN |= BTN_SW1; /* digital enable */
    GPIOF_AHB->PUR |= BTN_SW1; /* pull-up resistor enable */
    
    /* GPIO interrupt setup for SW1 */
    GPIOF_AHB->IS  &= ~BTN_SW1; /* edge detect for SW1 */
    GPIOF_AHB->IBE &= ~BTN_SW1; /* only one edge generate the interrupt */
    GPIOF_AHB->IEV &= ~BTN_SW1; /* a falling edge triggers the interrupt */
    
    /* configure switch SW2 */
    GPIOF_AHB->LOCK |= 0x4C4F434B; // Unlock Code
    GPIOF_AHB->CR |= 0x00000001; //Commit Register
    GPIOF_AHB->DIR &= ~BTN_SW2; /* input */
    GPIOF_AHB->DEN |= BTN_SW2; /* digital enable */
    GPIOF_AHB->PUR |= BTN_SW2; /* pull-up resistor enable */

    /* GPIO interrupt setup for SW2 */
    GPIOF_AHB->IS  &= ~BTN_SW2; /* edge detect for SW2 */
    GPIOF_AHB->IBE &= ~BTN_SW2; /* only one edge generate the interrupt */
    GPIOF_AHB->IEV &= ~BTN_SW2; /* a falling edge triggers the interrupt */  
    
    /* Enable Watchdog timer */
    //SYSCTL->RCGCWD |= WDTIMER0_ENABLE_BIT;
    //WATCHDOG0->LOAD = WDTIMER_COUNT;
    //WATCHDOG0->CTL |= WD_RESEN;
    //WATCHDOG0->CTL |= WD_INTEN;
    
    __enable_irq();
}

uint32_t BSP_tickCtr(void) {
    uint32_t tickCtr;

    __disable_irq();
    tickCtr = l_tickCtr;
    __enable_irq();

    return tickCtr;
}

void BSP_petWatchdog(void){
  WATCHDOG0->LOAD = WDTIMER_COUNT;
}

void BSP_ledRedOn(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] = LED_RED;
}

void BSP_ledRedOff(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] = 0U;
}

void BSP_ledBlueOn(void) {
    GPIOF_AHB->DATA_Bits[LED_BLUE] = LED_BLUE;
}

void BSP_ledBlueOff(void) {
    GPIOF_AHB->DATA_Bits[LED_BLUE] = 0U;
}

void BSP_ledGreenOn(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] = LED_GREEN;
}

void BSP_ledGreenOff(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] = 0U;
}

bool BSP_getSW1Pressed(void){
    return GPIOF_AHB->DATA_Bits[BTN_SW1] == 0;
}

bool BSP_getSW2Pressed(void){
    return GPIOF_AHB->DATA_Bits[BTN_SW2] == 0;
}

void BSP_ledAllOn(void) {
  GPIOF_AHB->DATA |= (LED_RED | LED_BLUE | LED_GREEN);
}

void BSP_ledAllOff(void) {
  GPIOF_AHB->DATA &= ~(LED_RED | LED_BLUE | LED_GREEN);
}