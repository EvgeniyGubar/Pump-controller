/*
 * Receiver.c
 *
 * Created: 21.08.2015 17:31:09
 * Modify:  03.06.2022 15:37:08
 * Author: Евгений
 * Приемный модуль управления скважным насосом. Принимает команды от приемного модуля,
 * если команда опознается, то выполняется и ретранслируется ответ для обратной связи.
 * Имеет 2 кнопки управления: кнопка 1 - старт/стоп, кнопка 2 - таймер +5 минут;
 * При включении, выключении издается соответствующий тональный сигнал, кнопка 1 меняет цвет подсветки,
 * на индикаторе появляется надпись ON или OFF на 5 секунд
 * При нажатии кнопки 2 значение таймера инкрементируется на 5 минут и результат выводится на идикатор сменяя
 * надпись ON/OFF.
 * При нажатии кнопки 1 (вкл) отсчет таймера запускается и на идикаторе отображается оставшееся время до
 * автоматического отключения.
 * При нажатии кнопки 1 (выкл) таймер сбрасывается и на индикаторе отображается OFF на 5 секунд
 * Установить таймер можно как до запуска, так и после запуска.
 
	PORTB:                  PORTD:
	0 - кнопка 1			0 - Управление реле	
	1 - кнопка 2            1 - ST (сдвиговый регистр)
	2 - СЕ					2 - Buzzer
	3 - CSN					3 - анод 3 индикатора
	4 - IRQ					4 - анод 2 индикатора
	5 - MISO				5 - анод 1 индикатора
	6 - MOSI				6 - светодиод красный
	7 - SCK                                                             */
/************************************************************************/

#define F_CPU 6000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SPI2313.h"
#include "NRF2401.h"
#include "buttons.h"

#define RELAY_ON	(PORTD|=(1<<0))
#define RELAY_OFF	(PORTD&=~(1<<0))
#define RELAY_CHECK (PIND&(1<<0))

#define BUZZ_ON		(PORTD|=(1<<2))
#define BUZZ_OFF	(PORTD&=~(1<<2))

#define RED_OFF		(PORTD|=(1<<6))
#define RED_ON		(PORTD&=~(1<<6))

#define ANOD_RES	(PORTD|=(1<<3)|(1<<4)|(1<<5))
#define ANOD_3		(PORTD&=~(1<<5))
#define ANOD_2		(PORTD&=~(1<<4))
#define ANOD_1		(PORTD&=~(1<<3))

#define nasos		(PORTD&(1<<PB5))	//проверка на включенный насос

uint8_t received_data;
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

enum Command
{
	START = 0x1A,
	STOP = 0x2B,
	PLUS_FIVE = 0x3C
};

void button_processing();
void run_command(enum Command);


/************************************************************************/
/*                                                                      */
/************************************************************************/	
int main(void)
{  
	CLKPR=(1<<CLKPCE);
	CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
	/*
		PORTB:                  PORTD:
		0 - кнопка 1			0 - Управление реле
		1 - кнопка 2            1 - ST (сдвиговый регистр)
		2 - СЕ					2 - Buzzer
		3 - CSN					3 - анод 3 индикатора
		4 - IRQ					4 - анод 2 индикатора
		5 - MISO				5 - анод 1 индикатора
		6 - MOSI				6 - светодиод красный
		7 - SCK
	*/	
	DDRD|=(1<<0)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6);	//пины на выход
	PORTB|=(1<<0)|(1<<1);								//пины на вход (подтяжки для кнопок)
	RED_OFF;											//управление катодом
	 	
 	/*Таймер для динамической индикации*/
	// F = fclock/(N*(1+OCR0A))
//  	OCR0A = 110;			//примерно 210 Гц, по 70 Гц на индикатор, на частоте кварца 6 Мгц
//  	TCCR0A|=(1<<WGM01);		//режим СТС, счет вверх до OCR0A
//  	TCCR0B|=(1<<CS02);		//делитель 256. запускаем таймер
//  	TIMSK|=(1<<OCIE0A);		//включаем прерывание
	
	ACSR |= (1<<ACD);		//выключили АЦП

	GIMSK|=(1<<PCIE0);		//включаем внешнее прерывание для nrf24
	PCMSK0|=(1<<PCINT4);	//разрешаем прерывание от модуля
	
	SysTick_init();	
	SPI_MasterInit();	
	HC595_init();
 	nrf24_init_config(2,1);			//МОДУЛЬ ПЕРЕХОДИТ В РЕЖИМ ПРИЕМНИКА. КАНАЛ 2. НАГРУЗКА 1 БАЙТ
 	nrf24_tx_address(tx_address);	//Загрузка адресов устройст 
 	nrf24_rx_address(rx_address);

    sei();
	
	uint16_t delay = GetTick();
	
  while(1)
	{
		button_processing();
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void button_processing()
{
	enum ButtonValues state = GetButtonPress();
	if (state != BUTTON_NONE)
	{
		switch(state)
		{
			case POWER_BUTTON:	{
					if(RELAY_CHECK == 0) run_command(START);
					else run_command(STOP); //если выключено
				}; break;
				
			//case POWER_BUTTON_PUSH:{}; break;
			case TIMER_BUTTON:	{
					run_command(PLUS_FIVE);
				}; break;
				
			case TIMER_BUTTON_PUSH:	{
				
				}; break;
			default: break;
		};
	}
}

void run_command(enum Command data)
{
	switch (data)
	{
		case START:	{
				//beep;
				//display(ON);
				RELAY_ON;
				RED_ON;
			}break;
		case STOP:	{
				//beep;
				RELAY_OFF;
				RED_OFF;
			}break;
		case PLUS_FIVE:	{
				RED_ON; 
			}break;

		default: break;
	}
}

ISR(PCINT0_vect)	//обработка прерывания от модуля
{
	if(nrf24_dataReady())			//проверка флага приема
	{
		nrf24_getData(&received_data);	//считывание данных в буфер
		run_command(received_data);		//выполнение команды
	}
}

// ISR (TIMER0_COMPA_vect)
// {
// 	
// }







//void beep(void)
//{
//uint8_t i;
//switch (command)
//{
//case 0x1A:{    //сигнал включения
//for (i=0;i<50;i++)    //500 Гц 80мс
//{
//BUZZ_ON;
//_delay_ms(1);
//BUZZ_OFF;
//_delay_ms(1);
//};
//for (i=0;i<100;i++)   //1000 гц 80мс
//{
//BUZZ_ON;
//_delay_us(500);
//BUZZ_OFF;
//_delay_us(500);
//};
//}; break;
//
//case 0x2B:{
//for (i=0;i<100;i++)   //1000 гц 80мс
//{
//BUZZ_ON;
//_delay_us(500);
//BUZZ_OFF;
//_delay_us(500);
//};
//}; break;
//
//case 0x3C:{
//for (i=0;i<100;i++)   //1000 гц 80мс
//{
//BUZZ_ON;
//_delay_us(500);
//BUZZ_OFF;
//_delay_us(500);
//};
//}; break;
//
//case 0x4D:{         //сигнал выключения
//for (i=0;i<100;i++)   //1000 гц 80мс
//{
//BUZZ_ON;
//_delay_us(500);
//BUZZ_OFF;
//_delay_us(500);
//};
//for (i=0;i<50;i++)    //500 Гц 80мс
//{
//BUZZ_ON;
//_delay_ms(1);
//BUZZ_OFF;
//_delay_ms(1);
//};
//}; break;
//
//}
//}