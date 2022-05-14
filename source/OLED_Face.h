#include <MKL25Z4.H>
#include <stdint.h>

#define MASK(X)		(1<<X)



//SSD1351 Commands
#define CMD_SETCOLUMN    (0x15)
#define CMD_SETROW       (0x75)
#define CMD_WRITERAM      (0x5C)
#define CMD_READRAM        (0x5D)
#define CMD_SETREMAP       (0xA0)
#define CMD_STARTLINE       (0xA1)
#define CMD_DISPLAYOFFSET   (0xA2)
#define CMD_DISPLAYALLOFF   (0xA4)
#define CMD_DISPLAYALLON    (0xA5)
#define CMD_NORMALDISPLAY   (0xA6)
#define CMD_INVERTDISPLAY    (0xA7)
#define CMD_FUNCTIONSELECT   (0xAB)
#define CMD_DISPLAYOFF       (0xAE)
#define CMD_DISPLAYON        (0xAF)
#define COM_SCAN_INC         (0xC0)
#define COM_SCAN_DEC         (0xC8)
#define CMD_PRECHARGE        (0xB1)
#define CMD_DISPLAYENHANCE   (0xB2)
#define CMD_CLOCKDIV         (0xB3)
#define CMD_SETVSL           (0xB4)
#define CMD_SETGPIO          (0xB5)
#define CMD_PRECHARGE2       (0xB6)
#define CMD_SETGRAY          (0xB8)
#define CMD_USELUT            (0xB9)
#define CMD_PRECHARGELEVEL   (0xBB)
#define CMD_VCOMH             (0xBE)
#define CMD_CONTRASTABC       (0xC1)
#define CMD_CONTRASTMASTER    (0xC7)
#define CMD_MUXRATIO          (0xCA)
#define CMD_COMMANDLOCK       (0xFD)
#define CMD_HORIZSCROLL       (0x96)
#define CMD_STOPSCROLL        (0x9E)
#define CMD_STARTSCROLL       (0x9F)
#define SET_COM_PINS           (0xDA)

#define DISPLAYON              (0xAF)
#define SCREEN_WIDTH      (128)
#define SCREEN_HEIGHT     (128)
//color definitions
#define BLACK (0x0000)
#define	RED            (0x001F)
#define	BLUE            (0xF800)
#define	GREEN          ( 0x07E0)
#define CYAN           ( 0x07FF)
#define MAGENTA         (0xF81F)
#define YELLOW         ( 0xFFE0  )
#define WHITE           (0xFFFF)

#define SCK  (2)//PTE
#define DC (5) //PTE
#define MOSI  (3)//PTE
#define MISO  (1)//PTE
#define CS    (4)//PTE
#define RS (20)//PTA



extern const uint16_t angry_right_eye[];
extern const uint16_t angry_left_eye[];
extern const uint16_t angry_mouth[];

extern const uint16_t eye[];
extern const uint16_t mouth[];

extern const uint16_t close_eyes[];

extern const uint16_t sad_left_eye[];
extern const uint16_t sad_right_eye[];

extern const uint16_t happy_mouth[];

extern const uint16_t look_right[];
extern const uint16_t look_left[];

extern const uint16_t no_mouth[];
void Init_SPI1();
uint8_t SPI_send_receive(uint8_t d_out);
void Init_OLED();
void on();

void write_command(uint8_t command);
void write_16_data(uint16_t data);
void write_data(uint8_t data );
void set_Disp_Window(uint16_t x1, uint16_t y1,uint16_t w, uint16_t h ) ;
void setOrientation(uint8_t r);
void drawpixels(const uint16_t color[],uint32_t len);
void drawfaceRGB(uint16_t x1,uint16_t y1,const uint16_t *colors,uint16_t w, uint16_t h);
void off();
void Power();

void drawPart(uint16_t x1,uint16_t y1,const uint16_t *colors,uint16_t w, uint16_t h);
void drawLine(uint16_t x1, uint16_t y1, uint16_t color,uint16_t w, uint16_t h );

void OLED_sleep();
void Wake_up();

void delay_ms(int t_ms);
