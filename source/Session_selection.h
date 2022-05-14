#include <mkl25z4.h>

#define MASK(X)		(1<<X)
#define RED_B (7)//PTD, INPUT for GPIO Interrupt
#define YELLOW_B (6)//PTD, INPUT for GPIO INTERRUPT
#define BUZZ_OUT (0) //PTE, comparator output

void Init_session();

