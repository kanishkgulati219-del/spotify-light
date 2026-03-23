#ifndef LIGHTING_ENGINE
#define LIGHTING_ENGINE

#include <stdint.h>
#include <lighting/lighting_modes.h>
#include <lighting_types.h>

typedef struct{
	uint16_t hue;
	light_state_t current_state;
	uint8_t saturation;
	uint8_t value;
	uint8_t status;
	uint8_t error;

}state_common_t;

extern state_common_t state_common;

void lighting_engine_init(void);
void event_dispatcher(light_command_t command);
void lighting_engine_update(void);
void get_current_status(uint8_t *status);

#endif	// LIGHTING_ENGINE
