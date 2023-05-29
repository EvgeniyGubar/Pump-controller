/*
 * Buttons.c
 *
 * Created: 23.01.2022 20:15:25
 *  Author: Evgeniy
 */ 


#include "Buttons.h"

#define POWER_BUT	(!(PINB&(1<<0)))
#define TIMER_BUT	(!(PINB&(1<<1)))

#define BUTTON_PRESS		(PINB&0x03)		//если нажата хотя бы одна кнопка			

/************************************************************************/
/*			Функция опроса кнопок, вызывается из главного цикла         */
/************************************************************************/
enum ButtonValues GetButtonPress(void){
	
 	volatile static uint16_t time = 0;           //время последней выборки входного вывода
	volatile static uint16_t push_count=0;       //счетчик удержания кнопок, флаг удержания
	static uint8_t push_flag = 0;
 	static uint8_t button_count = 0;
	static uint8_t button_state = 0; 
	
 	if(GetTick() != time)
 	{
 		if(BUTTON_PRESS != 0x03 )
 		{
 			if (button_count < 5) button_count++;	//счетчик дребезгов при нажатии в мс
 			if (button_count == 5)
 			{
				button_state = BUTTON_PRESS;		//зафиксировали сосотояние кнопок
  				if (push_count < 200) push_count++;
 				if (push_count == 199)		//срабатывание при удержанием
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
 			if (button_count > 0)	//счетчик дребезгов при отжатии
			{
				 button_count--; 
				 push_count = 0;
				 if (button_count == 0)	//короткое срабатывание при спадающем фронте
				 {
					 if (push_flag == 1)	push_flag = 0;		//если было срабатывание при удержании кнопки, то выходим из функции
					 else	//иначе - обработать короткое нажатие
					 {
						 switch(button_state)	//если состояние зафиксировалось при нажатии, то кнопка сработает при отжатии
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