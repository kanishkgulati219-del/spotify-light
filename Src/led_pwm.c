#include <stdint.h>
#include <led_pwm.h>


static uint16_t req_r = 0;
static uint16_t req_g = 0;
static uint16_t req_b = 0;

uint16_t brightness = 1000;

void led_set_color(uint16_t r,uint16_t g,uint16_t b){
	req_r = r;
	req_g = g;
	req_b = b;
}


void led_set_brightness(uint16_t level){
	brightness = level;
}

void led_update(){
	pwm_set_duty(red, (999 - (brightness * req_r)/1000));
	pwm_set_duty(green, (999 - (brightness * req_g)/1000));
	pwm_set_duty(blue,(999 - (brightness * req_b)/1000));

	pwm_update();											// defined in pwm.c
}
