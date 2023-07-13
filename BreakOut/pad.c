//	Set of pad routines to present same interface to pad as dtlh2500

#include <libps.h>

// Buffers
volatile u_char *bb0, *bb1;

// Prototypes
void PadInit (void);

// NOTE: argument id is not used
u_long PadRead (void);

// Set up buffers
void PadInit (void) {
	GetPadBuf(&bb0, &bb1);
}

// Pad read
u_long PadRead(void) {
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}