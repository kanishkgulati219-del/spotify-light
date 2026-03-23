#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#include <lighting_types.h>

// event queue
typedef struct{
	uint8_t head;						// Points to the next insertion spot
	uint8_t tail;						// Points to the oldest data spot
	uint8_t count;						// Points to the current count of the elements
	light_command_t commandARR[8];	// The fixed-size array
}event_queue_t;

void event_queue_init(void);
void event_queue_push(light_command_t const *cmd);
void event_queue_pop(light_command_t *cmd);
bool event_queue_empty(void);


#endif	//EVENT_QUEUE_H
