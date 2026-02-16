#ifndef UART_H
#define UART_H


#include "stdint.h"

void uart1_init(void);
void uart1_writeChar(char c);
void uart1_writeString(const char *str);
void uart1_writeHex(uint32_t value);


#endif
