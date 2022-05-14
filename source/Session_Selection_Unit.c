#include "Session_selection.h"
#include "Final.h"


void Init_session(){
	//GPIO
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK |SIM_SCGC5_PORTE_MASK;
	PORTD->PCR[RED_B] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[YELLOW_B] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[BUZZ_OUT] &= ~PORT_PCR_MUX_MASK;

	PORTD->PCR[RED_B] |= PORT_PCR_MUX(1)|PORT_PCR_IRQC(10)|PORT_PCR_PE(1);//falling edge interrupt;

	PORTD->PCR[YELLOW_B] |= PORT_PCR_MUX(1)|PORT_PCR_IRQC(10)|PORT_PCR_PE(1);//falling edge interrupt;

	PORTE->PCR[BUZZ_OUT] |= PORT_PCR_MUX(1);


	PTD->PDDR &= ~MASK(RED_B); //input
	PTD->PDDR &= ~MASK(YELLOW_B); //input

	PTE->PDDR &= ~MASK(BUZZ_OUT); //output
	PTE->PCOR |= MASK(BUZZ_OUT); //output
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_SetPriority(PORTD_IRQn,1);
	NVIC_EnableIRQ(PORTD_IRQn);

}




void PORTD_IRQHandler(){
	if(PORTD->ISFR & MASK(RED_B)){

		drawPart(1,1,eye,44,47);
		drawPart(81,1,eye,44,47);
		//state Robot = {Happy1,Blue,Up};
		PORTD->ISFR |= MASK(RED_B);

}
	if(PORTD->ISFR & MASK(YELLOW_B)){

			drawPart(1,1,sad_right_eye,44,47);
			drawPart(81,1,sad_left_eye,44,47);
			//state Robot = {Happy1,Blue,Up};
			PORTD->ISFR |= MASK(RED_B);

	}
}
