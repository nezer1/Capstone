#include <MKL25Z4.h>
#include "Human_Touch.h"
#include "OLED_Face.h"
#include "LED_Indicator.h"
#include "hand_movement.h"
#include "SPI.h"
#include"SD.h"
#include"diskio.h"
#include"ff.h"
#include "audio.h"
#include "Session_selection.h"



typedef enum{Happy1,Happy2, Sad, Neutral,Angry}emotions;
typedef enum{Blue,Yellow, Red, Green,White}RGB;
typedef enum{Up,Down}move;
typedef enum{no_touch,head,Back,right_hand,left_hand,left_leg,left_face,right_leg,right_face}touch_point;
typedef struct{
	emotions face;
	RGB RGB_leds;
	move hand;
	touch_point Touch;
}state;

extern state Robot;

extern uint8_t right_face_count;
extern uint8_t back_count;
extern uint8_t left_face_count;
extern uint8_t left_hand_count;
extern uint8_t right_hand_count;
extern uint8_t head_count;

extern uint8_t sleep_mode_tracker ;
extern FIL  fil;
