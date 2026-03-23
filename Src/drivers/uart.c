#include <drivers/uart.h>
#include <drivers/gpio.h>


#include <stm32f407xx.h>
#include <debug.h>

// defines
static uint32_t volatile *const rccEnableAPB2_reg = (uint32_t volatile*)0x40023844;		// To Enable the APB2

// usart1 registers
static uint32_t volatile *const usart1ST_reg = (uint32_t volatile*)0x40011000;			// Status register
static uint32_t volatile *const usart1DT_reg = (uint32_t volatile*)0x40011004;			// Data register
static uint32_t volatile *const usart1BRR_reg = (uint32_t volatile*)0x40011008;			// Set the Baud Rate for USART1
static uint32_t volatile *const usartCR1_reg = (uint32_t volatile*)0x4001100C;			// Control Register 1
static uint32_t volatile *const usartCR2_reg = (uint32_t volatile*)0x40011010;			// Control Register 2
//static uint32_t volatile *const usartCR3_reg = (uint32_t volatile*)0x40011014;			// Control Register 3

volatile static uint8_t frame_length = 0;


void USART1_IRQHandler(void){
	volatile static uint8_t i = 0;
	if((*usart1ST_reg >> 5) & 1){
		if(i == 0){
			recvCommand[i] = *usart1DT_reg;
			if(recvCommand[i] != 0xAA){			// check for a valid command here as well before pushing the frame to parser.
				i=0;
				return;
			}
		}else if(i == 1){
			recvCommand[i] = *usart1DT_reg;
			frame_length = recvCommand[i];
			if(frame_length < 4 || frame_length > 32){
				i = 0;
				return;
			}
		}else{
			recvCommand[i] = *usart1DT_reg;		// store the byte in usart1DT_reg to the command position i
		}

		if(i == (frame_length - 1)){
			i=0;
			command_recv = true;
		}else{
			i+=1;
		}
	}
}


void uart1_init(void){
	// init heartbeat acknowledgement array
	*rccEnableAPB2_reg |= (1 << 4);					// Enable USART1 Clock; this step is necessary to further configure the peripheral; if not done any changes wouldnt be reflected

	*usartCR1_reg &= ~((1 << 15));					// Keep OVER8 at zero to keep over-sampling by 16

	*usart1BRR_reg = ((45 << 4) | (9 << 0));		// Mantissa|Fraction ; should be assigned and not OR for deterministic purposes

	*usartCR1_reg |= ((1 << 2));					// Enable Receiver
	*usartCR1_reg |= ((1 << 3));					// Enable Transmitter
	*usartCR1_reg |= ((1 << 5));					// Enable Receiver Interrupt(RXNE=1)
	*usartCR1_reg &= ~((1 << 12));					// Keep M at zero for 8 data bits word
	*usartCR2_reg &= ~(3 << 12);					// Ensure STOP bit(s) is set to 1

	NVIC_EnableIRQ(USART1_IRQn);			// Enable USART interrupt in the NVIC
	/* Now enable USART1 */
	*usartCR1_reg |= ((1 << 13));


}

static void uart1_writeHex(uint8_t byte){
	while(!(*usart1ST_reg & (1 << 7)));		// waiting until it becomes ready
	*usart1DT_reg = byte;					// Send character a
	// debug line
}

void uart1sendBuffer(uint8_t *buff, uint8_t frameLength){
	for(int i=0 ; i<frameLength ; i++){
		uart1_writeHex(buff[i]);
	}
}

uint8_t get_frame_length(void){
	return frame_length;
}


