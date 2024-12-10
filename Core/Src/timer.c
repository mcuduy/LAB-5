/*
 * software_timer.c
 *
 *  Created on: Nov 22, 2024
 *      Author: ADMIN
 */

#include "timer.h"
int timer0_counter = 0;
unsigned char timer0_flag = 0;

int timer1_counter = 0;
unsigned char timer1_flag = 0;

void setTimer0(int duration) {
	timer0_counter = duration / TIMER_CYCLE;
	timer0_flag = 0;
}

void setTimer1(int duration) {
	timer1_counter = duration / TIMER_CYCLE;
	timer1_flag = 0;
}

void timer_run() {
	if (timer0_counter > 0) {
		timer0_counter--;
		if (timer0_counter <= 0) {
			timer0_flag = 1;
		}
	}
	if (timer1_counter > 0) {
		timer1_counter--;
		if (timer1_counter <= 0) {
			timer1_flag = 1;
		}
	}

}

unsigned char getTimer0Flag() {
	return timer0_flag;
}

unsigned char getTimer1Flag() {
	return timer1_flag;
}
