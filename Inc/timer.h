#ifndef TIMER_H
#define TIMER_H

// period_ms = 10		generate a timer interrupt every 10ms

void tim4_init(void);
void tim4_start(void);
void tim4_stop(void);

void set_timer_callback(void (*callback) (void));

#endif	//TIMER_H
