#ifndef LED_PWM
#define LED_PWM

#include <pwm.h>

void led_set_color(uint16_t r,uint16_t g,uint16_t b);	// Set color
void led_set_brightness(uint16_t level);					// Global dimmer
void led_update(void);									// Apply brightness scaling + polarity correction → call pwm



#endif	//LED_PWM
