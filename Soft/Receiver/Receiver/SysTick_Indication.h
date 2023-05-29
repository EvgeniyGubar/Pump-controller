/*
 * SysTick.h
 *
 * Created: 11.05.2022 14:25:05
 *  Author: User
 */ 


#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "74HC595.h"

void SysTick_init(void);
uint16_t GetTick(void);


#endif /* SYSTICK_H_ */