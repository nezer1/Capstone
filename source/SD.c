#include "SD.h"
#include <stdio.h>

uint8_t SD_Type = 0;

uint8_t MicroSDcard_command(uint8_t cmd,uint32_t arg, uint8_t crc){
	uint16_t cnt = 512;
	uint8_t SD_status;

	SPI_send(0xFF);

	//PTC->PCOR |= MASK(CS);
	//if(Wait_Read()){
		//return 0xff; //not successful
	//}


	//send command
	SPI_send(cmd | 0x40);// start bit is always 01 for a command frame (8 bits)

	//send argument
	for (int8_t s = 24; s >= 0; s -= 8) {
	    SPI_send(arg >> s); //break down into bytes (32 bits)
	  }

	//send CRC (7 bits, last bit is stop bit)
//	uint8_t crc = 0XFF;
//	if (cmd == CMD0){
//	    crc = 0X95;  // correct crc for CMD0 with arg 0
//	  }
//	if (cmd == CMD8) {
//	    crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
//	  }
//	if (cmd == 0x49) {
//	  	    crc = 0X95;  // correct crc
//	  	  }
	  SPI_send(crc);


	  do{
		 SD_status = SPI_send(0xff);
		 cnt--;
	  }while((cnt)&&(SD_status == 0xff));

	  //wait for response, duration of 256 bits count down
	  // for (uint8_t i = 0; ((MICRO_SD_status = SPI_receive()) & 0X80) && i != 0XFF; i++);

	   return SD_status;
}

uint8_t Wait_Read(){
	uint32_t cnt = 0x00fffff;
	uint8_t SD_status;

	do{
		SD_status = SPI_send(0xff);
		if(SD_status == 0xff){
			return 0;
		}
		cnt--;
	}while(cnt);

	return 1; //not sucessful
}


uint8_t init_MicroSDcard(){
	uint8_t status = 0, k = 0;
	uint16_t cnt = 0;
	uint8_t  buff[512];
	//Put SD card in SPI mode (reset)

	//supply min of 74 clock cycles with CS high
	for (uint8_t i = 0; i < 0x0F; i++) {
	    SPI_send(0XFF);//MOSI pin must be high during this period to indicate no message being sent
	  }

	// command to go idle in SPI mode
	do{
		status = MicroSDcard_command(CMD0,0x00,0x95); // IDLE state, triggers SD card to use SPI interface
		k++;
	}while((status != 1)&& (k < 200));

	if(k==0){
		SPI_send(0XFF);
		printf("\n SD reset fail");
		return 1;
	}

	status = MicroSDcard_command(CMD8,0x1AA,0x87);
	printf("SD_CMD8 return %d....\n\n",status);

	if(status == 1){
		cnt = 0xffff;

	do{
		MicroSDcard_command(CMD55,0,0xff);
		status = MicroSDcard_command (ACMD41,0x40000000,0xff);
		cnt--;
	}while((status)&& (cnt));


	status = MicroSDcard_command(CMD58,0,0);

	if(status != 0x00){
		printf("\nSD_CMD58 return  %d....\n", status );
		return 1;
	}

	for(uint8_t i = 0;i < 4; i++){
		buff[i] = SPI_receive();
	}
	 printf( "OCR return: %x %x %x %x....\n\n", buff[0],buff[1],buff[2],buff[3] );



	 if ( buff[0] & 0x40 )
	    {
	 		 SD_Type = SD_TYPE_V2HC;
	 		 printf( "card is V2.0 SDHC.....\n\n" );
	    }
	  else {
	 		 SD_Type = SD_TYPE_V2;
	 		 printf( "card is V2.0.....\n\n" );
	    }
	while(MicroSDcard_command(CMD16,512,0xff)!=0); //set block size to 512

	MicroSDcard_command(CMD9,0,0xff); //read CSD


	}
	//increase spi CLOCK speed
	SPI0->BR = SPI_BR_SPPR(1)|SPI_BR_SPR(1); // 10,485,760/(2*2^2)

	return 0; //success

}



uint8_t Read_CSD_CID(uint8_t cmd,uint8_t *buff){ //block-->address to read from

	if (MicroSDcard_command(cmd,0,0xff) != 0x00){

		return 1;
	}


	for (uint16_t i = 0; i < 16; i++) { //receive 16 byte data block

		*buff++ = SPI_send(0xff);
	}

	//read CRCs
	//SPI_send(OXFF);
	//SPI_receive();
	//SPI_send(OXFF);
	//SPI_receive();

	return 0;
}


