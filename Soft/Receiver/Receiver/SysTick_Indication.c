/*
 * SysTick.c
 *
 * Created: 11.05.2022 14:25:21
 *  Author: User
 */ 

#include "SysTick_Indication.h"

#define ANOD_RES	(PORTD|=(1<<3)|(1<<4)|(1<<5))
#define ANOD_3		(PORTD&=~(1<<5))
#define ANOD_2		(PORTD&=~(1<<4))
#define ANOD_1		(PORTD&=~(1<<3))

static volatile uint16_t TimeMs = 0;

/************************************************************************/
uint16_t GetTick(void)
{
	static volatile uint16_t MirrorTimeMs;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)	//����������� ����������� ��������, ����� ����� �� ������ ��������� � ������� ��������� ����������.
	{
		MirrorTimeMs = TimeMs;		
	}
	return MirrorTimeMs;
}

/************************************************************************/
ISR (TIMER1_COMPA_vect)
{
	TimeMs += 5;
 	static uint8_t digit;
 	
 	digit++;
 	
 	switch(digit)
 	{
	 	case 1: {
		 	ANOD_RES;
		 	HC595_send_data(0);
		 	ANOD_1;
	 	}; break;
	 	case 2: {
		 	ANOD_RES;
		 	HC595_send_data(1);
		 	ANOD_2;
	 	}; break;
	 	case 3: {
		 	ANOD_RES;
		 	HC595_send_data(2);
		 	ANOD_3;
		 	digit = 0;
	 	}; break;
 	}
}

/************************************************************************/
void SysTick_init(void)
{
	// ��������� ������� �� �������� � 5 �� � ������������ ���������
	// F = fclock/(N*(1+OCR1A))
	OCR1A = 29999;			//200 �� - 5��
	TCCR1B|=(1<<WGM12);		//����� ���, ���� ����� �� OCR1A
	TCCR1B|=(1<<CS10);		//��� ��������, ��������� ������
	TIMSK|=(1<<OCIE1A);		//�������� ����������
}