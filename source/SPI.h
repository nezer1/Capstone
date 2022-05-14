#include <MKL25Z4.H>

#define MASK(X)		(1<<X)

#define SCK_SD  (5)//PTC
#define MOSI_SD (6)//PTC
#define MISO_SD  (7)//PTC
#define CS_SD   (4)//PTC



uint8_t SPI_send(uint8_t d_out);
uint8_t SPI_receive();
void Init_SPI0();
