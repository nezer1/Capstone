#include "Final.h"

state Robot = {Happy1,Blue,Up,no_touch};
uint8_t right_face_count = 0;
uint8_t back_count = 0;
uint8_t left_face_count = 0;
uint8_t right_hand_count = 0;
uint8_t left_hand_count = 0;
uint8_t head_count = 0;

uint8_t sleep_mode_tracker = 0;

FIL fil;


void System_on(){
	TPM1->SC |= TPM_SC_CMOD(1); //start ultrasonic detection
}

//controls the timing for sending the robot into neutral mode
void Robot_timer(int count){
	SIM->SCGC5|= SIM_SCGC5_LPTMR_MASK; //clocck gate LPTMR

	MCG->C2 &= ~MCG_C2_IRCS_MASK; // select slow internal refclock, 32kHz /2
	MCG->C1 |= MCG_C1_IRCLKEN_MASK; // enable Internal Ref Clock

	//Reset LPTMR settings
	LPTMR0->CSR = 0;


	LPTMR0->CMR = count;
	LPTMR0->PSR |= LPTMR_PSR_PRESCALE(14);

	// enable interrupt & timer & clear flags
	LPTMR0->CSR |= LPTMR_CSR_TIE_MASK | LPTMR_CSR_TEN_MASK | LPTMR_CSR_TCF_MASK;

	NVIC_ClearPendingIRQ(LPTMR0_IRQn);
	NVIC_SetPriority(LPTMR0_IRQn, 3);
	NVIC_EnableIRQ(LPTMR0_IRQn);
}


void Reset_Robot_Timer(){
	LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK ; //disable timer
	LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; //disable timer
}

void LPTMR0_IRQHandler(void){

	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
	//Neutral mode
	drawPart(1,1,eye,44,47);
	drawPart(81,1,eye,44,47);
	drawPart(33,109,mouth,70,14);

	//Robot state
	Robot.face = Neutral;
	Robot.RGB_leds = White;
	Robot.hand = Up;

	LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK ; //disable timer
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;

	printf("timer stopped");
}

uint16_t i,j;


int main(){
	//SD card
	  FATFS fs;
	  FRESULT fr;


	  char file_name1[12]="final.wav";

	  Init_SPI0();
	  init_MicroSDcard();
	  fr= f_mount(&fs,file_name1,0);

	  if(fr){
	    printf("\nError mounting file system\r\n");
	    for(;;){}
	 }
	  fr = f_open(&fil, file_name1, FA_READ);                         // FA_WRITE | FA_OPEN_ALWAYS);//create csv file

	  if(fr){
	     printf("\nError opening text file\r\n");
	     for(;;){}
	 }
	  load_wav(&fil);
	  while(1){

}
}

