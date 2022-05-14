#include "Human_Touch.h"
#include "OLED_Face.h"
#include "Final.h"
#include "LED_Indicator.h"

uint16_t g_dist = 400;//400;		//global variable to hold  distance.
uint8_t g_timer_done = 0; //indicates end of 20s timer
uint8_t g_ultrasonic_done = 0 ; //indicates if ultrasonic is working or not

uint8_t g_result_0 = 0;
uint8_t g_result_4 = 0;
uint8_t g_result_3 = 0;
uint8_t g_result_8 = 0;
uint8_t g_result_14 = 0;
uint8_t g_result_5 = 0;
uint8_t g_result_7 = 0;
uint8_t g_result_9 = 0;

//state Robot = {Happy1,Blue,Up};
void init_pin(){
	//set up pin PTA12 for PWM (TPM1 CH0)
	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[Trig_pin] &=~PORT_PCR_MUX_MASK;
	PORTA->PCR[Trig_pin] |=PORT_PCR_MUX(3);

	//set up pin PTA13 for input capture (TPM1 CH1)
	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[ECHO_pin] &=~PORT_PCR_MUX_MASK;
	PORTA->PCR[ECHO_pin] |=PORT_PCR_MUX(3);

}


void init_Ultrasonic(){	//channel0 for signal generation,channel 1 for input capture
	//TPM1->SC |= TPM_SC_CMOD(0); //disable internal clock to run
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM1_MASK;	//*******TPM1 channel 0 and 1
	//Select clock source in SIM_SOPT		//system clock
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) ;

	TPM1->MOD = 8192;	//50ms =8192 or 0x2000

	//Channel 0  PWM: MSB-A==10 ELSB-A  10
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1)  ; //PWM output, interrupts not needed.
	TPM1->CONTROLS[0].CnV = 3 ;		//For trigger of 20us, CnV=3.28

	//input capture: MSB-A==00 ELSB-A  11
	TPM1->CONTROLS[1].CnSC |= TPM_CnSC_ELSA(1) |TPM_CnSC_ELSB(1)  ; //rising & falling edge
	TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK ;  //enable interrupts
	TPM1->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  ;
	TPM1->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_SetPriority(TPM1_IRQn, 2);
	NVIC_EnableIRQ(TPM1_IRQn);
}


void init_PIT0(){
	//Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	//Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; //enable mdis
	//PIT->MCR |= PIT_MCR_FRZ_MASK;

	//Initialize PIT0 to count down from starting_value
	PIT->CHANNEL[0].LDVAL =0X0C800000;   //every 20s
	//No chaining of timers
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;

	//PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;

	//Let the PIT channel generate interrupt requests
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	NVIC_SetPriority(PIT_IRQn, 1);
	//Clear any pending IRQ from PIT
	NVIC_ClearPendingIRQ(PIT_IRQn);
	//Enable the PIT interrupt in the NVIC
	NVIC_EnableIRQ(PIT_IRQn);
}

void sleep_call(){
	OLED_sleep();//OLED sleep mode

	Control_RGB_LED_1(0,0,0);
	Control_RGB_LED_2(0,0,0);
	//configure LLWU pins
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[WAKE_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[WAKE_PIN] |= PORT_PCR_MUX(1);
	PTD->PDDR &= ~MASK(WAKE_PIN);

	LLWU-> PE4 |= LLWU_PE4_WUPE14(1); //Rising edge trigger (make it level edged triggered)

	NVIC_EnableIRQ(LLW_IRQn);
	sleep_func();
}

void sleep_func(){
	volatile unsigned int dummyread;
	SMC->PMPROT |= SMC_PMPROT_ALLS_MASK; //allows MCU into LLS mode
	SMC->PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
	SMC->PMCTRL |=  SMC_PMCTRL_STOPM(0x3); //0b011 put MCU in Low Leakage stop mode(LLS)

	//wait for write to complete to SMC before stopping core to ensure pin filter is initialized
	 dummyread = SMC->PMCTRL;
	 dummyread++;

	 SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	 __WFI(); //entry into LLS mode
}


//------------Touch------------------------------------------------------------------------------
uint8_t Calibration_ADC(){
	uint16_t cal_var;
	ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK; //software conversion trigger
	ADC0->SC3 &= (~ADC_SC3_ADCO_MASK & ~ ADC_SC3_AVGS_MASK); //single converson, clear average function bitfield
	ADC0->SC3 |= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(0)); // turn averaging on, set max average sample of 32
	ADC0->CFG1 |= ADC_CFG1_ADICLK(1); //BUSclock/2-> input clock
	ADC0->CFG1 |= ADC_CFG1_ADIV(2); //Input clock/2 ,approx 5MHz

	ADC0->SC3 |= ADC_SC3_CAL_MASK; //start calibration
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

	if((ADC0->SC3 & ADC_SC3_CALF_MASK)){ //if fail, value will be non zero
		return(1);
	}
	cal_var = 0x00;

	cal_var = ADC0->CLP0;
	cal_var += ADC0->CLP1;
	cal_var += ADC0->CLP2;
	cal_var += ADC0->CLP3;
	cal_var += ADC0->CLP4;
	cal_var += ADC0->CLPS;

	cal_var = cal_var/2;
	cal_var |= 0x8000; //Set MSB

	ADC0->PG = ADC_PG_PG(cal_var); //overall conversion in single ended mode

	ADC0->SC3 &= ~ADC_SC3_CAL_MASK;// clear CAL bit

	return(0); //calibration worked
}


