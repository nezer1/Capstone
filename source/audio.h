#include <MKL25Z4.h>
#include"diskio.h"
#include"ff.h"

#define S_POS (1)
#define S_NEG (2)

#define PTE30 (30)

int load_wav(
	FIL *fp			// Pointer to the open file object to pla
);


void init_DAC(void);
void init_systick();
extern BYTE playback_Buff[10000];
extern UINT audio_channel;
extern DWORD resolution;






