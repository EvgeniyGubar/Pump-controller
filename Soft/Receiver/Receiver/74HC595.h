/*
 * _HC595.h
 *
 * Created: 28.07.2022 20:23:37
 *  Author: Evgeniy
 */
#ifndef HC595_H_
#define HC595_H_

#include <avr/io.h>

//ножки интерфейса SPI инициализируются в SPI2313.c
#define DS		5	//MOSI spi
#define SHCP	7	//SCK spi
#define STSP	1	//вход помещения данных в регистр хранения

//порт интерфейса  SPI
#define HC595_DDR	DDRD
#define HC595_PORT	PORTD

void HC595_init(void);
void HC595_send_data(uint8_t);

#endif /* HC595_H_ */