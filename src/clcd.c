//-------| src/clcd.c |-------//
#include "clcd.h"



static short * clcd_cmd, * clcd_data;

void init_clcd(short * address_cmd, short * address_data) {
	int D=1, C=0, B=0;
	int DL=1, N=1, F=0;
	
	clcd_cmd  = address_cmd;
	clcd_data = address_data;
	
	clcd_clear_display();
	clcd_return_home();
	clcd_display_control(D, C, B);
	clcd_function_set(DL, N, F);
}

void clcd_write_cmd(int cmd) {
	*clcd_cmd = (short)cmd; usleep(CLCD_CMD_US);
}
void clcd_write_data(int data) {
	*clcd_data = (short)data; usleep(CLCD_DATA_US);
}

void clcd_clear_display() {
	clcd_write_cmd(0x01); usleep(CLCD_RETURN_US);
}
void clcd_return_home() {
	clcd_write_cmd(0x02); usleep(CLCD_RETURN_US);
}
void clcd_entry_mode_set(int ID, int S) {
	int cmd = 1 << 2;
	if( ID != 0 ) { cmd |= (1 << 1); }
	if( S  != 0 ) { cmd |= (1 << 0); }
	clcd_write_cmd(cmd);
}
void clcd_display_control(int D, int C, int B) {
	int cmd = 1 << 3;
	if( D != 0 ) { cmd |= (1 << 2); }
	if( C != 0 ) { cmd |= (1 << 1); }
	if( B != 0 ) { cmd |= (1 << 0); }
	clcd_write_cmd(cmd);
}
void clcd_shift(int SC, int RL) {
	int cmd = 1 << 4;
	if( SC != 0 ) { cmd |= (1 << 3); }
	if( RL != 0 ) { cmd |= (1 << 2); }
	clcd_write_cmd(cmd);
}
void clcd_function_set(int DL, int N, int F) {
	int cmd = 1 << 5;
	if( DL != 0 ) { cmd |= (1 << 4); }
	if( N  != 0 ) { cmd |= (1 << 3); }
	if( F  != 0 ) { cmd |= (1 << 2); }
	clcd_write_cmd(cmd);
}
void clcd_set_DDRAM(int address) {
	int cmd = 1 << 7;
	cmd |= address;
	clcd_write_cmd(cmd);
}

void clcd_write_string(char str[]) {
	int i;
	for( i=0; (str[i] != 0); i++) {
		clcd_write_data(str[i]);
	}
}

void clcd_level_display(int level_buf) {

	char s1[10];
	char clcd_str_buf[60] = "";

	sprintf(s1, "%d", level_buf);

	clcd_str_buf[0] = '\0';
	strcat(clcd_str_buf, "level ");
	strcat(clcd_str_buf, s1);
	strcat(clcd_str_buf, " start! ");

	clcd_set_DDRAM(0x00);
	clcd_write_string(clcd_str_buf);
	usleep(2000000);
	clcd_clear_display();

	clcd_set_DDRAM(0x40);
	clcd_write_string("VVVVVVVVVVVVVVVV");
	usleep(2000000);
	clcd_clear_display();
}

void clcd_start_msg() {
	for (int i = 0; i < 2; i++) {
		clcd_set_DDRAM(0x00);
		clcd_write_string("game starting...");
		clcd_set_DDRAM(0x40);
		clcd_write_string("****************");
		usleep(800000);
		clcd_clear_display();
		usleep(800000);
	}
}

void clcd_inputMsg() {
	char guide_message1[] = "Enter the shown";
	char guide_message2[] = "8digits in order";
	clcd_set_DDRAM(0x00);
	clcd_write_string(guide_message1);
	clcd_set_DDRAM(0x40);
	clcd_write_string(guide_message2);
	usleep(2000000);
}

void clcd_correct() {
	clcd_set_DDRAM(0x04);
	clcd_write_string("Correct!")
		usleep(2000000);
	clcd_clear_display();
}

void clcd_wrong() {
	clcd_set_DDRAM(0x05);
	clcd_write_string("Wrong!")
		usleep(2000000);
	clcd_clear_display();
}



void left_time_display(double time_buf) {
	char s2[10] = "";
	char clcd_str_buf2[60] = "";
	clcd_str_buf2[0] = '\0';
	sprintf(s2, "%.2f", time_buf);
	strcat(clcd_str_buf2, "Lefttime : ");
	strcat(clcd_str_buf2, s2);
	clcd_set_DDRAM(0x40);
	clcd_write_string(clcd_str_buf2);
	clcd_set_DDRAM(0x00);
	clcd_write_string("enter the 8digits");
}