void init_ADC(void) {
	//clock gate
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTB_MASK |SIM_SCGC5_PORTD_MASK |SIM_SCGC5_PORTC_MASK;

	// Select analog for pin channel 0
	PORTE->PCR[PTE20] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE20] |= PORT_PCR_MUX(0);

	// Select analog for pin channel 4a
	PORTE->PCR[PTE21] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE21] |= PORT_PCR_MUX(0);

	//channel 3
	PORTE->PCR[PTE22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE22] |= PORT_PCR_MUX(0);

	//channel 7a
	PORTE->PCR[PTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE23] |= PORT_PCR_MUX(0);

	//channel 5
	PORTD->PCR[PTD1] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD1] |= PORT_PCR_MUX(0);


	//channel 8
	PORTB->PCR[PTB0] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0] |= PORT_PCR_MUX(0);

	//channel 9
	PORTB->PCR[PTB1] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1] |= PORT_PCR_MUX(0);

	//channel 14
	PORTC->PCR[PTC0] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PTC0] |= PORT_PCR_MUX(0);


	if(!(Calibration_ADC())){ //returns 0 for success

	ADC0->CFG1 |= ADC_CFG1_ADICLK(0); //BUSclock -> input clock
	ADC0->CFG1 |= ADC_CFG1_ADIV(0); //Input clock/1 ,approx 21Mhz

	ADC0->CFG1 |= ADC_CFG1_ADLPC_MASK | ADC_CFG1_MODE(0); //low power , 8 bit single ended conversion
	ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK; //software conversion trigger

	ADC0->SC3 &= ~ADC_SC3_ADCO_MASK ;
	//16 bit single-ended conversion, when DIFF=0
	//ADC0->CFG1 |= ADC_CFG1_MODE(0);		//0 is 8-bit conversion, 3=16bit,
	// Default settings: Software trigger, voltage references VREFH and VREFL
}
}

void init_PIT1(){
	//Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	//Enable module
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; //enable mdis

	//Initialize PIT1 to count down from starting_value
	PIT->CHANNEL[1].LDVAL = 52428;   //every 5ms approx
	//No chaining of timers
	//PIT->CHANNEL[1].TCTRL &= PIT_TCTRL_CHN_MASK;

	PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
	//Let the PIT channel generate interrupt requests
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;

	//PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK; //enable timer

	NVIC_SetPriority(PIT_IRQn, 1);
	//Clear any pending IRQ from PIT
	NVIC_ClearPendingIRQ(PIT_IRQn);
	//Enable the PIT interrupt in the NVIC
	NVIC_EnableIRQ(PIT_IRQn);
}
/*
void Touch_processing(state *t){
	if(g_result_4 > 0xAE && t->face==Neutral){
		drawfaceRGB(0,0,neutral,127,119);
	}
}
*/
//*************************HANDLERS********************************************************************************

void LLWU_IRQHandler(){
	LLWU->F2 |= LLWU_F2_WUF14_MASK; //clear wakeup source flag
	PMC->REGSC |= PMC_REGSC_BGBE_MASK;//acknowledge wake up event
	Wake_up();//OLED wake up routine
	TPM1->SC |= TPM_SC_CMOD(1); //start ultrasonic detection
	sleep_mode_tracker = 0;
	PIT->CHANNEL[0].LDVAL =0X0C800000; //20s timer
}

