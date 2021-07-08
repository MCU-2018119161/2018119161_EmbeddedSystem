/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2021-04-17 22:38:49
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-04-17 22:40:29
 */
#include ".\USER\main.h"

/*******************************************

函数名称：CLK_Init()

功 能：时钟设置

参 数：无

返回值 ：无

********************************************/

void Clk_Init(void) 
{

    uchari;

    BCSCTL1=0x00; //XT2振荡器、不分频

    BCSCTL2=SELM_2+SELS; //选择MCLK\SCLK的时钟源为高速时钟,不分频

    do {

        IFG1&=~OFIFG; //清除振荡失效标志

        for(i=0xff; i>0; i--); //等待

    }

    while ((IFG1&OFIFG) !=0); //等待振荡器工作正常

}

/*******************************************

函数名称：PWM_Init

功 能：初始化PWM

参 数：无

返回值 ：无

********************************************/

voiddelay_us(uint time) 
{

    uchari=0;

    while(time--) 
    {

        while((i++) !=8);

        i=0;

    }

}

//延时MS

voiddelay_ms(uchar time_1) 
{
    while(time_1--) delay_us(1000);

}

void PWM_Init() {

    P4DIR=0xff; // P4 PWM输出

    P4SEL=BIT1+BIT2+BIT3+BIT5;

    P4DIR=BIT1+BIT2+BIT3+BIT5;

    TBCCTL1|=OUTMOD_7; //捕获/比较控制寄存器，输出为模式7

    TBCCTL2|=OUTMOD_7; //PWM RESET/SET模式 TBCCTL3|=OUTMOD_7; //TBCCRn--复位

    TBCCTL5|=OUTMOD_7; //TBCL0--置位

    TBCCR1=0; //P4.1 L298N IN1

    TBCCR2=0; //P4.2 IN2

    TBCCR3=0; //P4.3 IN3

    TBCCR5=0; //P4.5 IN4

    TBCCR0=1000;

    TBCTL |=TBSSEL_1+MC_1; //TIMER_B工作于增计数方式 
}

voidall_init(void) 
{

    PWM_Init();

    Clk_Init();

    P2DIR=0x00; // P2.0---P2.4电压比较结果输入 P1DIR|=BIT0+BIT1; //将P3口切换为输出状态

    P1OUT|=BIT0+BIT1; //ENA，ENB

}

void STRAIGHT(void) //------------全速前进
{

    TBCCR2=400;

    TBCCR1=0;

    TBCCR5=350;

    TBCCR3=0;

}

void LEFT(void) //--------------左转
{

    TBCCR2=0;

    TBCCR1=250;

    TBCCR5=250;

    TBCCR3=0;

}

void RIGHT(void) //---------------右转
{

    TBCCR2=250;

    TBCCR1=0;

    TBCCR5=0;

    TBCCR3=250;

}

void LEFTS(void) //---------------左转微调 
{

    TBCCR2=100;

    TBCCR1=0;

    TBCCR5=250;

    TBCCR3=0;

}

void RIGHTS(void) //-------------------右转微调 {

    TBCCR2=250;

    TBCCR1=0;

    TBCCR5=100;

    TBCCR3=0;

}

void STOP(void) //---------------------停止 {

    TBCCR1=0;

    TBCCR2=0;

    TBCCR3=0;

    TBCCR5=0;

}

void circle(void) {

    TBCCR2=250;

    TBCCR1=0;

    TBCCR5=0;

    TBCCR3=200;

}

//////////////////////////////////////////////////////////////////

2逻辑判断程序： #include "main.h"

void judge(void) {

    while(1) {

        if((data&0x01)==0x01) //左优先

            {

            delay_us(100);

            if((data&0x01)==0x01) {

                //LEFT();

                //delay_ms(10);

                STRAIGHT(); //冲出一段距离

                delay_ms(10);

                LEFT(); //左转到中间灯离开黑线

                delay_ms(35); //时间自己微调

                while(1) {

                    LEFT();

                    if((data&0x04)==0x04) {

                        delay_us(100);

                        if((data&0x04)==0x04) //转到中间的灯在线上 break;

                    }

                }

                //STRAIGHT();

                break;

            }

            break;

        }

        if((data&0x04)==0x04) //优先级2 2黑 直走 {

        delay_us(100);

        if((data&0x04)==0x04) {

            STRAIGHT();

            break;

        }

        break;

    }

    if((data&0x10)==0x10) //优先级3 右转 5黑 {

    delay_us(100);

    if((data&0x10)==0x10) {

        STRAIGHT();

        delay_ms(5);

        while(1) {

            RIGHT();

            if((data&0x04)==0x04) {

                delay_us(10);

                if((data&0x04)==0x04) break;

            }

        }

        //STRAIGHT();

        break;

    }

    break;

}

if((data&0x02)==0x02) //

    {

    delay_us(100);

    if((data&0x02)==0x02) LEFTS（）； break；
}

if((data&0x08)==0x08) //

    {

    delay_us(100);
    左转微调 2黑右转微调 4黑 if((data&0x08)==0x08) RIGHTS();

    break
}

if((data&0x1f)==0x00) //12345白

    {

    delay_ms(10);

    if((data&0x1f)==0x00) {

        while(1) {

            circle();

            if((data&0x1f) !=0x00) {

                delay_us(100) //

                if((data&0x1f) !=0x00) break;
            }

        }

        break;

    }

    break;

}

else {

    STOP();

    break;

}

转到不为全白;
优先级4 原地转圈°
}
}