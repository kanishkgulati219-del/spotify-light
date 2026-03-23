#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

// period_ms = 10		generate a timer interrupt every 10ms

void tim4_init(void);
void tim4_start(void);
void tim4_stop(void);

extern volatile bool call_update_function;

#endif	//TIMER_H
