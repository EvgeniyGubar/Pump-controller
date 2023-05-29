/*
 * Buttons.c
 *
 * Created: 23.01.2022 20:15:25
 *  Author: Evgeniy
 */ 


#include "Buttons.h"

#define POWER_BUT	(!(PINB&(1<<0)))
#define TIMER_BUT	(!(PINB&(1<<1)))

#define BUTTON_PRESS		(PINB&0x03)		//���� ������ ���� �� ���� ������			

/************************************************************************/
/*			������� ������ ������, ���������� �� �������� �����         */
/************************************************************************/
enum ButtonValues GetButtonPress(void){
	
 	volatile static uint16_t time = 0;           //����� ��������� ������� �������� ������
	volatile static uint16_t push_count=0;       //������� ��������� ������, ���� ���������
	static uint8_t push_flag = 0;
 	static uint8_t button_count = 0;
	static uint8_t button_state = 0; 
	
 	if(GetTick() != time)
 	{
 		if(BUTTON_PRESS != 0x03 )
 		{
 			if (button_count < 5) button_count++;	//������� ��������� ��� ������� � ��
 			if (button_count == 5)
 			{
				button_state = BUTTON_PRESS;		//������������� ���������� ������
  				if (push_count < 200) push_count++;
 				if (push_count == 199)		//������������ ��� ����������
 				{
 					push_flag = 1;
 					switch(button_state)
 					{
	 					case 0x02: return POWER_BUTTON_PUSH; break;
	 					case 0x01: return TIMER_BUTTON_PUSH; break;
	 					default: break;
 					}
 				}
			}
 		}
 		else
 		{
 			if (button_count > 0)	//������� ��������� ��� �������
			{
				 button_count--; 
				 push_count = 0;
				 if (button_count == 0)	//�������� ������������ ��� ��������� ������
				 {
					 if (push_flag == 1)	push_flag = 0;		//���� ���� ������������ ��� ��������� ������, �� ������� �� �������
					 else	//����� - ���������� �������� �������
					 {
						 switch(button_state)	//���� ��������� ��������������� ��� �������, �� ������ ��������� ��� �������
						 {
							 case 0x02: return POWER_BUTTON; break;
							 case 0x01: return TIMER_BUTTON; break;
							 default: break;
						 }
						 button_state = 0;
					 }
				 }
			}
		}
		time = GetTick();
 	}	
	return BUTTON_NONE;
}