uint8_t readSingleBlock(uint32_t block,uint8_t *buff){ //block-->address to read from
	uint8_t status;

	if(SD_Type!=SD_TYPE_V2HC){
		block = block << 9;
	}

	status = MicroSDcard_command(CMD17,block,0x01); // read single block command


	while(status != 0){ //CMD17 should return 0
		status = MicroSDcard_command(CMD17,block,0x01);
	}


	while(SPI_receive() != DATA_START_BLOCK); // start token received?

	if(status == 0){ //correct return value for CMD17
		  for (uint16_t i=0; i<512; i++)  { //512 bytes per block
			  buff[i] =  SPI_send(0xff);
		  }
	}
	//2 bytes of CRC
	SPI_send(0XFF);
	SPI_send(0XFF);

	return 0;
}


uint8_t readMultipleBlock(uint32_t block,uint8_t *buff,uint8_t count){

	if(SD_Type!=SD_TYPE_V2HC)
		{
		  block = block << 9;
		}
	if(MicroSDcard_command(CMD18,block,0xff)!=0){ //CMD18 should return zero
		return 1;
	}

	do{
		while(SPI_send(0xff) != DATA_START_BLOCK); // start token received?

		for(uint16_t i=0; i<512; i++){
			*buff++ = SPI_send(0Xff);
	}

	//2 bytes of CRC
	SPI_send(0XFF);
	SPI_send(0XFF);

	}while(--count); //do section runs condition is checked

	MicroSDcard_command(CMD12,0,0xff); //end transmission since it is usally open when reading multiple blocks

	SPI_send(0XFF);//delay
	return 0;

}

uint8_t writeSingleBlock(uint32_t block,const uint8_t *buff){
	uint8_t  status;
	uint16_t retry;
	if(SD_Type!=SD_TYPE_V2HC)
	    {
	        block = block<<9 ;
	    }
	if(MicroSDcard_command(CMD24,block,0x01) != 0){ //enter single block write mode
		return 1;
	}

	//wait for sd card to be ready
	SPI_send(0XFF);
	SPI_send(0XFF);
	SPI_send(0XFF);

	SPI_send(DATA_START_BLOCK); //start token

	for(uint16_t i=0; i<512; i++){ // send 512 bytes of data
			SPI_send(buff[i]);
			}

	//Dummy CRC
	SPI_send(0XFF);
	SPI_send(0XFF);

	status =  SPI_send(0xff);
	status &= 0x1f;

	if(status != DATA_RES_ACCEPTED){
		return 1;
	}
	while(SPI_send(0xff) == 0x00){
		retry++;

	if(retry > 0xfffe){
		return 1;
	}
	}

	SPI_send(0xff);
	return 0;

}


uint8_t SD_WaitReady(){
	uint16_t retry = 0;
	uint8_t r1;
	do{
		r1 = SPI_send(0xff);
		retry++;
		if(retry == 0xfffe){
			return 1;}
	}while(r1!=0xff);

	return 0;
}

uint8_t writeMultipleBlocks(uint32_t block,const uint8_t *buff,uint8_t count){

	uint8_t status;
	if(SD_Type!=SD_TYPE_V2HC){
		block = block << 9;
	}


	 if(SD_Type!=SD_TYPE_MMC)
	    {
		 MicroSDcard_command(ACMD23,count,0x00);
	    }

	if(MicroSDcard_command(CMD25,block,0xff) != 0x00){ //CMD25 should return zero read multiple blocks mode
			return 1;
		}


	//wait for sd card to be ready
	SPI_send(0XFF);
	SPI_send(0XFF);
	//SPI_send(0XFF);

	do{
		SPI_send(WRITE_MULTIPLE_TOKEN); //start token
		for(uint16_t i=0; i<512; i++){ // send 512 bytes of data
			SPI_send(*buff++);
		}
	//Dummy CRC
		SPI_send(0XFF);
		SPI_send(0XFF);
		status = SPI_send(0XFF); //clock signals needed to receive
		status &= 0x1f;

		if(status != DATA_RES_ACCEPTED){
			return 1;}

		while(SD_WaitReady() == 0x01){
			return 1;
		}

	}while(--count);

	SPI_send(STOP_TRAN_TOKEN);

	SPI_send(0XFF);//delay

	return 0;
}


