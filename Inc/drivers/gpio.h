#ifndef GPIO_H
#define GPIO_H

void gpio_init(void);				// entry point for GPIO setup
void gpio_debug_led_on();
void gpio_debug_led_off();
void gpio_debug_led_toggle();

#endif
