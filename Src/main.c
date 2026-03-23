/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Kanishk Gulati
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 * @notes: USART1 was selected because it is connected to the faster APB2 bus to allow for higher baud rates.
 * @notes: Prescaler values are always of power of 2; this is silicon behavior
 ******************************************************************************
 */

#include <stdint.h>

// Custom Includes
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <drivers/pwm.h>
#include <drivers/timer.h>
#include <event_queue.h>
#include <parser.h>
#include <lighting/lighting_engine.h>
#include <lighting/lighting_modes.h>

// Turn on FPU Co-Processor
static uint32_t volatile *const CPAC_reg = (uint32_t volatile*)0xE000ED88;				// Coprocessor access control register

// RCC stands for reset and clock control
static uint32_t volatile *const rccCR_reg = (uint32_t volatile*)0x40023800;				// RCC Clock Control Register
static uint32_t volatile *const rccCFG_reg = (uint32_t volatile*)0x40023808;			// To configure the RCC clock
static uint32_t volatile *const rccPLLCFG_reg = (uint32_t volatile*)0x40023804;			// To configure the RCC PLL
static uint32_t volatile *const flashAC_reg = (uint32_t volatile*)0x40023C00;			// To configure flash registers


int main(void)
{

	*CPAC_reg |=(3 << 20) | (3 << 22);		// Enable full access

	/* Enable and wait for HSI to start to select it as a valid PLL source*/
	*rccCR_reg |=(1 << 0);	// Enable HSI
	while(!(*rccCR_reg & (1 << 1)));	// Wait for HSI to be stable(set by HW)
	

	/* PLL Config */
	*rccCR_reg &= ~(1 << 24);			// Make sure PLL is disabled before we configure its mul/div factors

	*rccPLLCFG_reg &= ~((0x3F<<0) | (0x1FF << 6) | (0x3 << 16) | (0x1 << 22)); 	// clear PLLM | PLLN | PLLP | PLLSRC(HSI chosen as source for PLL)
	*rccPLLCFG_reg |= ((1 << 4) | (336 << 6) | (1 << 16));						// set PLLM to 16 to divide HSI by 16, PLLN to 336 and PLLP to 4

	*rccCFG_reg |= ((0 << 4) | (4 << 10) | (0 << 13));	// set prescalers for AHB=1, APB1 = 2, APB2 = 1

	// Enable PLL before selecting it as a SYSCLK source
	*rccCR_reg |= (1 << 24);

	// Wait till PLL is locked
	while(!(*rccCR_reg & (1 << 25)));

	// Set the Flash Wait-states to 2(3 CPU cycles)
	*flashAC_reg |= (2 << 0);

	*rccCFG_reg |= (2 << 0);	// Set PLL as SYSCLK

	// Wait for SYSCLK switch status to confirm PLL as the source
	while(((*rccCFG_reg >> 2) & 0x3) != 2);	// 2 confirms PLL as the source

	// drivers init
	gpio_init();
	uart1_init();
	pwm_init();			// initialise the LED PWM

	// architecture init
	protocol_init();
	lighting_engine_init();

	while(1){
		if(command_recv){
			command_recv = false;
			uint8_t frame_length = get_frame_length();
			protocol_process_frame(recvCommand, frame_length);

		}
		while(!event_queue_empty()){
			light_command_t cmd;
			event_queue_pop(&cmd);
			event_dispatcher(cmd);
			//gpio_debug_led_toggle();	// debug
		}

		if(call_update_function){
			call_update_function = false;
			update_callback();
		}
	}

}
