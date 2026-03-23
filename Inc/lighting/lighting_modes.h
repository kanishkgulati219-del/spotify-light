#ifndef LIGHTING_MODES_H
#define LIGHTING_MODES_H


#include <stdbool.h>
#include <lighting_types.h>

//type
typedef struct{
	volatile uint16_t r;
	volatile uint16_t g;
	volatile uint16_t b;
}rgb;

typedef struct{
	volatile float h;
	volatile float s;
	volatile float v;
}hsv;

extern void (*update_callback)(void);

// helper functions
void lighting_init();
void timer_stop(void);
bool lighting_is_fading(void);
hsv get_color_status(void);

// mode functions
void lighting_set_rgb(uint16_t h, uint8_t s, uint8_t v, uint16_t fade_time);
void lighting_start_rainbow();
void lighting_start_color_loop(uint8_t *color_arr, uint8_t color_count, uint16_t fade_time);
void lighting_start_breathing(uint16_t period);
//void lighting_start_color_loop(const hsv_t *colors, uint8_t count);

// indirect timer callback update functions
void static_rgb_fade_update(void);
void rainbow_fade_update(void);
void color_loop_fade_update(void);


extern void (*update_callback)(void);
void update_timer_callback(light_state_t state);


#endif	//LIGHTING_MODES_H
