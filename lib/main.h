//-------| lib/main.h |-------//
#ifndef	MAIN_H
#define	MAIN_H 


#include <stdio.h>		// printf(), scanf(), fprintf()
#include <fcntl.h>		// open()
#include <unistd.h>		// close(), off_t , usleep()
#include <sys/mman.h>	// mmap(), munmap()
#include <stdlib.h> 	// exit()
#include <string.h>		// strlen()
#include <time.h>


#include "ieb.h"
#include "led.h"
#include "dot.h"
#include "fnd.h"
#include "clcd.h"
#include "keypad.h"



typedef enum {
	FALSE = 0,
	TRUE  = 1
} truth_t;

typedef enum {
	ERROR	= -1,
	SUCCESS	= 0
} error_t;


int main();
short * mapper(off_t offset, int prot);
void unmapper();
void emergency_closer();

truth_t logic();

void input_mode();

truth_t game_set();
void game_start();
void in_game();
void start_allDevice();

int* get_digits();
unsigned long connectDigits();
unsigned long keypad_input_digits();




#endif
