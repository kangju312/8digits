//-------| src/main.c |-------//
#include "main.h"
//int i for 문 밖으로 빼기

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
short* keypad_out, * keypad_in;

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
	
	//여기까지 매핑&초기화

	while (game_set() == 1) {}
	
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


truth_t game_set() {

	int i;  char buf;
	char clcd_str[20] = "";
	
	//디바이스 초기화
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
	scanf("%c", buf);
	
	if (buf == 's') {
			while( game_start() == 1 ; }
	else if (buf == 'e') { return FALSE; }
	return TRUE;
}

truth_t game_start(){

	start_allDevice();

	in_game(1);

	return TRUE;
}


truth_t in_game(int level) {

	int displayTime = 0;
	unsigned long digitsConnect = 0;
	unsigned long digitsInput = 0;
	int* random8Digits;
	int* key_value;
	
	switch (level) {
	case 1:

		clcd_level_display(1);

		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		fnd_hexa_number(digitsConnect);

		//5초 동안 보여줌
		displayTime = 5000000; 
		usleep(displayTime);
		fnd_clear();

		clcd_InputMsg();
		
		digitsInput = keypad_input_digits(&key_value);
		if (digitsInput == digitsConnect) {

			clcd_correct();
			in_game(2);
		}
		else {

			clcd_wrong();

			return FALSE;
		}

	case 2:

		clcd_level_display(2);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		fnd_hexa_number(digitsConnect);

		//2.5초 동안 보여줌
		displayTime = 2500000;
		usleep(displayTime);
		fnd_clear();

		clcd_InputMsg();

		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();

			in_game(3);
		}
		else {

			clcd_wrong();
			return FALSE;
		}

	case 3:
		clcd_level_display(3);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//3초동안 깜빡임
		int i;
		for (i = 0; i < 15; i++) {
			fnd_hexa_number(digitsConnect);
			usleep(100000);
			fnd_clear();
			usleep(100000);
		}


		clcd_InputMsg();
		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();
			in_game(4);
		}
		else {

			clcd_wrong();
			return FALSE;
		}
	case 4:
		clcd_level_display(4);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//2초 동안 보여줌
		displayTime = 2000000;
		usleep(displayTime);
		fnd_clear();

		clcd_InputMsg();
		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();
			in_game(5);
		}
		else {

			clcd_wrong();
			return FALSE;
		}

	case 5:
		clcd_level_display(5);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//1.8초동안 깜빡임
		int i;
		for (i = 0; i < 9; i++) {
			fnd_hexa_number(digitsConnect);
			usleep(100000);
			fnd_clear();
			usleep(100000);
		}


		clcd_InputMsg();
		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();
			in_game(6);
		}
		else {

			clcd_wrong();
			return FALSE;
		}
	case 6:
		clcd_level_display(4);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//1.3초 동안 보여줌
		displayTime = 1300000;
		usleep(displayTime);
		fnd_clear();

		clcd_InputMsg();

		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();
			in_game(7);
		}
		else {

			clcd_wrong();
			return FALSE;
		}
	case 7:
		clcd_level_display(7);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);
	
		//2초 동안 1:3으로 깜빡임
		int i;
		for (i = 0; i < 5; i++) {
			fnd_hexa_number(digitsConnect);
			usleep(100000);
			fnd_clear();
			usleep(300000);
		}


		clcd_InputMsg();
		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();
			in_game(8);
		}
		else {

			clcd_wrong();
			return FALSE;
		}
	case 8:
		clcd_level_display(8);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//2초 동안 1:3으로 깜빡임
		int i;
		for (i = 0; i < 5; i++) {
			fnd_hexa_number(digitsConnect);
			usleep(100000);
			fnd_clear();
			usleep(300000);
		}


		clcd_InputMsg();
		digitsInput = keypad_input_digits(&key_value);

		if (digitsInput == digitsConnect) {

			clcd_correct();
			clcd_game_success();
			return FALSE;
		}
		else {

			clcd_wrong();
			return FALSE;
		}
	default: break;

	}
}

