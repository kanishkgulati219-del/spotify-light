#include <stdint.h>
#include <pwm.h>

// RCC Registers
static uint32_t volatile *const rccEnableAPB1_reg = (uint32_t volatile*)0x40023840;		// To Enable the APB1

// TIM3 Registers
static uint32_t volatile *const tim3CR1_reg = (uint32_t volatile*)0x40000400;		// To configure Control register 1
static uint32_t volatile *const tim3EG_reg = (uint32_t volatile*)0x40000414;		// To configure Event Gen Register
static uint32_t volatile *const tim3CCM1_reg = (uint32_t volatile*)0x40000418;		// To configure Capture/Compare Mode of Channels 1 and 2
static uint32_t volatile *const tim3CCM2_reg = (uint32_t volatile*)0x4000041C;		// To configure Capture/Compare Mode of Channels 3 and 4
static uint32_t volatile *const tim3CCEN_reg = (uint32_t volatile*)0x40000420;		// To configure Capture/Compare Enable
static uint32_t volatile *const tim3PSC_reg = (uint32_t volatile*)0x40000428;		// To configure PSC of TIM3
static uint32_t volatile *const tim3ARR_reg = (uint32_t volatile*)0x4000042C;		// To configure ARR of TIM3
static uint32_t volatile *const tim3CCR1_reg = (uint32_t volatile*)0x40000434;		// Channel 1 duty config
static uint32_t volatile *const tim3CCR2_reg = (uint32_t volatile*)0x40000438;		// Channel 2 duty config
static uint32_t volatile *const tim3CCR3_reg = (uint32_t volatile*)0x4000043C;		// Channel 3 duty config

static inline void ch1_config(void){
	/* Channel 1 config */
	*tim3CCM1_reg &= ~(3 << 0);		// set the channel 1 as output
	*tim3CCM1_reg |= (1 << 3);		// set pre-load enable to avoid glitches when a new brightness command is received whilst LED is on

	*tim3CCM1_reg &= ~(7 << 4);		// Clear OC1M(Output Compare Mode) bits
	*tim3CCM1_reg |= (6 << 4);		// Set PWM Mode 1 where as long as CNT < CCR the signal stays high
}

static inline void ch2_config(void){
	/* Channel 2 config */
	*tim3CCM1_reg &= ~(3 << 8);		// set the channel 2 as output
	*tim3CCM1_reg |= (1 << 11);		// set pre-load enable to avoid glitches when a new brightness command is received whilst LED is on

	*tim3CCM1_reg &= ~(7 << 12);	// Clear OC2M(Output Compare Mode) bits
	*tim3CCM1_reg |= (6 << 12);		// Set PWM Mode 1 where as long as CNT < CCR the signal stays high
}

static inline void ch3_config(void){
	/*Channel 3 config */
	*tim3CCM2_reg &= ~(3 << 0);		// set the channel 3 as output
	*tim3CCM2_reg |= (1 << 3);		// set pre-load enable to avoid glitches when a new brightness command is received whilst LED is on

	*tim3CCM2_reg &= ~(7 << 4);		// Clear OC3M(Output Compare Mode) bits
	*tim3CCM2_reg |= (6 << 4);		// Set PWM Mode 1 where as long as CNT < CCR the signal stays high
}

static inline void ccm_config(void){
	*tim3CCEN_reg &= ~(1 << 0);
	*tim3CCEN_reg |= (1 << 0);		// Capture/Compare 1 output enable

	*tim3CCEN_reg &= ~(1 << 1);		// Polarity set to active high


	*tim3CCEN_reg &= ~(1 << 4);
	*tim3CCEN_reg |= (1 << 4);		// Capture/Compare 2 output enable

	*tim3CCEN_reg &= ~(1 << 5);		// Polarity set to active high

	*tim3CCEN_reg &= ~(1 << 8);
	*tim3CCEN_reg |= (1 << 8);		// Capture/Compare 3 output enable
	*tim3CCEN_reg &= ~(1 << 9);		// Polarity set to active high
}

void pwm_init(void){
	*rccEnableAPB1_reg |= (1 << 1);		// Enable TIM3 peripheral


	// the following config is used to generate a PWM signal of 1KHz
	*tim3PSC_reg = 0x53;			// Set PSC as 83
	*tim3ARR_reg = 0x3E7;			// Set ARR to 999; Controls frequency

	*tim3CR1_reg &= ~(1 << 4);		// Enable Up direction
	*tim3CR1_reg &= ~(3 << 5);		// Edge-aligned mode
	*tim3CR1_reg |= (1 << 7);		// Enable ARR preload

	ch1_config();
	ch2_config();
	ch3_config();

	ccm_config();

	*tim3CR1_reg |= (1 << 0);		// start counter
}

void pwm_update(){
	*tim3EG_reg |= (1 << 0);		// Update the registers
}

void pwm_set_duty(uint8_t channel, uint16_t duty){
	switch(channel){
	case red:
		*tim3CCR1_reg = duty;
		break;
	case green:
		*tim3CCR2_reg = duty;
		break;
	case blue:
		*tim3CCR3_reg = duty;
		break;
	default:
		break;
	}
}