//	//Human Presence Detection & Touch Detection
//	init_pin();
//	init_Ultrasonic();
//	init_ADC();
//	init_PIT1(); //touch
//	init_PIT0(); //human detection
//	System_on(); //helps by pass PIR sensor for ultrasonic detetcion
//
//	//OLED
//	//Init_DMA();
//	Init_SPI1();
//	Init_OLED();
//
//	setOrientation(0);
//
////	//session selection
//	Init_session();
//
////	//LED_INDICATOR
//	init_TPM0(); //LEDs are off by default
//
////	//hand_movement
//	init_TPM2();
//
//	//start_motor();
//	//TPM2->SC |= TPM_SC_CMOD(1);
//
//	//set_motor(0x53D,0);
//	//set_motor(0xF5C,0);
//
//	//just for initialization purposes
//	Robot_timer(0x150); // 120secs, initializes LPTMR
//
////	//static uint16_t cnt = 0;
//
//
////	//Control_RGB_LED_1(1023,350,350);
//
//
//	drawLine(0, 0,BLACK,128,128);
//
//
//	while(1){
////-----------------------------Intro mode-------------------------------------
//		if(g_timer_done==1 || ( g_result_0 >  0xAE && g_result_0 < 0xDF)){//Touch detected on head or 20secs timer ends
//
//
//			Robot.Touch = head;
//			head_count++;
//
//			Reset_Robot_Timer();
//
//			static uint8_t cnt0 = 0;
//			cnt0++;
//
//
//			//Introduction mode
//			drawPart(1,1,eye,44,47);
//			drawPart(81,1,eye,44,47);
//			start_motor();
//			drawPart(33,109,happy_mouth,70,14);
//			Control_RGB_LED_2(0,0,1023); //blue LED0
//			Control_RGB_LED_1(0,0,1023); //blue LED0
//
//			Robot_timer(0x10); // 120secs , 0x10 for demo purposes
//
//			//Robot state
//			Robot.face = Happy1;
//			Robot.RGB_leds = Blue;
//			Robot.hand = Up;
//
//			start_motor();
//
//			g_timer_done = 0;
//
//			if(cnt0 > 40){
//				drawPart(1,1,close_eyes,44,46);
//				drawPart(81,1,close_eyes,44,46);
//				drawPart(1,1,eye,44,46);
//				drawPart(81,1,eye,44,46);//}
//				cnt0 = 0;
//				}
//		}
//
//// -------------------------Happy mode 2-----------------------------------------------
//		if(		(g_result_9 > 0xAF && g_result_9 < 0xDF) || //right side of face
//				(g_result_4 > 0xAF && g_result_4 < 0xDF) ||  //hugs --back
//				(g_result_8 > 0xAF && g_result_8 < 0xDF) || //left side of face
//				(g_result_3 > 0xAF && g_result_3 < 0xDF) || //squeezing right hand
//				(g_result_7 > 0xAF && g_result_7 < 0xDF) ) //squeezing left hand
//		{
//			static uint8_t cnt1 = 0;
//			cnt1++;
//
//			if(g_result_9 > 0xAF && g_result_9 < 0xDF){
//				Robot.Touch = right_face;
//				right_face_count++;
//						}
//			if(g_result_4 > 0xAF && g_result_4 < 0xDF){
//				Robot.Touch = Back;
//				back_count++;
//									}
//			if(g_result_8 > 0xAF && g_result_8 < 0xDF){
//				Robot.Touch = left_face;
//				left_face_count++;
//									}
//
//			if(g_result_3 > 0xAF && g_result_3 < 0xDF){
//				Robot.Touch = right_hand;
//				right_hand_count++;
//							}
//
//			if(g_result_7 > 0xAF && g_result_7 < 0xDF){
//				Robot.Touch = left_hand;
//				left_hand_count++;
//						}
//
//
//			Reset_Robot_Timer();
//
//			if(Robot.face == Happy1){
//				Control_RGB_LED_1(1000,300,0); //yellow
//				Control_RGB_LED_2(800,800,0); //yellow
//				//start_motor();
//			}else{
//				drawPart(1,1,eye,44,47);
//				drawPart(81,1,eye,44,47);
//				//start_motor();
//				drawPart(33,109,happy_mouth,70,14);
//				Control_RGB_LED_1(1000,300,0); //yellow
//				Control_RGB_LED_2(800,800,0); //yellow
//					}
//
//			//Robot state
//			Robot.face = Happy2;
//			Robot.RGB_leds = Yellow;
//			Robot.hand = Up;
//
//
//
//
//
//			start_motor();
//
//			Robot_timer(0x10); // 120secs
//
//			if(cnt1>40){
//				drawPart(1,1,close_eyes,44,46);
//				drawPart(81,1,close_eyes,44,46);
//				drawPart(1,1,eye,44,46);
//				drawPart(81,1,eye,44,46);//}
//				cnt1 = 0;
//					}
//
//				}
//// -------------------------Angry mode -----------------------------------------------
//		if(right_face_count > 10 || back_count > 10 || left_face_count > 10 ||
//			right_hand_count > 10 || left_hand_count > 10	|| head_count > 10){
//			//touch count
//			right_face_count = 0;
//			back_count = 0;
//			left_face_count = 0;
//			left_hand_count = 0;
//			right_hand_count = 0;
//			head_count = 0;
//
//			Reset_Robot_Timer();
//
//			//Angry mode
//			if(Robot.face == Sad){ //no need to draw mouth again
//				drawPart(81,1,angry_left_eye,44,47);
//				drawPart(1,1,angry_right_eye,44,47);
//
//				Control_RGB_LED_1(0,800,0); //green
//				Control_RGB_LED_2(0,800,0); //green
//
//				//start_motor();
//				}else{
//				drawPart(81,1,angry_left_eye,44,47);
//				drawPart(1,1,angry_right_eye,44,47);
//				//start_motor();
//				drawPart(33,109,angry_mouth,70,14);
//				Control_RGB_LED_1(0,800,0); //green
//				Control_RGB_LED_2(0,800,0); //green
//					}
//
//				//Robot state
//				Robot.face = Angry;
//				Robot.RGB_leds = Green;
//				Robot.hand = Up;
//				start_motor(); // motor depends on current state
//
//				Robot_timer(0x10); // 120secs
//				}
//
////--------------------------SAD mode-----------------------------------------------------------
//		if(g_result_0 == 0xFF || g_result_4 == 0xFF || g_result_3 == 0xFF ||
//		g_result_8 == 0xFF || g_result_14 == 0xFF || g_result_5 == 0xFF ||
//		g_result_7 == 0xFF || g_result_8 == 0xFF){ //Hard hit, Ignored by the user
//				Reset_Robot_Timer();
//
//				//Sad mode
//				if(Robot.face == Angry){ //not need to draw mouth again
//					drawPart(81,1,sad_left_eye,44,47);
//						drawPart(1,1,sad_right_eye,44,47);
//						Control_RGB_LED_1(800,0,0); //red
//						Control_RGB_LED_2(0,800,0); //red
//
//					}else{
//						drawPart(81,1,sad_left_eye,44,47);
//						drawPart(1,1,sad_right_eye,44,47);
//						start_motor();
//						drawPart(33,109,angry_mouth,70,14);
//						Control_RGB_LED_1(800,0,0); //red
//						Control_RGB_LED_2(0,800,0); //red
//
//					}
//
//					//Robot state
//					Robot.face = Sad;
//					Robot.RGB_leds = Red;
//					Robot.hand = Up;
//					start_motor();
//
//					Robot_timer(0x10); // 120secs
//				}
//	}
//}
//




