/*
 * fsm.c
 *
 *  Created on: Nov 22, 2024
 *      Author: ADMIN
 */

#include "uart.h"

#include <string.h>
#include <stdio.h>

uint8_t temp = 0;                          // Temporarily store character received from UART
uint8_t data_receive[MAX_BUFFER_SIZE];     // Data received from UART
uint8_t data_receive_idx = 0;              // Storage index for received data
uint8_t data[MAX_BUFFER_SIZE];             // Processed data
uint8_t data_for_transmit[MAX_BUFFER_SIZE];// Data string to transmit
uint8_t data_size = 0;                     // Data size

uint32_t last_adc_value = 0;               // Store the last transmitted ADC value
uint32_t initial_adc_value = 0;            // Store the initial ADC value after !RST#
uint8_t send_old_adc_value = 0;            // Flag to indicate sending the old ADC value
uint8_t first_rst_command = 1;             // Flag to indicate the first !RST# command
uint8_t hold_initial_value = 1;            // Flag to hold the initial ADC value

// FSM states
enum DataReceive {
    WAIT_FOR_BEGIN_CHAR,   // Wait for the '!' character
    WRITING_DATA           // Write received data
};
enum DataReceive data_parser = WAIT_FOR_BEGIN_CHAR;

enum DataProcessing {
    WAIT_FOR_RST,          // Wait for the !RST# command
    DATA_TRANSMIT,         // Transmit ADC value
    WAIT_FOR_OK            // Wait for the !OK# command
};
enum DataProcessing state = WAIT_FOR_RST;

// UART data parsing function
void command_parser_fsm() {
    switch (data_parser) {
    case WAIT_FOR_BEGIN_CHAR:
        if (temp == '!') {                          // Detect start character
            data_parser = WRITING_DATA;
            data_receive_idx = 0;
            memset(data_receive, 0, MAX_BUFFER_SIZE); // Clear old data
        }
        break;

    case WRITING_DATA:
        if (temp == '#') {                         // End character
            data_parser = WAIT_FOR_BEGIN_CHAR;
            memcpy(data, data_receive, data_receive_idx); // Save received data into `data`
            data[data_receive_idx] = '\0';         // Null-terminate the string
        } else {
            data_receive[data_receive_idx++] = temp; // Write received data
            if (data_receive_idx >= MAX_BUFFER_SIZE) {
                data_parser = WAIT_FOR_BEGIN_CHAR;  // Handle buffer overflow
            }
        }
        break;

    default:
        break;
    }
}

// UART communication handling function
void uart_communication_fsm() {
    switch (state) {
    case WAIT_FOR_RST:
        if (strcmp((char *)data, "RST") == 0) {    // Detect the !RST# command
            state = DATA_TRANSMIT;
            if (first_rst_command || !hold_initial_value) {
                initial_adc_value = HAL_ADC_GetValue(&hadc1); // Read ADC value as initial
                first_rst_command = 0;             // Clear the flag after the first time
                hold_initial_value = 1;            // Enable holding the initial value
            }
            send_old_adc_value = 1;                // Set flag to send the old ADC value first
            memset(data, 0, MAX_BUFFER_SIZE);      // Clear old command
        }
        break;

    case DATA_TRANSMIT:
        if (send_old_adc_value) {
            // Send the initial ADC value first
            data_size = sprintf((char *)data_for_transmit, "!ADC=%lu#\r\n", initial_adc_value);
            HAL_UART_Transmit(&huart2, data_for_transmit, data_size, 1000);
            send_old_adc_value = 0; // Clear flag after sending
        } else {
            // Keep sending the initial ADC value until !OK# is received
            data_size = sprintf((char *)data_for_transmit, "!ADC=%lu#\r\n", initial_adc_value);
            HAL_UART_Transmit(&huart2, data_for_transmit, data_size, 1000);
        }
        state = WAIT_FOR_OK;
        setTimer0(1500);                           // Set timer to resend data after 3 seconds
        break;

    case WAIT_FOR_OK:
        if (strcmp((char *)data, "OK") == 0) {     // Detect the !OK# command
            state = WAIT_FOR_RST;                  // Return to waiting for the !RST# command
            memset(data, 0, MAX_BUFFER_SIZE);      // Clear old command
            hold_initial_value = 0;                // Disable holding the initial value
        }
        if (getTimer0Flag()) {                     // If timer expires
            state = DATA_TRANSMIT;                 // Resend ADC data
        }
        break;

    default:
        break;
    }
}
