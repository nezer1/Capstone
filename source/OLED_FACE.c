#include <MKL25Z4.H>
#include <stdint.h>

#include "OLED_Face.h"



//By default LSBFE is set to 0 -> MSB first
void Init_SPI1(){
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK|SIM_SCGC5_PORTB_MASK|SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTA_MASK;



	//disable SPI1 to allow configuration
	SPI1->C1 &= ~SPI_C1_SPE_MASK;

	//set PTE2 as SPI1_SCK -- ALT2
	PORTE->PCR[SCK] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SCK] |= PORT_PCR_MUX(2);
	//set PTE3 as SPI1_MOSI
	PORTE->PCR[MOSI] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[MOSI] |= PORT_PCR_MUX(5);
	//set PTE1 as SPI1_MISO
	PORTE->PCR[MISO] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[MISO] |= PORT_PCR_MUX(5);
	//set PTE4 as SPI1_chip_select
	PORTE->PCR[CS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[CS] |= PORT_PCR_MUX(2);

	//Data,command pin
	PORTE->PCR[DC] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[DC] |= PORT_PCR_MUX(1);
	PTE->PDDR |= MASK(DC); //output

	//Reset pin
	PORTA->PCR[RS] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RS] |= PORT_PCR_MUX(1);
	PTA->PDDR |= MASK(RS); //output
	PTA->PCOR = MASK(RS); //reset activated

	//Master mode selected ,SS output (MODFEN = 1)
	SPI1->C1 = SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;
	SPI1->C2 = SPI_C2_MODFEN(1);

	//Select active high clock(CPOL=0), first edge sample(CPHA=0)
	SPI1->C1 &= ~SPI_C1_CPHA_MASK;
	SPI1->C1 &= ~SPI_C1_CPOL_MASK;

	//BaudRate = Busclock/ ((SPPR+1)*2^(SPR1+1)) , By default Busclock= System clock/2
	SPI1->BR = SPI_BR_SPPR(1) | SPI_BR_SPR(1); // 10,485,760/(2*2^2)

	//SPI1->C2 |= SPI_C2_TXDMAE_MASK; //enable transmit by  DMA

	//SPI1->C1 = SPI_C1_SPTIE_MASK; //INTERRUPT enable for transmit flag

	//CONFIGURE NVIC for SPI1 ISR
	NVIC_SetPriority(SPI1_IRQn,2);
	NVIC_ClearPendingIRQ(SPI1_IRQn);
	NVIC_EnableIRQ(SPI1_IRQn);

	//enable SPI1
	SPI1->C1 |= SPI_C1_SPE_MASK;
}


void Init_OLED(){
    //PTE->PCOR = MASK(RS); //reset
	//delay_ms(2);
	PTE->PSOR = MASK(RS); //reset deativated
	//delay_ms(2);
	write_command(CMD_COMMANDLOCK);
	write_data(0x12); // Unlock OLED driver IC MCU interface from entering command
	write_command(CMD_COMMANDLOCK);
	write_data(0xB1); // Command A2,B1,B3,BB,BE,C1 accessible if in unlock state
	write_command(CMD_DISPLAYOFF);
	write_command(CMD_CLOCKDIV);
	write_data(0xF1); //7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	write_command(CMD_MUXRATIO);
	write_data(SCREEN_WIDTH-1);//multiplex ratio = 128
	write_command(CMD_DISPLAYOFFSET);
	write_data(0x00);
	write_command(CMD_SETGPIO);
	write_data(0x00); //GPIO disabled
	write_command(CMD_FUNCTIONSELECT);
	write_data(0x01); //internal diode enabled for voltage regulation
	write_command(CMD_PRECHARGE);
	write_data(0x32);
	write_command(CMD_VCOMH);
	write_data(0x05);
	write_command(CMD_NORMALDISPLAY); //normal display mode
	write_command(CMD_CONTRASTABC);
	write_data(0xC8);
	write_data(0x80);
	write_data(0xC8);
	write_command(CMD_CONTRASTMASTER);
	write_data(0x0F);
	write_command(CMD_SETVSL);
	write_data(0xA0);
	write_data(0xB5);
	write_data(0x55);
	write_command(CMD_PRECHARGE2);
	write_data(0x01);
	write_command(DISPLAYON);
}

void write_command(uint8_t command){
	PTE->PCOR = MASK(DC); //Data/command pin low
	SPI_send_receive(command);
}


void write_16_data(uint16_t data){
	uint8_t high = data >> 8, low =data;
	PTE->PSOR = MASK(DC); //Data/command pin high
	SPI_send_receive(high);
	SPI_send_receive(low);
}


void write_data(uint8_t data ){
	PTE->PSOR = MASK(DC); //Data/command pin high
	SPI_send_receive(data);
}

void on(){
	write_command(CMD_DISPLAYON);
}

void off(){
	write_command(CMD_DISPLAYOFF);
}

void OLED_sleep(){
	write_command(CMD_DISPLAYOFF);
	write_command(CMD_FUNCTIONSELECT);
	write_data(0x00); //disable diode sleep mode
}

void Wake_up(){
	write_command(CMD_FUNCTIONSELECT);
	write_data(0x01); //enable diode wake up
	delay_ms(1); //wait for voltage to be stable
	write_command(CMD_DISPLAYON);
}

uint8_t SPI_send_receive(uint8_t d_out){
	//PTE->PCOR = MASK(CS);
	while(!(SPI1->S & SPI_S_SPTEF_MASK)); //wait for transmit buffer to be empty (SPTEF =1 when transmit buffer empty)
	SPI1->D = d_out; //access transmit buffer


	while(!(SPI1->S & SPI_S_SPRF_MASK));
	return SPI1->D;
}


