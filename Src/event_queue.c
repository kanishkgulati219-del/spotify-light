#include <event_queue.h>


event_queue_t event_queue;


static int getNextIndex(uint8_t index){
	return (index + 1) % 8;
}

static bool isEmpty(event_queue_t const* events){
	if(events->count == 0) return true;
	return false;
}

static bool isFull(event_queue_t const* events){
	if(events->count == 8) return true;
	return false;
}


bool event_queue_empty(void){
	return isEmpty(&event_queue);
}


void event_queue_init(void){
	event_queue.head = 0;				// Points to the next insertion spot
	event_queue.tail = 0;				// Points to the oldest data spot
	event_queue.count = 0;				// Current number of elements

}

void event_queue_push(light_command_t const *command){
	if(isFull(&event_queue)){
		return ;
	}

	event_queue.count += 1;
	event_queue.commandARR[event_queue.head] = *command;
	event_queue.head = getNextIndex(event_queue.head);
}
void event_queue_pop(light_command_t *cmd){
	if(isEmpty(&event_queue)){
		return ;
	}
	*cmd = event_queue.commandARR[event_queue.tail];
	event_queue.tail = getNextIndex(event_queue.tail);
	event_queue.count -= 1;

}
