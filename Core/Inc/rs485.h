/*
 * rs485.h
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

#ifndef INC_RS485_H_
#define INC_RS485_H_

#include "main.h"
#include "usart.h"
#include "global.h"
#include "software_timer.h"
#include "crc16.h"

void rs485_fsm_init();
void rs485_fsm();

void rs485_init();
void rs485_idle();
void rs485_transmit();
void rs485_receive();
void rs485_control_waiting();

uint8_t crc_checking();

void rs485_prepare_transmit(uint8_t *data, uint8_t length);
uint8_t rs485_is_frame_ready();
void rs485_get_received_frame(uint8_t *buffer, uint8_t *length);

void RS485_UART_Callback(UART_HandleTypeDef *huart);


#endif /* INC_RS485_H_ */
