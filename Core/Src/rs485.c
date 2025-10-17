/*
 * rs485.c
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

#include "rs485.h"

uint8_t is_received_character = 0;
uint8_t receive_byte_buffer = 0;
uint8_t receive_buffer[256] = {0x00};
uint8_t receive_index = 0;
uint8_t receive_character_flag = 0;

uint8_t transmit_buffer[256] = {0x00};
uint8_t transmit_index = 0;
uint8_t transmit_length = 0;
uint8_t transmit_request = 0;

enum rs485_state {
	RS485_START,
	RS485_INIT,
	RS485_IDLE,
	RS485_TRANSMIT,
	RS485_RECEIVE,
	RS485_CONTROL_AND_WAITING
};

uint8_t rs485_status = RS485_START;

enum frame_receive_stae {FRAME_OK, FRAME_NOK};
uint8_t frame_status = FRAME_NOK;

void rs485_fsm_init() {
	HAL_UART_Receive_IT(&huart3, &receive_byte_buffer, 1);
}

uint8_t crc_checking() {
	if (receive_index >= 2) {
		uint16_t crc = crc16(receive_buffer, receive_index - 2);
		uint8_t crc_high = (crc >> 8) & 0xFF;
		uint8_t crc_low = crc & 0xFF;
		if (crc_low == receive_buffer[receive_index - 2] && crc_high == receive_buffer[receive_index - 1])
		{
			return 1;
		}
	}
	return 0;
}



void rs485_fsm() {
	switch(rs485_status) {
		case RS485_START:
			if (1) {
				rs485_status = RS485_INIT;
			}
			break;

		case RS485_INIT:
			rs485_init();

			// receive character -> INIT
			if (receive_character_flag == 1) {
				receive_character_flag = 0;
				timer_set(RS485_T35_TIMER, RS485_T35);
			}

			// t3.5 expired -> IDLE
			if (timer_isExpired(RS485_T35_TIMER)) {
				timer_set(RS485_T35_TIMER, RS485_T35);
				rs485_status = RS485_IDLE;
			}
			break;

		case RS485_IDLE:
			// ready to receive or to emit
			rs485_idle();

			//received character -> RECEIVE
			if(receive_character_flag == 1) {
				receive_character_flag = 0;
				timer_set(RS485_T15_TIMER, RS485_T15);
				timer_set(RS485_T35_TIMER, RS485_T35);
				rs485_status = RS485_RECEIVE;
			}

			// Demand of emission -> TRANSMIT
			if(transmit_request == 1) {
				rs485_status = RS485_TRANSMIT;
			}

			break;

		case RS485_TRANSMIT:
			rs485_transmit();

			// emission done -> reset t3.5
			if(transmit_index >= transmit_length) {
				transmit_request = 0;
				timer_set(RS485_T35_TIMER, RS485_T35);
			}

			if(timer_isExpired(RS485_T35_TIMER)) {
				rs485_status = RS485_IDLE;
			}
			break;

		case RS485_RECEIVE:
			rs485_receive();

			if (receive_character_flag == 1) {
				receive_character_flag = 0;
				timer_set(RS485_T15_TIMER, RS485_T15);
				timer_set(RS485_T35_TIMER, RS485_T35);
			}

			if(timer_isExpired(RS485_T15_TIMER)) {
				rs485_status = RS485_CONTROL_AND_WAITING;
			}
			break;

		case RS485_CONTROL_AND_WAITING:

			rs485_control_waiting();

			if (receive_character_flag == 1) {
				receive_character_flag = 0;
				frame_status = FRAME_NOK;
			}

			// t3.5 expired -> return to idle
			if (timer_isExpired(RS485_T35_TIMER)) {
				// check crc and process frame
				if(crc_checking()) {
					frame_status = FRAME_OK;
				} else {
					frame_status = FRAME_NOK;
				}

				// reset receive buffer
				receive_index = 0;

				// return to IDLE
				timer_set(RS485_T35_TIMER, RS485_T35);
				rs485_status = RS485_IDLE;
			}

			break;

		default:
			break;

	}

}

void rs485_init() {
	// init t3.5
	timer_set(RS485_T35_TIMER, RS485_T35);

	// reset
	receive_index = 0;
	transmit_index = 0;
	transmit_request = 0;
	frame_status = FRAME_NOK;
}

void rs485_idle() {
	// IDLE - ready to receive or to emit

}

void rs485_transmit() {
	if(transmit_index < transmit_length) {
		HAL_UART_Transmit(&huart3, &transmit_buffer[transmit_index], 1, 100);
		transmit_index++;
	}
}

void rs485_receive() {
	// Do nothing
}

void rs485_control_waiting() {
	// Do nothing
}



void rs485_prepare_transmit(uint8_t *data, uint8_t length) {
	for(uint8_t i = 0; i < length; i++) {
		transmit_buffer[i] = data[i];
	}

	// update crc
	uint16_t crc = crc16(transmit_buffer, length);
	transmit_buffer[length] = crc & 0xFF;
	transmit_buffer[length + 1] = (crc >> 8) & 0xFF;
	transmit_length = length + 2;
	transmit_index = 0;

	// emission demand
	transmit_request = 1;
}

uint8_t rs485_is_frame_ready() {
	return (frame_status == FRAME_OK) ? 1 : 0;
}

void rs485_get_received_frame(uint8_t *buffer, uint8_t *length) {
	if(frame_status == FRAME_OK) {
		// Not include CRC
		*length = receive_index - 2;
		for(uint8_t i = 0; i < *length; i++) {
			buffer[i] = receive_buffer[i];
		}
		frame_status = FRAME_NOK; // reset flag
	} else {
		*length = 0;
	}
}

void RS485_UART_Callback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
		receive_buffer[receive_index++] = receive_byte_buffer;

		// Received flag
		receive_character_flag = 1;

		// Receive next byte
		HAL_UART_Receive_IT(huart, &receive_byte_buffer, 1);
	}
}
