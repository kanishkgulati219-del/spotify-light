#ifndef UART_H
#define UART_H


#include <stdint.h>
#include <stdbool.h>


extern volatile uint8_t recvCommand[32];
extern volatile bool command_recv;

void uart1_init(void);
uint8_t get_frame_length(void);
void uart1sendBuffer(uint8_t *buff, uint8_t frameLength);


#endif
