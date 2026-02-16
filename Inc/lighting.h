#ifndef	LIGHTING_H
#define LIGHTING_H

// set functions
void lighting_init(void);
void lighting_set_color(uint16_t r, uint16_t g, uint16_t b);
void lighting_set_brightness(uint16_t level);
void lighting_update(void);

// fade functions
void lighting_fade_to(uint16_t r, uint16_t g, uint16_t b, uint16_t time_ms);
uint8_t lighting_is_fading(void);
void lighting_stop_fade(void);





#endif	//LIGHTING_H
