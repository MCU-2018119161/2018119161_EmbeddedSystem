/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
 

#include <stdio.h>
#include "nb_iot/los_nb_api.h"
#include "nb_cmd_ioctl.h"
#include "lcd.h"
#include "bsp_dht11.h"
#include "hc_sr04.h"
#include "E53_ST1.h"


#define OCEAN_IP "49.4.85.232,80"   //华为云入网IP
#define SECURITY_PORT "5684"     //加密接入端口号
#define NON_SECURITY_PORT "5683"  //没有加密的接入端口号
#define DEV_PSKID  "865057044093205"    //设备标识号
#define DEV_PSK  "7ca59f83ca086b9f5d7776f879b8a97b"  //设备密钥

#define cn_buf_len    256   //may be bigger enough
static char s_report_buf[cn_buf_len];
char E53_ST1_send_str[33]={0};  //长度与平台编解码插件数据总长度对应，不含messageid
char E53_ST1_send_hex[66]={0};  //长度与平台编解码插件数据总长度对应，不含messageid

extern hc_sr04_device_t hc_sr04_device1;
extern DHT11_Data_TypeDef DHT11_Data;

void nb_iot_entry(void);

void demo_nbiot_only(void)
{
#if defined(WITH_AT_FRAMEWORK) && defined(USE_NB_NEUL95_NO_ATINY)
    #define AT_DTLS 0
    #if AT_DTLS
    sec_param_s sec;
    sec.setpsk = 1;
    sec.pskid = DEV_PSKID;
    sec.psk = DEV_PSK;
    #endif
    printf("\r\n=====================================================");
    printf("\r\nSTEP1: Init NB Module( NB Init )");
    printf("\r\n=====================================================\r\n");

#if AT_DTLS
    los_nb_init((const int8_t *)OCEAN_IP, (const int8_t *)SECURITY_PORT, &sec);
	LCD_Clear(WHITE);		   
	POINT_COLOR = RED;			
	LCD_ShowString(40, 10, 200, 16, 24, "IoTCluB BearPi");
	LCD_ShowString(50, 50, 200, 16, 24, "E53_ST1_Demo");
	LCD_ShowString(10, 100, 200, 16, 16, "NCDP_IP:");
	LCD_ShowString(80, 100, 200, 16, 16, OCEAN_IP);
	LCD_ShowString(10, 150, 200, 16, 16, "NCDP_PORT:");
	LCD_ShowString(100, 150, 200, 16, 16, SECURITY_PORT);
#else
    los_nb_init((const int8_t *)OCEAN_IP, (const int8_t *)NON_SECURITY_PORT, NULL);
	LCD_Clear(WHITE);		   	
	POINT_COLOR = BLACK;			
	LCD_ShowString(40, 10, 200, 16, 24, "TEAM 7 CAN");
	LCD_ShowString(30, 170, 200, 16, 16, "CONNECT SUCCESSFULLY");
#endif

#if defined(WITH_SOTA)
    extern void nb_sota_demo(void);
    nb_sota_demo();
#endif
    printf("\r\n=====================================================");
    printf("\r\nSTEP2: Register Command( NB Notify )");
    printf("\r\n=====================================================\r\n");
	los_nb_notify("+NNMI:",strlen("+NNMI:"),nb_cmd_data_ioctl,OC_cmd_match);
    printf("\r\n=====================================================");
    printf("\r\nSTEP3: Report Data to Server( NB Report )");
    printf("\r\n=====================================================\r\n");
	nb_iot_entry();

#else
    printf("Please checkout if open WITH_AT_FRAMEWORK and USE_NB_NEUL95_NO_ATINY\n");
#endif

}


