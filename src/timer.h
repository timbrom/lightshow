#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include <stdint.h>

extern void (*timer_callback_handler)(void);

void timer_init(void);
void timer_startInterval(uint16_t period_ms);
void timer_stopInterval(void);
void timer_toggleLed(void);

#endif /* TIMER_H */

