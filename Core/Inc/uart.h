/*
 * fsm.h
 *
 *  Created on: Nov 22, 2024
 *      Author: ADMIN
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "timer.h"

extern ADC_HandleTypeDef hadc1;

extern UART_HandleTypeDef huart2;

#define MAX_BUFFER_SIZE 30

uint8_t temp;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t buffer_flag;
uint8_t index_buffer;
uint8_t buffer_count;

void command_parser_fsm();
void uart_communication_fsm();

#endif /* INC_UART_H_ */
