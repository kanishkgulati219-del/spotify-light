#include "uart.h"
#include "gpio.h"
#include <stdint.h>

// defines
static uint32_t volatile *const rccEnableAPB2_reg = (uint32_t volatile*)0x40023844;		// To Enable the APB2

// usart1 registers
static uint32_t volatile *const usart1ST_reg = (uint32_t volatile*)0x40011000;			// Status register
static uint32_t volatile *const usart1DT_reg = (uint32_t volatile*)0x40011004;			// Data register
static uint32_t volatile *const usart1BRR_reg = (uint32_t volatile*)0x40011008;			// Set the Baud Rate for USART1
static uint32_t volatile *const usartCR1_reg = (uint32_t volatile*)0x4001100C;			// Control Register 1
static uint32_t volatile *const usartCR2_reg = (uint32_t volatile*)0x40011010;			// Control Register 2
//static uint32_t volatile *const usartCR3_reg = (uint32_t volatile*)0x40011014;			// Control Register 3


void uart1_init(void){
	*rccEnableAPB2_reg |= (1 << 4);					// Enable USART1 Clock; this step is necessary to further configure the peripheral; if not done any changes wouldnt be reflected

	*usartCR1_reg &= ~((1 << 15));					// Keep OVER8 at zero to keep over-sampling by 16

	*usart1BRR_reg = ((45 << 4) | (9 << 0));		// Mantissa|Fraction ; should be assigned and not OR for deterministic purposes

	*usartCR1_reg |= ((1 << 3));					// Enable Transmitter
	*usartCR1_reg &= ~((1 << 12));					// Keep M at zero for 8 data bits word
	*usartCR2_reg &= ~(3 << 12);					// Ensure STOP bit(s) is set to 1

	/* Now enable USART1 */
	*usartCR1_reg |= ((1 << 13));

}

void uart1_writeChar(char c){
	while(!(*usart1ST_reg & (1 << 7)));		// waiting until it becomes ready
	//debug line
	//gpio_debug_led_toggle();

	*usart1DT_reg = (uint16_t)c;					// Send character a
	// debug line
}


