//-------| src/fnd.c |-------//
#include "fnd.h"


static unsigned short fnd_hexadecimal[16] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x67, // 9
	0x77, // A
	0x7C, // b
	0x39, // C
	0x5E, // d
	0x79, // E
	0x71  // F
		//알파벳 만들고 싶은거 만들자
};

static short * fnd[MAX_FND];

void init_fnd(short * address[]) {
	int i;
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = address[i];
	}
}

void fnd_clear() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0;
	}
}

void fnd_all() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0xFF;
	}
}

void fnd_write(int hexadecimal, int fnd_num) {
	*fnd[fnd_num] = fnd_hexadecimal[hexadecimal];
}

void fnd_hexa_number(unsigned long number) {
	int i;
	for(i=0; i<MAX_FND; i++){
		fnd_write( (short)(number & 0xF), i);
		number = number >> 4;
	}
}

unsigned long fnd_digits_display(int * digits) {

	unsigned long digitsConnect = 0;

	for (int i = 0; i < 8; i++) {

		int temp = (0x0f & digits[i]);
		temp = temp << 4 * i;
		digitsConnect |= temp;
	}

	fnd_hexa_number(digitsConnect);

	return digitsConnect;
}