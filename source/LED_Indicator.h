#include <mkl25z4.h>
#include <stdio.h>


#define R1_pinA (1)  //PTC associated with TPM0 CH0		//PWM //right side
#define G1_pinB (2)  //PTC associated with TPM0 CH1   //PWM //right side
#define B1_pinC (3)  //PTC associated with TPM0 CH2		//PWM

#define R2_pinA (31)  //PTE associated with TPM0 CH4		//PWM
#define G2_pinB (3)  //PTD associated with TPM0 CH3   //PWM
#define B2_pinC (5)  //PTD associated with TPM0 CH5		//PWM

void init_TPM0();
void Control_RGB_LED_1(uint16_t red_on, uint16_t green_on, uint16_t blue_on);
void Control_RGB_LED_2(uint16_t red_on, uint16_t green_on, uint16_t blue_on);
