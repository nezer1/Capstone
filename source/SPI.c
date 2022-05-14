#include "SPI.h"

uint8_t SPI_send(uint8_t d_out){
	uint8_t val;
	while(!(SPI0->S & SPI_S_SPTEF_MASK));
	SPI0->D = d_out;

	while(!(SPI0->S & SPI_S_SPRF_MASK));
	val = SPI0->D;
	return val;
}


uint8_t SPI_receive(){
	uint8_t val;
	while(!(SPI0->S & SPI_S_SPTEF_MASK));
	SPI0->D = 0xff; //clocks signals needed to receive

	while(!(SPI0->S & SPI_S_SPRF_MASK));
	val = SPI0->D;
	return val;
}


void Init_SPI0(){
	SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK|SIM_SCGC5_PORTB_MASK|SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTC_MASK;
		// Set ports to outputs
		//PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
		//PTD->PDDR |= MASK(BLUE_LED_POS);
	//disable SPI1 to allow configuration
	SPI0->C1 &= ~SPI_C1_SPE_MASK;

	//set PTC5 as SPI0_SCK -- ALT2
	PORTC->PCR[SCK_SD] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[SCK_SD] |= PORT_PCR_MUX(2);
	//set PTC6 as SPI0_MOSI
	PORTC->PCR[MOSI_SD] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[MOSI_SD] |= PORT_PCR_MUX(2);
	//set PTC7 as SPI0_MISO
	PORTC->PCR[MISO_SD] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[MISO_SD] |= PORT_PCR_MUX(2);
	//set PTC4 as SPI0_chip_select
	PORTC->PCR[CS_SD] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[CS_SD] |= PORT_PCR_MUX(2);
	//PTC->PDDR |= MASK(CS); //output

	//Master mode selected ,enable SS output (not controlled by SPI when MODFEN = 0)
	SPI0->C1 |= SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;
	SPI0->C2 |= SPI_C2_MODFEN(1);

	//Select active high clock(CPOL=0), first edge sample(CPHA=0)
	SPI0->C1 &= ~SPI_C1_CPHA_MASK;
	SPI0->C1 &= ~SPI_C1_CPOL_MASK;

	//BaudRate = Busclock/ ((SPPR+1)*2^(SPR1+1)) , By default Busclock= System clock/2
	SPI0->BR = SPI_BR_SPPR(6)|SPI_BR_SPR(1); // 10,485,760/(7*2^2)

	//enable SPI1
	SPI0->C1 |= SPI_C1_SPE_MASK;
}

