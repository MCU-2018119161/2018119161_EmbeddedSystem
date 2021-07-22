#include "delay.h"


void delay_ms(uint16_t ms)
{
	SysTick->CTRL = 0;
	SysTick->CTRL &= ~(0x1<<2);		//�ⲿʱ��Դ    72/8 = 9MHZ
	
	SysTick->LOAD = 9*1000*ms;		//���뼶��ʱ	
	
	SysTick->VAL = 0;				//дVAL�Ĵ���  д�κ�������
	
	SysTick->CTRL |= 0x1<<0;		//ʹ�ܶ�ʱ��
	
	while((SysTick->CTRL & (0x1 <<16)) == 0);
	
	SysTick->CTRL &= ~(0x1<<0);		//������������0�رն�ʱ��
}

//ʵ��΢����ʱ����
void delay_us(uint16_t us)
{
	SysTick->CTRL = 0;
	SysTick->CTRL &= ~(0x1<<2);		//�ⲿʱ��Դ    72/8 = 9MHZ
	
	SysTick->LOAD = 9*us;		//΢�뼶��ʱ
	
	SysTick->VAL = 0;				//дVAL�Ĵ���  д�κ�������
	
	SysTick->CTRL |= 0x1<<0;		//ʹ�ܶ�ʱ��
	
	while((SysTick->CTRL & (0x1 <<16)) == 0);
	
	SysTick->CTRL &= ~(0x1<<0);		//������������0�رն�ʱ��
}
