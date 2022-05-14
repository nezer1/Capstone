#include <mkl25z4.h>
#include <stdio.h>



void init_Ultrasonic();
void init_pin();
void init_PIT0();
void sleep_call();
void sleep_func();
void init_Servo();
void  measure();


#define MASK(X)  (1<<X)
#define Trig_pin		(12)		//PTA associated with TPM1 CH0		//PWM trigger
#define ECHO_pin		(13)		//PTA associated with TPM1 CH1   //input capture
#define WAKE_PIN		(4) //PTD LLWU pin

extern uint16_t g_dist;		//global variable to hold  distance.
extern uint8_t g_timer_done; //indicates end of 20s minute timer
extern uint8_t g_ultrasonic_done; //indicates if ultrasonic is working or not

extern uint8_t g_result_0;
extern uint8_t g_result_4;
extern uint8_t g_result_3;
extern uint8_t g_result_8;
extern uint8_t g_result_14;
extern uint8_t g_result_5;
extern uint8_t g_result_7;
extern uint8_t g_result_9;





//touch
#define PTE20 (20) //channel 0, head touch
#define PTE21 (21) //ADC input (channel 4a) , Back touch
#define PTE22 (22) //ADC input (channel 3), right hand
#define PTE23 (23) // ADC input (channel 7a), left hand
#define PTD1 (1) // ADC input (channel 5), left leg touch
#define PTB0 (0) // ADC input (channel 8), left side of face
#define PTC0 (0) // ADC input (channel 14), right leg touch
#define PTB1 (1) // ADC input (channel 9), right side of face

void init_ADC(void) ;
void init_PIT1(void);
uint8_t Calibration_ADC();
void Touch_processing();