//Graphics functions
void setOrientation(uint8_t r){
	  // 6,7 Color depth (01 = 65K)
	  // 5   Odd/even split COM (0: disable, 1: enable)
	  // 4   Scan direction (0: top-down, 1: bottom-up)
	  // 3   Reserved
	  // 2   Color remap (0: A->B->C, 1: C->B->A)
	  // 1   Column remap (0: 0-127, 1: 127-0)
	  // 0   Address increment (0: horizontal, 1: vertical)
	uint8_t RAM_disp = 0b01100100; //65K, Enable split, ABC
	switch (r) {
	case 0:
		RAM_disp |=  0b00010000; // Scan bottom-up
		break;
	case 1:
		RAM_disp |=  0b00010011; // Scan bottom-up, column remap 127-0, vertical
		break;
	case 2:
		RAM_disp |=  0b00000010; // column remap 127-0
		break;
	case 3:
		RAM_disp |=  0b00000001; // vertical
		break;
	}
	write_command(CMD_SETREMAP);
	write_data(RAM_disp);

	uint8_t startpoint = (r < 2)? 127 : 0;
	write_command(CMD_STARTLINE); //sets starting address of display ram
	write_data(startpoint);
}


void set_Disp_Window(uint16_t x1, uint16_t y1,uint16_t w, uint16_t h ){
	uint16_t x2 = x1 + w - 1, y2 = y1 + h - 1;// already have first point so substract 1
	write_command(CMD_SETCOLUMN); //X range
	write_data(x1);
	write_data(x2);
	write_command(CMD_SETROW); //Y range
	write_data(y1);
	write_data(y2);
	write_command(CMD_WRITERAM); // Begin write to graphic RAM
}

void drawPixel(uint16_t x1, uint16_t y1, uint16_t color){
	if ((x1 >= 0) && (x1 < SCREEN_WIDTH ) && (y1 >= 0) && (y1 < SCREEN_HEIGHT)) {
	set_Disp_Window(x1,y1,1,1);
	write_16_data(color);
}
}

void drawLine(uint16_t x1, uint16_t y1, uint16_t color,uint16_t w, uint16_t h ){
	if ((x1 >= 0) && (x1 < SCREEN_WIDTH ) && (y1 >= 0) && (y1 < SCREEN_HEIGHT)) {
	set_Disp_Window(x1,y1,w,h);
	while(x1<128*128){
	write_16_data(color);
	x1++;
}
}
}

void drawLine1(uint16_t x1, uint16_t y1, const uint16_t buf[],uint16_t w, uint16_t h){
	if ((x1 >= 0) && (x1 < SCREEN_WIDTH ) && (y1 >= 0) && (y1 < SCREEN_HEIGHT)) {
	set_Disp_Window(x1,y1,w,h);
	while(x1<1600){
	write_16_data(buf[x1]);
	x1++;
}
}
}

void drawpixels(const uint16_t color[],uint32_t len){
	while(len--){
		write_16_data(*color++);
	}
}

void drawfaceRGB(uint16_t x1,uint16_t y1,const uint16_t *colors,uint16_t w, uint16_t h){
	off();
	drawLine(0, 0,BLACK,128,128);
	set_Disp_Window(x1, y1, w, h);
	while(h--){ //going through all rows
		drawpixels(colors,w); //writing to all columns in a row equivalent bitmap
		colors += w; //advance pointer for next row by display width provided
	}
	on();
	}


void drawPart(uint16_t x1,uint16_t y1,const uint16_t *colors,uint16_t w, uint16_t h){
	set_Disp_Window(x1, y1, w, h);
	while(h--){ //going through all rows
		drawpixels(colors,w); //writing to all columns in a row equivalent bitmap
		colors += w; //advance pointer for next row by display width provided
		}
}


/*
void DMA_drawfaceRGB(uint16_t x1,uint16_t y1,const uint16_t *colors,uint16_t w, uint16_t h){
		drawLine(0, 0,BLACK,128,128);
		set_Disp_Window(x1, y1, w, h);
		DMA_write_16_data( *colors,25400);

}

*/





/*
void drawFace(int8_t x1, int8_t y1,const uint16_t bitmap[2304]){
	drawLine(0,0,BLACK);
	for(int8_t row = 0; row < 145; row++){
		for(int8_t col=0; col< 16;col++){
			drawPixel(x1,y1,bitmap[row *col]);
			}
		}
}
*/




//--------------------Handlers----------------------------------------------------------------------------------
/*
void PORTD_IRQHandler(void){
	PTD->PTOR |= MASK(BLUE_LED_POS);
	if(PORTD->ISFR & MASK(OFF_PIN)){
	write_command(CMD_DISPLAYOFF);
	PORTD->ISFR |= MASK(OFF_PIN); //clear flags
	}

	if(PORTD->ISFR & MASK(ON_PIN)){
		write_command(CMD_DISPLAYON);
		PORTD->ISFR |= MASK(ON_PIN); //clear flags
	}

}
*/
/*
void SPI1_IRQHandler(){
	if(SPI1->S & SPI_S_SPTEF_MASK){
		SPI1->D = d_out;
	}

	if(SPI1->S & SPI_S_SPRF_MASK){
		return SPI1->D ;
	}
}

*/

void delay_ms(int t_ms){
	for (int i=0; i<t_ms; i++)
		for (int j=0; j<48000;j++);
}



