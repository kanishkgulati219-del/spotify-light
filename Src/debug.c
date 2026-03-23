#include <debug.h>
#include <drivers/gpio.h>
#include <lighting/led_pwm.h>


#include <stdint.h>

void led_toggle(void){
	gpio_debug_led_toggle();
}


void error_crc_fail(){
	led_set_color(999,0,0);
	led_update();
}

void error_invalid_length(void){
	led_set_color(999,999,0);
	led_update();
}

void error_invalid_header(void){
	led_set_color(0,0,999);
	led_update();
}
