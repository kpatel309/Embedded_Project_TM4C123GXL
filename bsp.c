//*****************************************************************************
//
// @file bsp.c
// @author Kushal Patel
// @brief Board Support Package. API to abstract hardware for user code.
//
//*****************************************************************************
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
#define WDTIMER_COUNT ( 0x07FFFFFF )
#define WD_INTEN ( 1 << 0 )
#define WD_RESEN ( 1 << 1 )
#define WD_INTTYPE ( 1 << 2 )

static void ADC_init(void);
static void LED_init(void);
static void SysTick_init(void);
static void Switches_init(void);
static void Timers_init(void);
static void Watchdog_init(void);

extern uint32_t l_tickCtr;

void BSP_init(void){

    LED_init();
    
    SysTick_init();
      
    Timers_init();
    
    Switches_init();
    
    ADC_init();
    
    Watchdog_init();
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
  WATCHDOG0->ICR = 25; //Can write any number to clear interrupt
}

void BSP_ledRedOn(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] = LED_RED;
}

void BSP_ledRedOff(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] = 0U;
}

void BSP_ledRedToggle(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] ^= LED_RED;
}

void BSP_ledBlueOn(void) {
    GPIOF_AHB->DATA_Bits[LED_BLUE] = LED_BLUE;
}

void BSP_ledBlueOff(void) {
    GPIOF_AHB->DATA_Bits[LED_BLUE] = 0U;
}

void BSP_ledBlueToggle(void) {
    GPIOF_AHB->DATA_Bits[LED_BLUE] ^= LED_BLUE;
}

void BSP_ledGreenOn(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] = LED_GREEN;
}

void BSP_ledGreenOff(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] = 0U;
}

void BSP_ledGreenToggle(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] ^= LED_GREEN;
}

void BSP_ledAllOn(void) {
  GPIOF_AHB->DATA |= (LED_RED | LED_BLUE | LED_GREEN);
}

void BSP_ledAllOff(void) {
  GPIOF_AHB->DATA &= ~(LED_RED | LED_BLUE | LED_GREEN);
}

bool BSP_getSW1Pressed(void){
    return GPIOF_AHB->DATA_Bits[BTN_SW1] == 0;
}

bool BSP_getSW2Pressed(void){
    return GPIOF_AHB->DATA_Bits[BTN_SW2] == 0;
}

static void ADC_init(void){
/* Initialize ADC */
    //1. Enable the ADC clock using the RCGCADC register
    SYSCTL->RCGCADC = (1<<1); // use ADC1
      
    //2. Enable the clock to the appropriate GPIO modules
    SYSCTL->RCGCGPIO = (1<<4) | (1<<5); // use port E
    GPIOE->DIR &= ~(1<<1);
    
    //3. Set the GPIO AFSEL bits for the ADC input pins
    GPIOE->AFSEL = (1<<1); // use PE1, AIN2
    
    //4. Configure the AINx signals to be analog inputs 
    GPIOE->DEN &= ~(1<<1); //PE1
    
    //5. Disable the analog isolation circuit for all ADC input pins thar are to be used 
    GPIOE->AMSEL = (1<<1); // PE1 
    
    //6. If required by the application, reconfigure the sample sequencer priorities in the ADCSSPRI register.
    
    /* Initialize Sample Sequencer */
    //1. Ensure that the sample sequencer is disabled by clearing the corresponding ASENn bit in the ADCACTSS register.
    ADC1->ACTSS &= ~(1<<3); // Sample Sequencer 3 is dissabled.
    
    //2. Configure the trigger event for the sample sequencer in the ADCEMUX register; What triggers a read?
    ADC1->EMUX = (0xF<<12); // SS3, trigger event = continuously sample
    
    //3. When using a PWM generator as the trigger source, use the ADC Trigger source select
    
    //4. For each sample in the sample sequence, configure the corresponding input source in the ADCSSMUXn register
    ADC1->SSMUX3 = 2;
    
    //5. For each sample in the sample sequence, configure the sample control bits in the corresponding nibble in the ADCSSCTLn register.
    ADC1->SSCTL3 = 0x6;
    
    //6. If interrupts are to be used, set the corresponding MASK bit in the ADCIM register
    ADC1->IM = (1<<3); // A sample has completed conversion and the respective ADCSSCTL3 IEn bit is set, enabling a raw interrupt
    
    //7. Enable the sample sequencer logic by setting the corresponding ASENn bit in the ADCACTSS register.
    ADC1->ACTSS |= (1<<3);
    
    ADC1->ISC = (1<<3); // clear interrupt flag
    
    NVIC_EnableIRQ(ADC1SS3_IRQn); //enable interrupt
}
static void LED_init(void){
    /* Enable LEDs and set initial state to all off */
    SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF (Clock Gating)*/
    SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN); /* Enable direction as output for the 3 leds */
    GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN); /* Digital enable */
    BSP_ledAllOff();
}

static void SysTick_init(void){
    /* Initialize systick timer for every .5 seconds */
    SysTick->LOAD = SYS_CLOCK_HZ/2U - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = (1U << 2) | (1U << 1) | 1U;
}

static void Switches_init(void){
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
}

static void Timers_init(void){
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

    // 6. Enable interrupts
    TIMER0->IMR |= (1<<0); // Timeout interrupt mask
    //NVIC->ISER[0] |= (1<<19); //Enable inteerupt request in NVIC Equivalent to below
    NVIC_EnableIRQ(TIMER0A_IRQn);

    // 7.. Re-enable the timer.
    TIMER0->CTL |= (1<<0);
}

static void Watchdog_init(void){
    /* Enable Watchdog timer */
    SYSCTL->RCGCWD |= WDTIMER0_ENABLE_BIT;
    WATCHDOG0->LOAD = WDTIMER_COUNT;
    WATCHDOG0->CTL |= WD_RESEN;
    WATCHDOG0->CTL |= WD_INTEN;
    NVIC_EnableIRQ(WATCHDOG0_IRQn);
}