void PIT_IRQHandler(){

	// channel 0 triggered interrupt
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		if(sleep_mode_tracker ==0){

		//Clear interrupt request flag for channel
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;

		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK; //disable 1 minute timer
		//TPM1->SC |= TPM_SC_CMOD(1); //enable TPM clock to run

		g_timer_done = 1; //20 seconds  timer ended
		sleep_mode_tracker = 1;
		PIT->CHANNEL[1].LDVAL = 55000;
		//printf("20 secs -----20secs-----20 secsssss???");
		}
		else{
			sleep_call();
		}
	}

	//Touch, channel 1 triggered interrupt
	if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {

		// channel 1 triggered interrupt
		enum Selector_states {S1,S2,S3,S4,S5,S6,S7,S8};
		static enum Selector_states next_state = S1;

		switch(next_state){
		case S1:
			ADC0->SC1[0] = ADC_SC1_ADCH(0)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_0 = ADC0->R[0];
			if(g_result_0 > 0xAE){
				(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
			}
			//printf("%X ,chan 0\n", g_result_0);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S2;

			break;
		case S2:
			ADC0->SC1[0] = ADC_SC1_ADCH(4)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_4 = ADC0->R[0];
			if(g_result_4 > 0xAE){
				(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
			}
			//printf("%X,channel 4\n", g_result_4);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S3;
			break;
		case S3:
			ADC0->SC1[0] = ADC_SC1_ADCH(3)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_3 = ADC0->R[0];
			if(g_result_3 > 0xAE){
				(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
				}
			//printf("%X chan 3 \n ", g_result_3);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S4;
			break;

		case S4:
			ADC0->SC1[0] = ADC_SC1_ADCH(8)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_8 = ADC0->R[0];
			if(g_result_8 > 0xAE){
				(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
						}
			//printf("%X chan 8\n", g_result_8);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S5;
			break;
		case S5:
			ADC0->SC1[0] = ADC_SC1_ADCH(14)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_14 = ADC0->R[0];
			if(g_result_14> 0xAE){
			(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
			}
			//printf("%X chan 14\n", g_result_14);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S6;
			break;

		case S6:
			ADC0->SC1[0] = ADC_SC1_ADCH(5)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_5 = ADC0->R[0];
			if(g_result_5 > 0xAE){
				(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
				}

			//printf("%X chan 5\n", g_result_5);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S7;
			break;
		case S7:
			ADC0->SC1[0] = ADC_SC1_ADCH(7)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_7 = ADC0->R[0];
			if(g_result_7 > 0xAE){
				(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
				}
			//printf("%X chan 7\n", g_result_7);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S8;
			break;

		case S8:
			ADC0->SC1[0] = ADC_SC1_ADCH(9)  | ADC_SC1_COCO_MASK;//
			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			g_result_9 = ADC0->R[0];
			if(g_result_9 > 0xAE){
			(PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK);
			}
			//printf("%X chan 7\n", g_result_9);
			PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK; //w1c
			next_state = S1;
			break;
	}
	}

}



void TPM1_IRQHandler(){

	static int ctr=0;
	static unsigned int previous=0;
	unsigned int current=0;
	static unsigned int interval=0;


	if (TPM1->STATUS & TPM_STATUS_CH1F_MASK){		//any input capture?
		current=TPM1->CONTROLS[1].CnV;

		current |= (ctr <<16);// add the no. of overflows. Each ctr tick is 2^16
		interval = current-previous; //pulse width or duration
		previous=current; //can be rising edge or falling edge , its unsigned so if previous = falling, current - previous will not be negative
		TPM1->CONTROLS[1].CnSC |=TPM_CnSC_CHF_MASK;  	//clear flag
			}
	if (TPM1->SC & TPM_SC_TOF_MASK){
		ctr++;		//a timer overflow occurred.
		TPM1->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
		//g_dist= interval*1.047;
		//printf("%d \n",g_dist);
		if(!(ctr % 11)){
			g_dist= interval*1.047;
			printf("distance in mm=%d\n", g_dist);
}
	if(g_dist<= 300){
		TPM1->SC &= ~TPM_SC_CMOD_MASK; //human detected hence ultrasonic off
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; //start 20s countdown
		PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK; //start touch detection
		g_ultrasonic_done = 1;
		}
	}
}



