#include <timer.h>
#include <stdint.h>
#include <stm32f407xx.h>



// RCC Registers
static uint32_t volatile *const rccEnableAPB1_reg = (uint32_t volatile*)0x40023840;		// To Enable the APB1

// TIM4 Registers
static uint32_t volatile *const tim4CR1_reg = (uint32_t volatile*)0x40000800;		// To configure Control register 1
static uint32_t volatile *const tim4SR_reg = (uint32_t volatile*)0x40000810;		// To configure Status Register
static uint32_t volatile *const tim4DIER_reg = (uint32_t volatile*)0x4000080C;		// To enable Timer interrupt
static uint32_t volatile *const tim4EG_reg = (uint32_t volatile*)0x40000814;		// To set the UG bit to reset counter
static uint32_t volatile *const tim4PSC_reg = (uint32_t volatile*)0x40000828;		// To configure PSC of TIM4
static uint32_t volatile *const tim4ARR_reg = (uint32_t volatile*)0x4000082C;		// To configure ARR of TIM4

static void (*timer_callback)(void) = 0;		// A function pointer that takes no argument and returns nothing

void set_timer_callback(void (*callback) (void)){
	timer_callback = callback;
}

void TIM4_IRQHandler(void){
	if (*tim4SR_reg & (1 << 0))   // Check UIF
	    {
	        *tim4SR_reg &= ~(1 << 0); // Clear UIF
	        lighting_fade_update();        // Your fade step
	    }
}

// a period of 10ms
void tim4_init(void){
	*rccEnableAPB1_reg &= ~(1 << 2);
	*rccEnableAPB1_reg |= (1 << 2);		// Enable APB1 clock for TIM4

	// the following config is used to generate an interrupt every 10ms
	*tim4PSC_reg = 0x53;				// Set PSC as 83
	*tim4ARR_reg = 0x270F;				// Set ARR to 9999; Controls frequency

	*tim4CR1_reg &= ~(1 << 1);			// Enable Update event generation
	*tim4CR1_reg |= (1 << 2);			// Only have counter overflow as an update source
	*tim4CR1_reg |= (1 << 4);			// enable up counting
	*tim4CR1_reg &= ~(3 << 5);			// Set Edge-aligned mode
	*tim4CR1_reg |= (1 << 7);			// Enable ARR preload

	*tim4DIER_reg |= (1 << 0);			// Enable update interrupt

	NVIC_EnableIRQ(TIM4_IRQn);			// Enable TIM4 interrupt in the NVIC
}

// start the timer
void tim4_start(void){
	*tim4CR1_reg |= (1 << 0);			// Start counter
}

void tim4_stop(void){
	*tim4CR1_reg &= ~(1 << 0);			// Stop counter
	*tim4EG_reg |= (1 << 0);			// Reset the counter
}
