//-------| src/led.c |-------//
#include "led.h"

static short * led;

void init_led(short * address) {
	led = address;
}

void led_down_shift() {
	
	int i;
	for( i=1; i<=16 ; i++ ) {
		*led = (short)~( ((unsigned long)0xFF << i) >> 8 );
		usleep(50000);
	}
}

void led_up_shift() {
	
	int i;
	for( i=1; i<=16 ; i++ ) {
		*led = (short)~((unsigned long)0xFF00 >> i);
		usleep(50000);
	}
}

void led_blink_all() {
	int i;
	for( i=1; i<=16 ; i++ ) {
		if ( i%2 == 1 ) {
			*led = (short)~0xFF;
		}
		else {
			*led = (short)~0x00;
		}
		usleep(100000);
	}
}

void led_clear() {
	*led = (short)~0;
}

void led_all() {
	*led = (short)~0xFF;
}

//레벨에 맞춰서 led 켜지는 함수
void led_level(int level) {

	short ledOn = 0xff;

	int t4;
	for (t4 = 0; t4 < level; t4++){
		ledOn = ledOn << 1;
	}
	ledOn = (0xff & ledOn);
	*led = ledOn;
}