//收集数据任务
VOID data_collection_task(VOID)
{
	UINT32 uwRet = LOS_OK;	

	
	char Trmp_buf[8]={0};  //临时温度存储容量
	char Capa_buf[8]={0};  //容量的字符数组
	char Latitu_Longitu_buf[17]={0};//临时存储经纬度为零的字符数组
			
	Init_E53_ST1();	
	while (1)
	{
		E53_ST1_Read_Data();                                  //度取GPS的经纬度信息
		HC_SR04_Measure(&hc_sr04_device1);                    //读取容量的数据
		DHT11_Read_Date(&DHT11_Data);                         //读取dht11温度
		float temp;
		if(DHT11_Data.temp_deci<10)
		{
			temp=DHT11_Data.temp_int+(((float)DHT11_Data.temp_deci)/10);
		
		}
		if(DHT11_Data.temp_deci<100&&DHT11_Data.temp_deci>10)
		{
			temp=DHT11_Data.temp_int+(((float)DHT11_Data.temp_deci)/100);
		}
		
		
		printf("\r\n**************************  Longitude Value is  %.5f\r\n", E53_ST1_Data.Longitude);
		printf("\r\n**************************  Latitude Value is  %.5f\r\n", E53_ST1_Data.Latitude);
		printf("\r\n**************************  Capacity Value is  %.2f cm\r\n", hc_sr04_device1.distance);
		printf("\r\n**************************  Temperature Value is  %.1f T\r\n",temp);
		
		
		sprintf(Trmp_buf, "%.1f", temp);//温度
		sprintf(Capa_buf, "%.2f", hc_sr04_device1.distance);//容量转字符
		
		LCD_ShowString(30, 80, 200, 16, 16, "Capacity: ");
	    LCD_ShowString(105, 80, 200, 16, 16, Capa_buf);
	    LCD_ShowString(30, 130, 200, 16, 16, "Temperature:");
	    LCD_ShowString(130, 130, 200, 16, 16, Trmp_buf);
		//构建数据字符串（定位，容量，温度数据）
		//if(E53_ST1_Data.Latitude==0&&E53_ST1_Data.Longitude==0)  //没有定位到数据
	//	{
			
			
			//memset(Latitu_Longitu_buf, 0, sizeof(Latitu_Longitu_buf));//开辟经纬度为零的空间
			sprintf(E53_ST1_send_str,"%.5f%.6f", E53_ST1_Data.Longitude,E53_ST1_Data.Latitude); //在带发送缓冲区，填充经纬度为零的数据
			
			//容量
			memset(E53_ST1_send_str+17, 0, sizeof(Capa_buf));
			memset(Capa_buf, 0, sizeof(Capa_buf));
			sprintf(Capa_buf, "%.6f", hc_sr04_device1.distance);//容量转字符
			memcpy(E53_ST1_send_str + 17, &Capa_buf, sizeof(Capa_buf));//将容量连接到待发送的缓冲区
			
			//温度
			memset(E53_ST1_send_str+25, 0, sizeof(Trmp_buf));
			memset(Trmp_buf, 0, sizeof(Trmp_buf));
			sprintf(Trmp_buf, "%.6f", temp);//容量转字符
			memcpy(E53_ST1_send_str + 25, &Trmp_buf, sizeof(Trmp_buf));//将容量连接到待发送的缓冲区
			
		
	//	}else{//定位到数据
			
	//		sprintf(E53_ST1_send_str, "%.6f%.6f%.6f%.6f", E53_ST1_Data.Longitude,E53_ST1_Data.Latitude,hc_sr04_device1.distance,temp);	
		
	//	}
		
	
		
		
		uwRet=LOS_TaskDelay(2000);
		if(uwRet !=LOS_OK)
		return;
	}
}
//上报前动作：数据收集函数
UINT32 creat_data_collection_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
    UINT32 TskHandle;
    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "data_collection_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_collection_task;
    task_init_param.uwStackSize = 0x1000;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

//数据上报云
VOID data_report_task(VOID)
{
	UINT32 uwRet = LOS_OK;
	UINT32 msglen = 0;
	UINT32 msgid =5;
                              
	while(1)
	{
		
		memset(E53_ST1_send_hex, 0, 66);
		memset(s_report_buf, 0, sizeof(s_report_buf));
		sprintf(s_report_buf,"%02d", msgid);
		str_to_hex((const char*)(&E53_ST1_send_str),sizeof(E53_ST1_send_str),E53_ST1_send_hex);
		//printf ("%s",E53_ST1_send_hex);
		memcpy(s_report_buf + 2, &E53_ST1_send_hex, sizeof(E53_ST1_send_hex));
		msglen = sizeof(E53_ST1_send_hex); 
		if(los_nb_report((const char*)(&s_report_buf), (msglen+2) / 2)>=0)		//发送数据到平台	
		{
			printf("ocean_send_data OK!\n");                                //发生成功
		}
		else                                                                            //发送失败
		{
			printf("ocean_send_data Fail!\n"); 
		}
	
		uwRet=LOS_TaskDelay(5000);
		if(uwRet !=LOS_OK)
		return;
	}
}
UINT32 creat_data_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
    UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "data_report_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_report_task;
    task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

VOID nb_reply_task(VOID)
{
    UINT32 msglen = 0;
    while (1)
    {				                                               
        LOS_SemPend(reply_sem, LOS_WAIT_FOREVER);
        printf("This is reply_report_task!\n");   
        msglen = strlen(s_resp_buf);
        los_nb_report((const char*)(&s_resp_buf), msglen / 2);		//发送数据到平台			
        memset(s_resp_buf, 0, sizeof(s_resp_buf));
    }
}

UINT32 creat_nb_reply_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
    UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "nb_reply_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)nb_reply_task;
    task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }

    return uwRet;
}

void nb_iot_entry(void)
{
    UINT32 uwRet = LOS_OK;

    uwRet = creat_data_collection_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
		
    uwRet = creat_data_report_task();
    if (uwRet != LOS_OK)
    {
    return ;
    }
    uwRet = LOS_SemCreate(0,&reply_sem);
    if (uwRet != LOS_OK)
    {
    return ;
    }				
    uwRet = creat_nb_reply_task();
    if (uwRet != LOS_OK)
    {
    return ;
    }

}
