#ifndef PWM_H
#define PWM_H

#define red   	1
#define green   2
#define blue   	3


void pwm_init(void);									// initialise PWM peripheral
void pwm_set_duty(uint8_t channel, uint16_t duty);		// core control function
uint16_t pwm_get_duty(uint8_t channel);					// optional but useful for debugging
void pwm_set_frequency(uint32_t freq_hz);
void pwm_update();

// Turn PWM outputs on/off without deconfiguring timer, for later
void pwm_enable(void);
void pwm_disable(void);


#endif	//PWM_H
