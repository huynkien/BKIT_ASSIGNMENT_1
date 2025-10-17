/*
 * software_timer.h
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"
#include "tim.h"

void timer_init(void);

void timer3_set(uint8_t index, uint16_t duration);
uint8_t timer3_isExpired(uint8_t index);
void timer3_run(void);


#endif /* INC_SOFTWARE_TIMER_H_ */
