/*
 * Transceiver.c
 *
 * Modify: 03.06.2022 16:13:18
 * Author : User
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "SPI2313.h"
#include "NRF2401.h"

#define On		!(PIND&(1<<PD0))
#define Timer	!(PIND&(1<<PD1))
#define State	!(PIND&(1<<PD2))
#define Off		!(PIND&(1<<PD4))
#define beep_on	(PORTD|=(1<<PB5))   //звук
#define beep_off (PORTD&=~(1<<PB5))  //звук


uint8_t data_array;
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

void beep(void)
{
uint8_t i;
switch (data_array)
{
	case 0x1A:{    //сигнал включения
		for (i=0;i<50;i++)    //500 Гц 80мс
		{
			beep_on;
			_delay_ms(1);
			beep_off;
			_delay_ms(1);
		};
		for (i=0;i<100;i++)   //1000 гц 80мс
		{
			beep_on;
			_delay_us(500);
			beep_off;
			_delay_us(500);
		};
	}; break;
	case 0x2B:{
		for (i=0;i<100;i++)   //1000 гц 80мс
		{
			beep_on;
			_delay_us(500);
			beep_off;
			_delay_us(500);
		};
	}; break;
	case 0x3c:{
		for (i=0;i<100;i++)   //1000 гц 80мс
		{
			beep_on;
			_delay_us(500);
			beep_off;
			_delay_us(500);
		};
	}; break;
	case 0x4D:{         //сигнал выключения
		for (i=0;i<100;i++)   //1000 гц 80мс
		{
			beep_on;
			_delay_us(500);
			beep_off;
			_delay_us(500);
		};
		for (i=0;i<50;i++)    //500 Гц 80мс
		{
			beep_on;
			_delay_ms(1);
			beep_off;
			_delay_ms(1);
		};
	}; break;
	
   case 0x00:{              //fail
		for (i=0;i<100;i++)    //500 Гц 80мс  fail звук
		{
			beep_on;
			_delay_ms(1);
			beep_off;
			_delay_ms(1);
		};
	}
   }
}

ISR (PCINT2_vect)   //обработка кнопок
{	
	_delay_ms(50);
		
	if(On)              //включение
	{
		data_array=0x1A;
		while(On){}
		nrf24_send(&data_array);
	};
	
	if(Timer)            //Установка времени по 10 минут
	{
		data_array=0x2B;
		while(Timer){}
		nrf24_send(&data_array);
	};
	
	if(State)            //Запрос состояния
	{
		data_array=0x3C;
		while(State){}
		nrf24_send(&data_array);
	};
	
	if(Off)             //Выключение
	{
		data_array=0x4D;
		while(Off){}
		nrf24_send(&data_array);		
	};
}	
	

ISR(PCINT0_vect)  //обработка прерывания от модуля и обработка сигналов обратной связи
{
	static uint8_t repeat;
	uint8_t status;
	
	status = nrf24_lastMessageStatus();

	if(status == NRF24_TRANSMISSON_OK)
	{
		nrf24_write_reg(STATUS,(1<<TX_DS));
		nrf24_powerDown();  //засыпает модуль
		beep();
		data_array=0;
		repeat=0;            
	}
	
	else if(status == NRF24_MESSAGE_LOST)
	{
		if (repeat!=25)
		{
			nrf24_write_reg(STATUS,(1<<MAX_RT));
			SPI_MasterTransmit(REUSE_TX_PL);     //отправляем пакет снова
			nrf24_ce_low;
			nrf24_ce_high;
			repeat++;			
		}
		else
		{
			nrf24_powerDown();  //засыпает модуль
			repeat=0;
			data_array=0;
			beep();
		};
	}
}	
	
	
int main(void)
{
	CLKPR=(1<<CLKPCE);
	CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (1<<CLKPS1) | (0<<CLKPS0);  //делим частоту на 4
	
	DDRD|=(1<<PB5);		//пин управления звуком на выход
	PORTD|=(1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD6);      //Все пины с подтяжкой для экономии энергии
	PORTB|=(1<<PB2)|(1<<PB1);	//с подтяжкой для экономии энергии
	PORTA=0xff;					//с подтяжкой для экономии энергии
	
	SPI_MasterInit();
	nrf24_init_config(2,1);		//Инициализация и конфигурация. Канал #2 , Полезная нагрузка: 1
	
	/* Загрузка адресов устройст */
	nrf24_tx_address(tx_address);
	nrf24_rx_address(rx_address);
	    
	GIMSK|=(1<<PCIE2)|(1<<PCIE0);									//разрешение внешних прерываний от кнопок (2) от nrf24 (0)
	PCMSK2|=(1<<PCINT11)|(1<<PCINT12)|(1<<PCINT13)|(1<<PCINT15);	//разрешаем прерывание на кнопки
	PCMSK0|=(1<<PCINT0);											//разрешаем прерывание от модуля
		
	PRR|=(1<<PRTIM1)|(1<<PRTIM0)|(1<<PRUSART);  //выключили питание таймеров и usart
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); //если спать - то на полную 
	
	sei();  //глоб. прерывания
	
	nrf24_powerDown();  //засыпает модуль

while(1)
    {
		sleep_enable();	//теперь приготовимся сами
		sleep_cpu();	//спать!
	}
}