//-------| src/keypad.c |-------//
#include "keypad.h"


static short * keypad_out, * keypad_in;

void init_keypad(short * address_out, short * address_in) {
	keypad_out  = address_out;
	keypad_in = address_in;
}


