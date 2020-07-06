#ifndef __BSP_H__
#define __BSP_H__

#include <stdbool.h>
/* Board Support Package for the EK-TM4C123GXL board */

/* system clock setting [Hz] */
#define SYS_CLOCK_HZ 16000000U

/* BSP interface functions */
void BSP_init(void);
uint32_t BSP_tickCtr(void);

/* Watchdog functions */
void BSP_petWatchdog(void);

/* User Input section */
bool BSP_getSW1Pressed(void);
bool BSP_getSW2Pressed(void);

/* Led controlling functions */
void BSP_ledRedOn(void);
void BSP_ledRedOff(void);

void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);

void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);

void BSP_ledAllOff(void);
void BSP_ledAllOn(void);

#endif // __BSP_H__