#include <stdint.h>
#include <gpio.h>


static void gpio_debug_led_init(void);		// Configure the LED pin
static void gpio_uart1_init(void);			// Configure GPIO pins for USART1
static void gpio_tim3_init(void);			// Configure GPIO pins for TIM3

static uint32_t volatile *const rccEnableAHB_reg = (uint32_t volatile*)0x40023830;		// To Enable the AHB1

static uint32_t volatile *const PortAMode_reg = (uint32_t volatile*)0x40020000;			// To configure the mode of GPIOA
static uint32_t volatile *const PortASpeed_reg = (uint32_t volatile*)0x40020008;		// To configure the speed of GPIOA pins
static uint32_t volatile *const PortAPUPD_reg = (uint32_t volatile*)0x4002000C;			// To configure the GPIOA pins to be Pulled Up/Down
static uint32_t volatile *const PortAaFH_reg = (uint32_t volatile*)0x40020024;			// Set AF on GPIOAs 8-15
static uint32_t volatile *const PortAOpData_reg = (uint32_t volatile*)0x40020014;		// To set the output value to GPIOA pins


static uint32_t volatile *const PortCMode_reg = (uint32_t volatile*)0x40020800;			// To configure the mode of GPIOC
static uint32_t volatile *const PortCoType_reg = (uint32_t volatile*)0x40020804;			// To configure the mode of GPIOC
static uint32_t volatile *const PortCSpeed_reg = (uint32_t volatile*)0x40020808;		// To congigure the speed of GPIOC pins
static uint32_t volatile *const PortCPUPD_reg = (uint32_t volatile*)0x4002080C;
static uint32_t volatile *const PortCaFL_reg = (uint32_t volatile*)0x40020820;			// Set AF on GPIOCs 1-7
static uint32_t volatile *const PortCaFH_reg = (uint32_t volatile*)0x40020824;			// Set AF on GPIOCs 8-14


void gpio_init(void){
	/*(Both GPIOA and C ports share the same AHB1 clock)*/

	// Enable the RCC clock for GPIOA 
	*rccEnableAHB_reg |= (1 << 0);

	// now move ahead with debug LED and USART1 Pins configuration
	gpio_debug_led_init();			// GPIO A
	gpio_uart1_init();				// GPIO A

	/* Enable the RCC clock for GPIOC */
	*rccEnableAHB_reg |= (1 << 2);
	gpio_tim3_init();

}

void gpio_debug_led_toggle(){
	*PortAOpData_reg ^= (1 << 6);	// Toggle the LED
}


static void gpio_debug_led_init(void){
	/* Configure the mode of the GPIOA pin as Output */
	*PortAMode_reg &= ~(3<< 12);	// Clear the 12 and 13 bits first
	*PortAMode_reg |= (1 << 12);	// Set the bit 12 to enable general purpose output mode

	// Set default state of LED to be OFF
	*PortAOpData_reg |= (1 << 6);	// Active Low GPIO pin
}

static void gpio_uart1_init(void){
	/* Configure the mode of the IO pin as Output */
	*PortAMode_reg &= ~((3 << 18) | (3 << 20));		// Clear the 18 and 19 bits first for Pin 9 and 20 and 21 for Pin 10
	*PortAMode_reg |= ((2 << 18) | (2 << 20));		// Set the bit 9 and 10 to AF mode

	*PortASpeed_reg &= ~((3 << 18) | (3 << 20));	// Clear bits 18:20 for GPIO 9 and 10
	*PortASpeed_reg |= ((2 << 18) | (2 << 20));		// Set GPIO 9 and 10 to be high speed pins to accomodate for higher baud rate 115200

	*PortAPUPD_reg &= ~(3 << 20);					// Clear bits for GPIO 10
	*PortAPUPD_reg |= (1 << 20);					// Set GPIO 10 to 1 to pull it up to avoid noise when idle

	*PortAaFH_reg &= ~((7 << 4) | (7 << 8));		// Clear AF7 for GPIO 9 and 10
	*PortAaFH_reg |= ((7 << 4) | (7 << 8));			// Set AF7 for GPIO 9 and 10

}

static inline void tim3_ch1_init(void){
	/* Configure the mode of the IO pin as Output */
	*PortCMode_reg &= ~(3 << 12);			// Clear 12 and 13 bits for GPIOC-6
	*PortCMode_reg |= (2 << 12);			// Set to Alternate Function Mode

	*PortCoType_reg &= ~(1 << 6);			// Output Push-pull config(reset state)

	*PortCSpeed_reg &= ~(3 << 12);			// Clear bits 12:13 for GPIOC-6
	*PortCSpeed_reg |= (2 << 12);			// Set High speed for GPIOC-6

	*PortCPUPD_reg &= ~(3 << 12);			// Clear bits 12:13 for GPIO 6 for no push-pull

	*PortCaFL_reg &= ~(15 << 24);			// Clear bits 24:27
	*PortCaFL_reg |= (2 << 24);				// AF2(TIM3) on GPIOC-6

}
static inline void tim3_ch2_init(void){
	/* Configure the mode of the IO pin as Output */
	*PortCMode_reg &= ~(3 << 14);			// Clear 14 and 15 bits for GPIOC-7
	*PortCMode_reg |= (2 << 14);			// Set to Alternate Function Mode

	*PortCoType_reg &= ~(1 << 7);			// Output Push-pull config(reset state)

	*PortCSpeed_reg &= ~(3 << 14);			// Clear bits 14:15 for GPIOC-7
	*PortCSpeed_reg |= (2 << 14);			// Set High speed for GPIOC-7

	*PortCPUPD_reg &= ~(3 << 14);			// Clear bits 14:15 for GPIOC-7 for no push-pull

	*PortCaFL_reg &= ~(15 << 28);			// Clear bits 28:31
	*PortCaFL_reg |= (2 << 28);				// AF2(TIM3) on GPIOC-7
}
static inline void tim3_ch3_init(void){
	/* Configure the mode of the IO pin as Output */
	*PortCMode_reg &= ~(3 << 16);			// Clear 16 and 17 bits for GPIOC-8
	*PortCMode_reg |= (2 << 16);			// Set to Alternate Function Mode

	*PortCoType_reg &= ~(1 << 8);			// Output Push-pull config(reset state)

	*PortCSpeed_reg &= ~(3 << 16);			// Clear bits 16:17 for GPIOC-8
	*PortCSpeed_reg |= (2 << 16);			// Set High speed for GPIOC-8

	*PortCPUPD_reg &= ~(3 << 16);			// Clear bits 16:17 for GPIOC-8 for no push-pull

	*PortCaFH_reg &= ~(15 << 0);			// Clear bits 3:0
	*PortCaFH_reg |= (2 << 0);				// AF2(TIM3) on GPIOC-8

}

static void gpio_tim3_init(void){
	tim3_ch1_init();			// configure PC6 for channel 1
	tim3_ch2_init();			// configure PC7 for channel 2
	tim3_ch3_init();			// configure PC8 for channel 3
}




