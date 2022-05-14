#include "audio.h"
#include "Final.h"

#define TOP4BITS(X)	((X>>8) & 0xF)
#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+(c2<<8)+c1)
#define	LD_DWORD(ptr)		(DWORD)(*(DWORD*)(BYTE*)(ptr))
#define	LD_WORD(ptr)		(WORD)(*(WORD*)(BYTE*)(ptr))

UINT audio_channel = 0;
DWORD resolution = 0;
DWORD fsmp = 0;
DWORD data_SZ = 0;
BYTE playback_Buff[10000];
UINT br;
//BYTE playback_Buff2[256];

unsigned char test[10] = {255,0,255,0,255,0,255,0,255};

int load_wav(
	FIL *fp			// Pointer to the open file object to play
){

	BYTE buff[40];
	DWORD eof,subchunk_SZ, offset;

	if (f_read(fp, buff, 12, &br) || br != 12) return -1;
	if (LD_DWORD(&buff[0]) != FCC('R','I','F','F')) return -1;
	if (LD_DWORD(&buff[8]) != FCC('W','A','V','E')) return -1;
	eof = LD_DWORD(&buff[4]);
	uint8_t i = 0;
	//while (f_tell(fp) < eof) {
	while(i<2){
		if (f_read(fp, buff, 8, &br) || br != 8) return -1;
			subchunk_SZ = (LD_DWORD(&buff[4]) + 1) & ~1; //subchunk size

		switch (LD_DWORD(&buff[0])) { //subchunkID

			case FCC('f','m','t',' '):
				//reading subchunk info into buffer based on size
				if (subchunk_SZ > 1000 || subchunk_SZ < 16 || f_read(fp, buff, subchunk_SZ, &br) || subchunk_SZ != br) return -1;
				if (LD_WORD(&buff[0]) != 0x1) return -1; //PCM?
				if (LD_WORD(&buff[2]) == 2) {//mono or stereo?
					return 0; //system doesnt play stereo
				}
				else{
					audio_channel = 1;//mono
				}
				resolution = LD_WORD(&buff[14]); //8 bit or 16 bit
				fsmp = LD_DWORD(&buff[4]); //sample rate
				i++;
				break;

			case FCC('d','a','t','a'):
				offset = f_tell(fp);
				data_SZ = subchunk_SZ; ;// wave data length in bytes
				//f_lseek(fp, f_tell(fp));
				if (f_read(fp,playback_Buff,10000,&br) || br != 10000) return -1;

				init_systick();
				init_DAC();
				i++;
				break;


}
	}
}


//uint16_t * Reload_DMA_Source=0;
//uint32_t Reload_DMA_Byte_Count=0;
//uint32_t DMA_Playback_Count=0;

//void Init_DMA(uint8_t *source,uint32 ){
//
//}



void init_DAC(void) {
  // clock gate
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;
	//default setting: disable buffer mode. (All control registers =0)
	// Enable DAC, Use VDDA as reference voltage (Analog Power Supply)

	//set up pin PTE30 for DAC out
	SIM->SCGC5 |=SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[PTE30] &=~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE30] |=PORT_PCR_MUX(0); //DAC out

//	uint16_t audio_start = 0x800;
//	DAC0->DAT[0].DATL = DAC_DATL_DATA0(audio_start); //byte
//	DAC0->DAT[0].DATH = TOP4BITS(audio_start);


	DAC0->C0 = DAC_C0_DACEN_MASK;// | DAC_C0_DACRFS_MASK;						//can even afford to use Vrefh rfs=0
}


void init_systick(){
	//initialize systick
	SysTick->LOAD = (20971520u/fsmp)-1 ;  //. Use timebase 1s. Configure duration. demo effect.
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
				SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
}


void SysTick_Handler(){
	//static uint8_t cnt =0;
	if(f_tell(&fil) < data_SZ){
	for(int i = 0;i<10000;i++){
		DAC0->DAT[0].DATL = DAC_DATL_DATA0(playback_Buff[i]);
		//printf("%x \n",playback_Buff[i]);
		//DAC0->DAT[0].DATL = DAC_DATL_DATA0(playback_Buff2[i]);
		//printf("%x \n",playback_Buff[i]);
	}
	 if (f_read(&fil,playback_Buff,10000,&br) || br != 10000){
		 printf("read");}
	 }
	//cnt++;
}

