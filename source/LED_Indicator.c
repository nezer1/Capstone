#include "LED_Indicator.h"

void init_TPM0(){
	//Clock gate
		SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;	//TPM0
		//Select clock source in SIM_SOPT		//system clock
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

		SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTC_MASK;
		// selecting TPM 0 CH0
		PORTC->PCR[R1_pinA] &=~PORT_PCR_MUX_MASK;
		PORTC->PCR[R1_pinA] |=PORT_PCR_MUX(4);
		// selecting TPM 0 CH1
		PORTC->PCR[G1_pinB] &=~PORT_PCR_MUX_MASK;
		PORTC->PCR[G1_pinB] |=PORT_PCR_MUX(4);

		// selecting TPM 0 CH2
		PORTC->PCR[B1_pinC] &=~PORT_PCR_MUX_MASK;
		PORTC->PCR[B1_pinC] |=PORT_PCR_MUX(4);

		// selecting TPM 0 CH3
		PORTD->PCR[G2_pinB] &=~PORT_PCR_MUX_MASK;
		PORTD->PCR[G2_pinB] |=PORT_PCR_MUX(4);

		// selecting TPM 0 CH4
		PORTE->PCR[R2_pinA] &=~PORT_PCR_MUX_MASK;
		PORTE->PCR[R2_pinA] |=PORT_PCR_MUX(3);

		// selecting TPM 0 CH5
		PORTD->PCR[B2_pinC] &=~PORT_PCR_MUX_MASK;
		PORTD->PCR[B2_pinC] |=PORT_PCR_MUX(4);

		// Max value is 2^16-1
		//TPM0->CNT = 1638;
		TPM0->MOD= 1023;	//10ms = 1638.4
		//pre-scaler value, clear module interrupt flags,enable interrupts on module
		TPM0->SC |= TPM_SC_PS(7);  //TPM_SC_TOF_MASK|TPM_SC_TOIE_MASK;
		//up counting mode
		TPM0->SC |= TPM_SC_CPWMS(0);
		//edge-aligned high->low PWM
		TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);
		TPM0->CONTROLS[1].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);
		TPM0->CONTROLS[2].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);
		TPM0->CONTROLS[3].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);
		TPM0->CONTROLS[4].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);
		TPM0->CONTROLS[5].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);

		//intial duty cycle to keep PWM channels off
		TPM0->CONTROLS[0].CnV = 0;
		TPM0->CONTROLS[1].CnV = 0;
		TPM0->CONTROLS[2].CnV = 0;
		TPM0->CONTROLS[3].CnV = 0;
		TPM0->CONTROLS[4].CnV = 0;
		TPM0->CONTROLS[5].CnV = 0;

		TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

		//NVIC_EnableIRQ(TPM0_IRQn);
		//NVIC_ClearPendingIRQ(TPM0_IRQn);
		//NVIC_SetPriority(TPM0_IRQn, 3);
}




void Control_RGB_LED_1(uint16_t red_on, uint16_t green_on, uint16_t blue_on) {
		TPM0->CONTROLS[0].CnV= (red_on & 1023);

		TPM0->CONTROLS[1].CnV =  (green_on & 1023);

		TPM0->CONTROLS[2].CnV = (blue_on & 1023);
}


void Control_RGB_LED_2(uint16_t red_on, uint16_t green_on, uint16_t blue_on) {
		TPM0->CONTROLS[3].CnV= (red_on & 1023);

		TPM0->CONTROLS[4].CnV =  (green_on & 1023);

		TPM0->CONTROLS[5].CnV = (blue_on & 1023);
}