void start_allDevice() {

	led_blink_all();
	for (i = 1; i < 6; i++) {
		dot_write(8);
		fnd_all();
		led_all();
		clcd_set_DDRAM(0x00);
		clcd_write_string("game starting..."); //스토리 수정
		clcd_set_DDRAM(0x40);
		clcd_write_string("****************"); //스토리 수정
		usleep(200000);

		dot_clear();
		fnd_clear();
		led_clear();
		clcd_clear_display();
		usleep(200000);


	}
}

int* get_digits() {

	static int randomDigits[8];

	srand(time(NULL));

	int t;
	for (t = 0; t < 8; t++) {
		randomDigits[t] = rand() % 16;
	}
	return randomDigits;
}

unsigned long connectDigits(int* digitsArray) {
	
	unsigned long digitsConnect = 0;
	int* digits = digitsArray;

	int t;
	for (int t = 0; t< 8; t++) {
		//[0]이 LSB
		int temp = (0x0f & digits[t]);
		temp = temp << 4 * t;
		digitsConnect |= temp;
	}

	return digitsConnect;
}



unsigned long keypad_input_digits(int* key_value) {


	//int endTime = (unsigned)time(NULL);
	//endTime += 20;
	//left_time_display(6);
	//while ( key_count < 8) {	// while 안에 조건 레벨별 추가가능
	//	double startTime = (unsigned)time(NULL);
	//	double left_time = endTime - startTime;
	//	if (left_time < 0) {
	//		clcd_timeout();
	//		break;
	//				}
	//	if (left_time <= 5.5 && left_time > 5) { //if, else-if문으로 코드 짤때,
	//		clcd_clear_display();
	//		left_time_display(5.5);
	//	}
	//	else if (left_time <= 5 && left_time > 4.5) {
	//		clcd_clear_display();
	//		left_time_display(5);
	//	}
	//	else if (left_time <= 4.5 && left_time > 4) {
	//		clcd_clear_display();
	//		left_time_display(4.5);
	//	}

	//	else if (left_time <= 4 && left_time > 3.5) {
	//		clcd_clear_display();
	//		left_time_display(4);
	//	}

	//	else if (left_time <= 3.5 && left_time > 3) {
	//		clcd_clear_display();
	//		left_time_display(3.5);
	//	}

	//	else if (left_time <= 3 && left_time > 2.5) {
	//		clcd_clear_display();
	//		left_time_display(3);
	//	}
	//	else if (left_time <= 2.5 && left_time > 2) {
	//		clcd_clear_display();
	//		left_time_display(2.5);
	//	}

	//	else if (left_time <= 2 && left_time > 1.5) {
	//		clcd_clear_display();
	//		left_time_display(2);
	//	}

	//	else if (left_time <= 1.5 && left_time > 1) {
	//		clcd_clear_display();
	//		left_time_display(1.5);
	//	}
	//	else if (left_time <= 1 && left_time > 0.5) {
	//		clcd_clear_display();
	//		left_time_display(1);
	//	}

	//	else if (left_time <= 0.5 && left_time > 0) {
	//		clcd_clear_display();
	//		left_time_display(0.5);
	//	}

		int col, row, key_count = 0;
		unsigned long digitsConnect_temp = 0;
		short key_temp = 0;
		for (col = 0; col < MAX_KEY_COL; col++) {
			*keypad_out = (short)(0x08 >> col);
			key_temp = *keypad_in;

			for (row = 0; row < MAX_KEY_ROW; row++) {
				if (((key_temp >> row) & 1) == 1) {
					*key_value = (row * 4) + col;
					key_count++;

					int temp = (0x0f & *key_value);
					temp = temp << 4 * (8 - key_count);
					digitsConnect_temp |= temp;

					dot_write(*key_value);
					
					int start = clock();
					while ((clock() - start) < 1000) {
						dot_clear();
					}
				}

			}
		}

	return digitsConnect_temp;
}