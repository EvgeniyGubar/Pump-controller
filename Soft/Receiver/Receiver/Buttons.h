/*
 * Buttons.h
 *
 * Created: 23.01.2022 20:14:43
 *  Author: Evgeniy
 */ 

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <avr/io.h>
#include "SysTick_Indication.h"

//нумерация кнопок
enum ButtonValues
{
	POWER_BUTTON_PUSH,
	TIMER_BUTTON_PUSH,
	POWER_BUTTON,
	TIMER_BUTTON,
	BUTTON_NONE
};

enum ButtonValues GetButtonPress(void);//Функция опроса кнопок, вызывается из главного цикла

#endif /* BUTTON_H_ */