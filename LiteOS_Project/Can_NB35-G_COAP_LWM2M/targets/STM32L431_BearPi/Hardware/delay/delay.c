#include "delay.h"


void delay_ms(uint16_t ms)
{
	SysTick->CTRL = 0;
	SysTick->CTRL &= ~(0x1<<2);		//外部时钟源    72/8 = 9MHZ
	
	SysTick->LOAD = 9*1000*ms;		//毫秒级延时	
	
	SysTick->VAL = 0;				//写VAL寄存器  写任何数清零
	
	SysTick->CTRL |= 0x1<<0;		//使能定时器
	
	while((SysTick->CTRL & (0x1 <<16)) == 0);
	
	SysTick->CTRL &= ~(0x1<<0);		//计数器倒数到0关闭定时器
}

//实现微妙延时功能
void delay_us(uint16_t us)
{
	SysTick->CTRL = 0;
	SysTick->CTRL &= ~(0x1<<2);		//外部时钟源    72/8 = 9MHZ
	
	SysTick->LOAD = 9*us;		//微秒级延时
	
	SysTick->VAL = 0;				//写VAL寄存器  写任何数清零
	
	SysTick->CTRL |= 0x1<<0;		//使能定时器
	
	while((SysTick->CTRL & (0x1 <<16)) == 0);
	
	SysTick->CTRL &= ~(0x1<<0);		//计数器倒数到0关闭定时器
}
