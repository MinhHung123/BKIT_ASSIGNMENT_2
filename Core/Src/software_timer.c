/*
 * software_timer.c
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

#define MAX_TIMER   5

#include "software_timer.h"

uint16_t timer_counter[MAX_TIMER];
uint8_t  timer_flag[MAX_TIMER];


void timer_init(void) {
	HAL_TIM_Base_Start_IT(&htim3);
}

void timer_set(uint8_t index, uint16_t duration) {
	timer_counter[index] = duration;
	timer_flag[index] = 0;
}

uint8_t timer_isExpired(uint8_t index) {
	if (timer_flag[index] == 1) {
		return 1;
	}
	return 0;
}

void timer_run(void) {
	for (uint8_t i = 0; i < MAX_TIMER; i++) {
		if (timer_counter[i] > 0) {
			timer_counter[i]--;
			if (timer_counter[i] <= 0) {
				timer_flag[i] = 1;
			}
		}
	}
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//
//	if (htim->Instance == TIM3) {
//		timer_run();
//	}
//}
