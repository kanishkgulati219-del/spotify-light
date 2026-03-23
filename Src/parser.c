#include <string.h>

#include <parser.h>
#include <drivers/uart.h>
#include <event_queue.h>
#include <debug.h>


// uart command variables
volatile bool command_recv;
volatile uint8_t recvCommand[32];

// command struct variables

light_command_t command;

// function definitions
static bool protocol_parse_frame(volatile uint8_t *frame, uint8_t length); 							// validate SOF, length, CRC.
static void protocol_extract_params(volatile uint8_t *frame, light_command_t *cmd, uint8_t length); 	// extract HSV, mode, timing, etc.
static uint8_t protocol_compute_crc(volatile uint8_t *frame, uint8_t length); 			// checksum verification.

void protocol_init(void){
	for(int i = 0; i < sizeof(recvCommand)/sizeof(uint8_t); i++){
		recvCommand[i] = 0;
	}
	command_recv = false;
}


void protocol_process_frame(volatile uint8_t *frame, uint8_t length){
	if(!protocol_parse_frame(frame, length)){
		led_toggle();
		return ;
	}
	if(frame[2] == HEARTBEAT){
		uint8_t current_status[3], heartbeatAckArr[7];
		get_current_status(current_status);


		heartbeatAckArr[0] = 0xAA;
		heartbeatAckArr[1] = 0x82;	//HEARTBEAT_RESP
		heartbeatAckArr[2] = current_status[0];
		heartbeatAckArr[3] = current_status[1];
		heartbeatAckArr[4] = current_status[2];
		heartbeatAckArr[5] = 0x00;
		heartbeatAckArr[6] = protocol_compute_crc(heartbeatAckArr, 7);

		uart1sendBuffer(heartbeatAckArr, sizeof(heartbeatAckArr)/sizeof(uint8_t));

	}else{
		protocol_extract_params(frame, &command,length);
		// push the event to event queue
		event_queue_push(&command);
	}

}


static bool protocol_parse_frame(volatile uint8_t *frame, uint8_t length){
	// validate SOF
	if(*frame != 0xAA){
		//error_invalid_header();
		return false;
	}

	// validate length
	if(length != frame[1]){
		//error_invalid_length();
		return false;
	}

	// Compute CRC
	uint8_t compCRC = protocol_compute_crc(frame, length);
	// validate CRC
	if(compCRC != frame[length-1]){
		led_toggle();
		return false;
	}

	return true;
}


static uint8_t protocol_compute_crc(volatile uint8_t *frame, uint8_t length){
	uint8_t crc = 0;
	for(int i = 0 ; i < (length-1) ; i++){
		crc ^= frame[i];
	}

	return crc;
}

static void protocol_extract_params(volatile uint8_t *frame, light_command_t *cmd, uint8_t length){
	cmd->event = frame[2];
	switch(cmd->event){
	case SET_COLOR:
	{
		cmd->hue = (frame[3] << 8) | frame[4];
		cmd->saturation = frame[5];
		cmd->value = frame[6];
		cmd->fade_time = (frame[7] << 8) | frame[8];
		break;
	}
	case SET_MODE:
	{
		cmd->mode = frame[3];
		switch(cmd->mode){
		case STATIC_RGB:
			cmd->hue = (frame[4] << 8) | frame[5];		// 0 << 8 | 16	;	22 ( terracotta/amber hue)
			cmd->saturation = frame[6];
			cmd->value=frame[7];
			cmd->fade_time = (frame[8] << 8) | frame[9];
			break;
		case DYNAMIC_RAINBOW:
			break;
		case DYNAMIC_SCENE:
		{
			// 170 24 1 3 4 0 32 6 0 0 214 55 255 0 209 31 255 0 208 15 255 19 136 6
			cmd->modeParameters.color_count = frame[4];
			for(uint8_t i =0; i < cmd->modeParameters.color_count ; i++){
				cmd->modeParameters.color_arr[0 + i*3] =(frame[5 + i*4] << 8) | frame[6 + i*4];
				cmd->modeParameters.color_arr[1 + i*3] = frame[7 + i*4];
				cmd->modeParameters.color_arr[2 + i*3] = frame[8 + i*4];
			}
			cmd->fade_time = (frame[4+(cmd->modeParameters.color_count * 4) + 1] << 8) | frame[5+(cmd->modeParameters.color_count *4)+ 1];
			break;
		}
		case DYNAMIC_BREATHING:
			break;
		default:
			break;
		}
		break;
	}
	case SET_BRIGHTNESS:
	{
		cmd->value = frame[3];
		cmd->fade_time = (frame[4] << 8) | frame[5];
		break;
	}
	default:
		break;
	}

}
