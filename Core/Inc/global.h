/*
 * global.h
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

#define BAUDRATE		9600
#define BITS_PER_CHAR   10

#define RS485_T15_TIMER 1
#define RS485_T15       1000 / (BAUDRATE / BITS_PER_CHAR) * 1.5

#define RS485_T35_TIMER	2
#define RS485_T35		1000 / (BAUDRATE / BITS_PER_CHAR) * 3.5

#endif /* INC_GLOBAL_H_ */
