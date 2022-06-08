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

	int i;  char buf[10];
	char clcd_str[20] = "";
	
	//디바이스 초기화
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();
	
	//게임 시작 터미널 화면
	printf("\n");
	printf("************************************\n");
	printf("*                                  *\n");
	printf("*      *********************       *\n");
	printf("*     ***********************      *\n");
	printf("*     ****     *****     ****      *\n");
	printf("*         ****       ****          *\n");
	printf("*      *********************       *\n");
	printf("*     **** 1 *** 2 *** 3 ****      *\n");
	printf("*    *************************     *\n");
	printf("*    ***** 4 *** 5 *** 6 *****     *\n");
	printf("*    *************************     *\n");
	printf("*    ***** 7 *** 8 *** 9 *****     *\n");
	printf("*    *************************     *\n");
	printf("*    ***** * *** 0 *** # *****     *\n");
	printf("*     ***********************      *\n");
	printf("*                                  *\n");
	printf("*   Welcome to number after 010    *\n");
	printf("*                                  *\n");
	printf("*  press (s) to start 1st approach *\n");
	printf("*  press (e) to exit               *\n");
	printf("*                                  *\n");
	printf("************************************\n");
	scanf("%c", buf);

	for (i = 0; i < strlen(buf); i++) {
		if (buf[i] == 's') { game_start(); }
		else if (buf[i] == 'e') { return FALSE; }
	}
	return TRUE;
}

void game_start(){

	start_allDevice();

	in_game(1);

	return TRUE;
}


void in_game(int level) {

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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
		}

	case 3:
		clcd_level_display(3);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//3초동안 깜빡임
		int i1;
		for (i1 = 0; i1 < 15; i1++) {
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
		}

	case 5:
		clcd_level_display(5);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//1.8초동안 깜빡임
		int i2;
		for (i2 = 0; i2 < 9; i2++) {
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
		}
	case 7:
		clcd_level_display(7);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);
	
		//2초 동안 1:3으로 깜빡임
		int i3;
		for (i3 = 0; i3 < 5; i3++) {
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
		}
	case 8:
		clcd_level_display(8);
		led_level(level);

		random8Digits = get_digits();

		digitsConnect = connectDigits(random8Digits);

		//2초 동안 1:3으로 깜빡임
		int i4;
		for (i4 = 0; i4 < 5; i4++) {
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
			led_clear();
			dot_clear();
			fnd_clear();
			emergency_closer();
		}
	default: break;

	}
}

void start_allDevice() {

	led_blink_all();
	int t1;
	for (t1 = 1; t1 < 6; t1++) {
		dot_write(8);
		fnd_all();
		led_all();
		clcd_set_DDRAM(0x00);
		clcd_write_string(" may I ask your "); //스토리 수정
		clcd_set_DDRAM(0x40);
		clcd_write_string(" phone number.? "); //스토리 수정
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

	int t2;
	for (t2 = 0; t2 < 8; t2++) {
		randomDigits[t2] = rand() % 10; //10진수로 변경
	}
	return randomDigits;
}

unsigned long connectDigits(int* digitsArray) {
	
	unsigned long digitsConnect = 0;
	int* digits = digitsArray;

	int t3;
	for (t3 = 0; t3< 8; t3++) {
		//[0]이 LSB
		int temp = (0x0f & digits[t3]);
		temp = temp << 4 * t3;
		digitsConnect |= temp;
	}

	return digitsConnect;
}



unsigned long keypad_input_digits(int* key_value) {

	int col, row, key_count = 0;
	unsigned long digitsConnect_temp = 0;
	short key_temp = 0;
	char clcd_key_value[] = "";
	clcd_key_value[0] = '\0';
	char c1[4] = "****";
	char c2[4] = "****";

	while (key_count < 8) {	
	// while 안에 조건 레벨별 추가가능
	
		for (col = 0; col < MAX_KEY_COL -1; col++) {
			*keypad_out = (short)(0x08 >> col);
			key_temp = *keypad_in;

			for (row = 0; row < MAX_KEY_ROW; row++) {
				if (((key_temp >> row) & 1) == 1) {
					if (row == 0) {
						*key_value = col +1 ;
					}
					else if(row == 1) {
						*key_value = col +4;
					}
					else if (row == 2) {
						*key_value = col + 7;
					}
					else {
						*key_value = 0;
					}
						
					//*key_value = (row * 4) + col;

					if (key_count < 4) {
						c1[key_count] = *key_value;
					}
					else {
						c2[key_count - 4] = *key_value;
					}

					key_count++;

					int temp = (0x0f & *key_value);
					temp = temp << 4 * (key_count-1);
					digitsConnect_temp |= temp;

					dot_write(*key_value);

					strcat(clcd_key_value, "010-");
					strcat(clcd_key_value, c1);
					strcat(clcd_key_value, "-");
					strcat(clcd_key_value, c2);

					usleep(500000);
					clcd_set_DDRAM(0x00);
					clcd_write_string("Next Number?");
					clcd_set_DDRAM(0x40);
					clcd_write_string(clcd_key_value);

					dot_clear();
					clcd_clear_display();

				}

			}
		}
	}
	int i;
	for (i = 0; i < 2; i++) {
		clcd_set_DDRAM(0x00);
		clcd_write_string("  calling to...");
		clcd_set_DDRAM(0x40);
		clcd_write_string(clcd_key_value);
		usleep(800000);
		clcd_clear_display();
		usleep(800000);
	}
	return digitsConnect_temp;
}
