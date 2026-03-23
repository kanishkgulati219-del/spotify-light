#ifndef LIGHTING_TYPES_H
#define LIGHTING_TYPES_H

#define scene_colors	4
#define color_val	scene_colors * 3

typedef struct{
	uint8_t color_count;
	uint8_t color_arr[color_val];
}mode_parameters_t;

typedef enum{
	SUCCESS,
	BUSY,
	ERROR
}status_t;

typedef enum{
	NO_STATE = 0,
	STATIC_RGB,
	/* STATIC CCT TO BE ADDED */
	DYNAMIC_RAINBOW,
	DYNAMIC_SCENE,
	DYNAMIC_BREATHING,
}light_state_t;

typedef enum{
	INVALID_EVENT = -1,
	HEARTBEAT,
	SET_MODE,
	SET_COLOR,
	SET_BRIGHTNESS,
	SET_TEMP,			// for CCT LEDs
	GET_STATUS,
}light_event_t;

typedef struct{
	uint16_t hue;
	uint16_t fade_time;
	uint8_t saturation;
	uint8_t value;
	mode_parameters_t modeParameters;
	light_event_t event;
	light_state_t mode;

}light_command_t;

#endif	//LIGHTING_TYPES_H
