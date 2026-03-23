#include <lighting/lighting_engine.h>
#include <lighting/lighting_modes.h>
#include <debug.h>


state_common_t state_common;

void lighting_engine_init(void){
	lighting_init();
}

void event_dispatcher(light_command_t command){
	switch(command.event){
	case SET_MODE:
		if(((state_common.current_state = DYNAMIC_RAINBOW) || (state_common.current_state = DYNAMIC_SCENE)) && (lighting_is_fading())) timer_stop();
		switch(command.mode){
		case STATIC_RGB:
			state_common.current_state = STATIC_RGB;
			state_common.hue = 22;
			state_common.saturation = 255;
			state_common.value= 255;

			update_timer_callback(command.mode);
			state_common.status = SUCCESS;
			//led_toggle();	// debug
			lighting_set_rgb(state_common.hue, state_common.saturation, state_common.value, command.fade_time);	// set to default color i.e terracotta(brand color)
			break;
		case DYNAMIC_RAINBOW:
			state_common.current_state = DYNAMIC_RAINBOW;
			state_common.hue = 0;
			state_common.saturation = 1;
			state_common.value = 1;

			update_timer_callback(command.mode);

			state_common.status = BUSY;
			//led_toggle();	// debug
			lighting_start_rainbow();
			break;
		case DYNAMIC_SCENE:
			// call set color loop
			state_common.current_state = DYNAMIC_SCENE;
			update_timer_callback(command.mode);
			lighting_start_color_loop(command.modeParameters.color_arr , command.modeParameters.color_count, command.fade_time);

			break;
		case DYNAMIC_BREATHING:
			// call set breathing for the current color.
			state_common.current_state = DYNAMIC_BREATHING;
			break;
		default:
			break;
		}
		break;
	case SET_COLOR:
		switch(state_common.current_state){
		case STATIC_RGB:
			state_common.hue = command.hue;
			state_common.saturation = command.saturation;
			state_common.value = command.value;

			state_common.status = SUCCESS;
			//led_toggle();	// debug
			lighting_set_rgb(state_common.hue, state_common.saturation, state_common.value, command.fade_time);
			break;
		case DYNAMIC_RAINBOW:
			break;
		case DYNAMIC_SCENE:
			break;
		case DYNAMIC_BREATHING:
			break;
		default:
			break;
		}
		break;
	case SET_BRIGHTNESS:
		//set_brightness(/*something needs to come here*/);
			state_common.value = command.value;
			state_common.status = SUCCESS;
			lighting_set_rgb(state_common.hue, state_common.saturation, state_common.value, command.fade_time);
			break;
		break;
	default:
		// need to do something here
		break;
	}
}


void get_current_status(uint8_t *status){
	status[0] = state_common.status;
	status[1] = state_common.current_state;
	status[2] = state_common.value;
}
