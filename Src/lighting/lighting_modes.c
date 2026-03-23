#include <stdint.h>
#include <math.h>

#include <lighting/lighting_modes.h>
#include <lighting/led_pwm.h>
#include <drivers/timer.h>
#include <debug.h>

static rgb to_rgb;
static hsv current;

static float to_h = 0.0f;
static float to_s = 0.0f;
static float to_v = 0.0f;

static float steps_h = 0.0f;
static float steps_s = 0.0f;
static float steps_v = 0.0f;

static uint16_t steps_remaining = 0;

static hsv static_color_arr[color_val];
static uint8_t static_color_count = 0;
static uint16_t static_fade_time;

static uint16_t period_ms = 10;			// generate a timer interrupt every 10ms
static bool is_active = false;				// is fade active


void (*update_callback)(void) = NULL;		// A function pointer that takes no argument and returns nothing

static void set_timer_callback(void (*callback) (void)){
	update_callback = callback;
}

static rgb hsv_to_rgb(float hue, float saturation, float value){
	rgb set_rgb;
	hue =fmod(hue, 360.0f);
	if(hue < 0) hue+= 360.0;
	if(saturation > 1.0) saturation = 1.0;
	if(value > 1.0) value = 1.0;
	float chroma = saturation * value;
	float sector = hue/60;
	float variable = chroma *(1 - fabs(fmod(sector, 2.0f) - 1.0));
	float m = value - chroma;

	uint8_t i = floorf(sector);
	switch(i){
	case 0:
		set_rgb.r = round((chroma+m)*999);
		set_rgb.g = round((variable+m)*999);
		set_rgb.b =round((0+m)*999);
		break;
	case 1:
		set_rgb.r = round((variable+m)*999);
		set_rgb.g = round((chroma+m)*999);
		set_rgb.b = round((0+m)*999);
		break;
	case 2:
		set_rgb.r = round((0+m)*999);
		set_rgb.g = round((chroma+m)*999);
		set_rgb.b = round((variable+m)*999);
		break;
	case 3:
		set_rgb.r = round((0+m)*999);
		set_rgb.g = round((variable+m)*999);
		set_rgb.b = round((chroma+m)*999);
		break;
	case 4:
		set_rgb.r = round((variable+m)*999);
		set_rgb.g = round((0+m)*999);
		set_rgb.b = round((chroma+m)*999);
		break;
	case 5:
		set_rgb.r = round((chroma+m)*999);
		set_rgb.g = round((0+m)*999);
		set_rgb.b = round((variable+m)*999);
		break;
	default:
		// signal red as error
		set_rgb.r = 999;
		set_rgb.g = 0;
		set_rgb.b = 0;
		break;
	}

	current.h = hue;
	current.s = saturation;
	current.v = value;

	return set_rgb;

}


// Initialise fade engine
void lighting_init(){
	// initialise tim4 for 10ms interrupt
	current.h = 0.0f;
	current.s = 0.0f;
	current.v = 0.0f;
	tim4_init();
}


static void lighting_set_color(uint16_t r, uint16_t g, uint16_t b){
	led_set_color(r,g,b);
}


// called by timer(periodic update)
void static_rgb_fade_update(void){
	if(is_active == false){
		return;
	}

	float set_h = current.h + steps_h;
	float set_s = current.s + steps_s;
	float set_v = current.v + steps_v;

	steps_remaining -= 1;
	if(steps_remaining == 0){
		set_h  = to_h;
		set_s  = to_s;
		set_v  = to_v;

		is_active = false;
		tim4_stop();
	}
	to_rgb = hsv_to_rgb(set_h, set_s, set_v);
	lighting_set_color(to_rgb.r, to_rgb.g, to_rgb.b);
	led_update();

}
void lighting_set_rgb(uint16_t h, uint8_t s, uint8_t v, uint16_t fade_time){
	to_h = (float)h;				//uint16_t
	to_s = s/255.0f;		// float
	to_v = v/255.0f;		// float

	uint16_t steps_total = fade_time/period_ms;

	float diff_h = fmod((to_h - current.h), 360.0f);
	if(diff_h > 180.0f) diff_h -=360.0f;
	if(diff_h < -180.0f) diff_h +=360.0f;
	steps_h = diff_h/steps_total;
	steps_s = (to_s - current.s)/steps_total;
	steps_v = (to_v - current.v)/steps_total;
	steps_remaining = steps_total;

	is_active = true;
	tim4_start();

}



