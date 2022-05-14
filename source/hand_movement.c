#include "hand_movement.h"
#include "Final.h"

void init_TPM2()
{
	//set up left hand, TPM2_CH0
	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[M1] &=~PORT_PCR_MUX_MASK;
	PORTA->PCR[M1] |=PORT_PCR_MUX(3);

	//set up right hand (TPM2_CH1)
	PORTA->PCR[M2] &=~PORT_PCR_MUX_MASK;
	PORTA->PCR[M2] |=PORT_PCR_MUX(3);

	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM2_MASK;
	//Select clock source
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	//Configure registers
	TPM2->MOD= 0xCCCD;  //20ms or 50Hz period
	//up counting mode
	//TPM2->SC |= TPM_SC_CPWMS(0);
	//PWM set up
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM2->CONTROLS[1].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;

	TPM2->CONTROLS[0].CnV = 0; //(1ms) 0xA3D (1.5ms) 0xF5C  (2ms)
	TPM2->CONTROLS[1].CnV =0; //(1ms) 0xA3D (1.5ms) 0xF5C  (2ms)

	TPM2->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(3)| TPM_SC_TOIE_MASK ;

	//TPM2->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_SetPriority(TPM2_IRQn, 3);
	NVIC_EnableIRQ(TPM2_IRQn);
}

void TPM2_IRQHandler(){
	//static enum stages { s1,s2,s3}next_stage = Robot.face;
	//static uint8_t cnt = 0;
	if (TPM2->SC & TPM_SC_TOF_MASK){
		switch(Robot.face){

		case Happy1:
			TPM2->CONTROLS[0].CnV = 0xF5C; //(1ms)
			TPM2->CONTROLS[1].CnV = 0xF5C; //(1ms)

			//TPM2->CONTROLS[0].CnV = 0x53D; //(1ms)
			//TPM2->CONTROLS[1].CnV = 0x53D; //(1ms)

			//delay(3);
			//TPM2->SC &= ~TPM_SC_CMOD_MASK;
			break;
		case Happy2:
			TPM2->CONTROLS[0].CnV = 0x53D; //(1ms)
			TPM2->CONTROLS[1].CnV = 0x53D; //(1ms)
			TPM2->SC &= ~TPM_SC_CMOD_MASK;
			break;
		case Sad:
			TPM2->CONTROLS[0].CnV = 0x1A00;
			TPM2->CONTROLS[0].CnV = 0x1A00;
			TPM2->SC &= ~TPM_SC_CMOD_MASK;
			break;
		case Neutral:
			TPM2->CONTROLS[0].CnV = 0xF5C;
			TPM2->CONTROLS[0].CnV = 0xF5C;
			TPM2->SC &= ~TPM_SC_CMOD_MASK;
			break;
		case Angry:
			TPM2->CONTROLS[0].CnV = 0xF5C;
			TPM2->CONTROLS[0].CnV = 0xF5C;
			TPM2->SC &= ~TPM_SC_CMOD_MASK;
			break;
		}
		TPM2->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
	}
}



void start_motor( )
{
	//TPM2->CONTROLS[0].CnV = motor1; //(1ms) 0xA3D (1.5ms) 0xF5C  (2ms)
	//TPM2->CONTROLS[1].CnV = motor2; //(1ms) 0xA3D (1.5ms) 0xF5C  (2ms)

	TPM2->SC |= TPM_SC_CMOD(1); //enable internal clock to run

}

void delay(int t_ms){
	for (int i=0; i<t_ms; i++)
		for (int j=0; j<48000;j++);
}

