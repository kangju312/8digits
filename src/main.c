//-------| src/main.c |-------//
#include "main.h"


static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};
static off_t IEB_FND[MAX_FND] = {
	IEB_FND0,
	IEB_FND1,
	IEB_FND2,
	IEB_FND3,
	IEB_FND4,
	IEB_FND5,
	IEB_FND6,
	IEB_FND7
};

static int fd;
static int map_counter = 0;
static void * map_data[100];
static seclection_t sel; 


int main(int argc, char* argv[]) {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in;
	
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}
	
	led = mapper(IEB_LED, PROT_WRITE);
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i], PROT_WRITE);
	}
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
	}
	clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
	clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);
	keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
	keypad_in = mapper(IEB_KEY_R, PROT_READ);
	
	init_led(led);
	init_dot(dot);
	init_fnd(fnd);
	init_clcd(clcd_cmd, clcd_data);
	init_keypad(keypad_out, keypad_in);
	
	game_set();

	//sel.all = 0;
	// int open = 0;
	// while( logic() == TRUE ) {	}
	
	unmapper();
	close(fd);
	return 0;
}

short * mapper(off_t offset, int prot) {
	map_data[map_counter] = mmap(NULL, sizeof(short), prot, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}


void game_set() {
	int i;  char buf[10];
	char clcd_str[20] = "";
	
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();
	
	//게임 시작 터미널 화면
	printf("\n");
	printf("************************************\n");
	printf("*     Welcome to  8digits game	   *\n");
	printf("*								   *\n");
	printf("*	 press (s) to start level 1	   *\n");
	printf("*		press (e) to exit		   *\n");
	printf("*       						   *\n");
	printf("************************************\n\n");
	scanf("%s", buf);
	
	for (i = 0; i < strlen(buf); i++) {
		if (buf[i] == 's') { game_start(); }
		else if (buf[i] == 'e') { break; }
	}

}

void game_start(){

	//게임 시작 디바이스 출력

	blinkAllDevice();

	/*
	****************(#1)****************
	clcd 출력 더 디테일하게 수정 필요
	메세지(외우는 게임이라고),
	깜빡임, clear,글자 위치, 글자 움직임

	clcd_set_DDRAM(0x00);
	clcd_write_string("game starting...");


	usleep(2000000);
	clcd_set_DDRAM(0x00);
	clcd_write_string("level 1 start");
	clcd_clear_display();

	*/

	void in_game(1);

}


void in_game(int level) {
	int displayTime;
	int randomDigits[8];
	unsigned long tempDigits = 0;
	unsigned long digitsConnect = 0;

	srand(time(NULL));

	for (int i = 0; i < 8; i++) {
		randomDigidts[i] = rand() % 16;
	}

	switch (level) {
		case 1:
			
			led_level(level);


			/*
			****************(#2)****************
			clcd에 level 1 시작된다고
			fnd에 출력되는 8자리를 암기하라고 출력
			*/

			for (int i = 0; i < 8; i++) {
				temp = (0x0f & random8Digits[i]);
				temp = temp << 4 * i;
				digitsConnect |= temp;
			}

			fnd_hexa_number(digitsConnect);
			displayTime = level * 1000000;
				usleep(displayTime);

			fnd_clear();

			/*
			****************(#3)****************
			clcd에 keypad입력하라고 출력
			*/

			/*
			keypad에 입력 받으면서 dot에 표시해주기
			*/
	}

	//fnd에 레벨 별로 어떻게 출력할지?
	// //key value를 어떻게 여러개 저장하지.?
	
}


void input_mode() {
	int key_count, key_value;
	char clcd_str[20];
	key_count = keypad_read(&key_value);
	
	if( key_count == 1 ) {
		if( sel.led  == 1 ) { led_bit(key_value); }
		if( sel.dot  == 1 ) { dot_write(key_value); }
		if( sel.fnd  == 1 ) { fnd_write(key_value, 7); }
		if( sel.clcd == 1 ) { 
			sprintf(clcd_str, "%#04x            ", key_value);
			clcd_set_DDRAM(0x40);
			clcd_write_string(clcd_str);
		}
	}
	else if( key_count > 1 ) {
		sel.all = 0;
	}
}

void blinkAllDevice() {

	led_blink_all();
	for (i = 1; i < 6; i++) {
		dot_write(8);
		fnd_all();
		led_all();
		usleep(100000);
		dot_clear();
		fnd_clear();
		led_clear();
		usleep(100000);
		
	}
}