// called by timer(periodic update)
void rainbow_fade_update(void){
	if(is_active == false){
		return;
	}
	float set_h = current.h + 1.0f;
	set_h = fmod(set_h, 360.0f);

	to_rgb = hsv_to_rgb(set_h, 1.0f, 1.0f);
	lighting_set_color(to_rgb.r, to_rgb.g, to_rgb.b);
	led_update();
}


void lighting_start_rainbow(){
	current.h = 0.0f;
	current.s = 1.0f;
	current.v = 1.0f;
	is_active = true;
	tim4_start();

}

// called by timer(periodic update)
void color_loop_fade_update(void){
	if(is_active == false){
		return;
	}

	static int8_t from = -1;
	static uint8_t to = 0;
	to = to%(static_color_count);
	static bool color_transition_done = false;


	if(color_transition_done == true){
		from++;
		to++;
		to_h = static_color_arr[0 + to*3];
		to_s = static_color_arr[1 + to*3];
		to_v = static_color_arr[2 + to*3];

		uint16_t steps_total = static_fade_time/period_ms;

		float diff_h = fmod((to_h - current.h), 360.0f);
		if(diff_h > 180.0f) diff_h -=360.0f;
		if(diff_h < -180.0f) diff_h +=360.0f;
		steps_h = diff_h/steps_total;
		steps_s = (to_s - current.s)/steps_total;
		steps_v = (to_v - current.v)/steps_total;
		steps_remaining = steps_total;

		color_transition_done = false;
	}


	float set_h = current.h + steps_h;
	float set_s = current.s + steps_s;
	float set_v = current.v + steps_v;

	steps_remaining -= 1;
	if(steps_remaining == 0){
		set_h  = to_h;
		set_s  = to_s;
		set_v  = to_v;
		color_transition_done = true;
	}
	to_rgb = hsv_to_rgb(set_h, set_s, set_v);
	lighting_set_color(to_rgb.r, to_rgb.g, to_rgb.b);
	led_update();
}


void lighting_start_color_loop(uint8_t *color_arr, uint8_t color_count, uint16_t fade_time){
	static_color_count = color_count;
	static_fade_time = fade_time;
	for(uint8_t i = 0; i < static_color_count; i++){
			static_color_arr[i].h =(float)color_arr[0 + i*3];
			static_color_arr[i].s = color_arr[1 + i*3]/255.0f;
			static_color_arr[i].v = color_arr[2 + i*3]/255.0f;
	}
	// transit to the first color
	to_h = static_color_arr[0].h;
	to_s = static_color_arr[0].s;
	to_v = static_color_arr[0].v;

	uint16_t steps_total = 400/period_ms;

	float diff_h = fmod((to_h - current.h), 360.0f);
	if(diff_h > 180.0f) diff_h -=360.0f;
	if(diff_h < -180.0f) diff_h +=360.0f;
	steps_h = diff_h/steps_total;
	steps_s = (to_s - current.s)/steps_total;
	steps_v = (to_v - current.v)/steps_total;
	steps_remaining = steps_total;

	is_active = true;
	tim4_start();

}

bool lighting_is_fading(void){
	return is_active;
}

void timer_stop(void){
	tim4_stop();
}

hsv get_color_status(void){
	return current;
}

void update_timer_callback(light_state_t state){
	switch(state){
	case STATIC_RGB:
		set_timer_callback(static_rgb_fade_update);
		break;
	case DYNAMIC_RAINBOW:
		set_timer_callback(rainbow_fade_update);
		break;
	case DYNAMIC_SCENE:
		set_timer_callback(color_loop_fade_update);
		break;

	default:
		break;
	}
}
