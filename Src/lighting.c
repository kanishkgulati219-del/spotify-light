#include <stdint.h>

#include <lighting.h>
#include <led_pwm.h>
#include <timer.h>

#include <stdbool.h>

static volatile uint16_t current_r = 0;
static volatile uint16_t current_g = 0;
static volatile uint16_t current_b = 0;

static uint16_t to_r = 0;
static uint16_t to_g = 0;
static uint16_t to_b = 0;

static uint16_t steps_r = 0;
static uint16_t steps_g = 0;
static uint16_t steps_b = 0;

static volatile uint16_t steps_remaining = 0;

uint16_t period_ms = 10;			// generate a timer interrupt every 10ms
bool is_active = false;				// is fade active

// Function definition
void lighting_fade_update(void);

// Initialise fade engine
void lighting_init(void){
	// initialise tim4 for 10ms interrupt
	set_timer_callback(lighting_fade_update);
	tim4_init();
}

void lighting_set_color(uint16_t r, uint16_t g, uint16_t b){
	current_r = r;
	current_g = g;
	current_b = b;
	led_set_color(current_r, current_g, current_b);
}

void lighting_set_brightness(uint16_t level){
	led_set_brightness(level);
}

void lighting_update(void){
	led_update();
}

// called by timer(periodic update)
void lighting_fade_update(void){
	if(is_active == false){
		return;
	}

	uint16_t volatile set_r = current_r + steps_r;
	uint16_t volatile set_g = current_g + steps_g;
	uint16_t volatile set_b = current_b + steps_b;

	steps_remaining -= 1;
	if(steps_remaining == 0){
		set_r  = to_r;
		set_g  = to_g;
		set_b  = to_b;

		is_active = false;
		tim4_stop();
	}
	lighting_set_color(set_r, set_g, set_b);
	led_update();

}

uint8_t lighting_is_fading(void){
	// wait for all colors to be set
	return is_active;
}

void lighting_fade_to(uint16_t r, uint16_t g, uint16_t b, uint16_t time_ms){
	to_r = r;
	to_g = g;
	to_b = b;
	uint16_t steps_total = time_ms/period_ms;
	steps_r = (to_r - current_r)/steps_total;
	steps_g = (to_g - current_g)/steps_total;
	steps_b = (to_b - current_b)/steps_total;
	steps_remaining = steps_total;

	is_active = true;
	tim4_start();
}
