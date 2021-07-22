/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2021-05-29 18:18:38
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-06-19 11:27:11
 */
#ifndef __CORE_DELAY_H
#define __CORE_DELAY_H

#include "stm32l4xx.h"

/* 锟斤拷取锟节猴拷时锟斤拷频锟斤拷 */
#define GET_CPU_ClkFreq()       HAL_RCC_GetSysClockFreq()
#define SysClockFreq            (80000000)

/* 为锟斤拷锟斤拷使锟矫ｏ拷锟斤拷锟斤拷时锟斤拷锟斤拷锟节诧拷锟斤拷锟斤拷CPU_TS_TmrInit锟斤拷锟斤拷锟斤拷始锟斤拷时锟斤拷锟斤拷拇锟斤拷锟斤拷锟�
   锟斤拷锟斤拷每锟轿碉拷锟矫猴拷锟斤拷锟斤拷锟斤拷锟绞硷拷锟揭伙拷椤�
   锟窖憋拷锟斤拷值锟斤拷锟斤拷为0锟斤拷然锟斤拷锟斤拷main锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷CPU_TS_TmrInit锟缴憋拷锟斤拷每锟轿讹拷锟斤拷始锟斤拷 */  
#define CPU_TS_INIT_IN_DELAY_FUNCTION   1

/*******************************************************************************
 * 锟斤拷锟斤拷锟斤拷锟斤拷
 ******************************************************************************/
uint32_t CPU_TS_TmrRd(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);

//使锟斤拷锟斤拷锟铰猴拷锟斤拷前锟斤拷锟斤拷锟饺碉拷锟斤拷CPU_TS_TmrInit锟斤拷锟斤拷使锟杰硷拷锟斤拷锟斤拷锟斤拷锟斤拷使锟杰猴拷CPU_TS_INIT_IN_DELAY_FUNCTION
//锟斤拷锟斤拷锟绞敝滴�8锟斤拷
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define HAL_Delay(ms)     CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)


#endif /* __CORE_DELAY_H */

