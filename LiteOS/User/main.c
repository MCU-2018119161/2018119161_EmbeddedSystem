/***************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   STM32全系列开发板-LiteOS！
  **************************************************************
  ***************************************************************
  */ 
 /* LiteOS 头文件 */
#include "los_sys.h"
#include "los_task.ph"
#include "los_queue.h"
#include "los_sem.h"
#include "los_mux.h"

/* 板级外设头文件 */
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h"

/* 定义任务句柄 */
UINT32 Receive_Task_Handle;
UINT32 Send_Task_Handle;

/* 要发送的消息 */
UINT32 send_data1 = 1;
UINT32 send_data2 = 2;

/* 定义消息队列变量、长度、大小 */
UINT32 Test_Queue_Handle;
#define TEST_QUEUE_LEN 10
#define TEST_QUEUE_SIZE 10

/* 二值信号量id,用到的变量 */
UINT32 BinarySem_Handle;
uint32_t ucValue[]={0x00,0x00};

/* 互斥锁信号量 */
 UINT32 Mutex_Handle; //互斥锁ID

/* 函数声明 */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_Receive_Task(void);
static UINT32 Creat_Send_Task(void);

static void Receive_Task(void); //消息队列入口函数
static void Send_Task(void);    //消息队列入口函数

static void Receive_BinarySem_Task(void);//二值信号量入口函数
static void Send_BinarySem_Task(void);//二值信号量入口函数


static void HighPriority_Mux_Task(void);//互斥锁入口函数
static void LowPriority_Mux_Task(void); //互斥锁入口函书

//板载硬件初始化函数
static void BSP_Init(void);

/***************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
			第二步：创建APP应用任务
			第三步：启动LiteOS，开始多任务调度，启动失败则输出错误信息
  **************************************************************/
int main(void)
{	
	UINT32 uwRet = LOS_OK;  //定义一个任务创建的返回值，默认为创建成功
	
	/* 板载相关初始化 */
  BSP_Init();
	
	printf("这是一个STM32全系列开发板-LiteOS-SDRAM动态创建多任务！\n\n");
	
	/* LiteOS 内核初始化 */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS 核心初始化失败！失败代码0x%X\n",uwRet);
		return LOS_NOK;
  }

	uwRet = AppTaskCreate();
	if (uwRet != LOS_OK)
  {
		printf("AppTaskCreate创建任务失败！失败代码0x%X\n",uwRet);
		return LOS_NOK;
  }

  /* 开启LiteOS任务调度 */
  LOS_Start();
	
	//正常情况下不会执行到这里
	while(1);
	
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 任务创建，为了方便管理，所有的任务创建函数都可以放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  *****************************************************************/
static UINT32 AppTaskCreate(void)
{
	/* 定义一个返回类型变量，初始化为LOS_OK */
	UINT32 uwRet = LOS_OK;

  /*创建一个测试消息队列  */
  // uwRet=LOS_QueueCreate("Test_Queue",TEST_QUEUE_LEN,&Test_Queue_Handle,0,TEST_QUEUE_SIZE);

 

  /* 创建二值信号量 */
  //uwRet= LOS_BinarySemCreate(1,&BinarySem_Handle);

  /* 创建互斥锁 */
  uwRet = LOS_MuxCreate(&Mutex_Handle);

  if (uwRet != LOS_OK)
  {
    printf("Test_Queue队列创建失败，失败代码0x%d\n", uwRet);
    return uwRet;
  }

  uwRet = Creat_Receive_Task();
  if (uwRet != LOS_OK)
  {
		printf("Receive_Task任务创建失败！失败代码0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Send_Task();
  if (uwRet != LOS_OK)
  {
		printf("Send_Task任务创建失败！失败代码0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ 函数名  ： Creat_Receive_Task
  * @ 功能说明： 创建receive_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Receive_Task()
{
	//定义一个创建任务的返回类型，初始化为创建成功的返回值
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Receive_Task";/* 任务名 */
	//task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_Task;/* 任务函数入口 */
  //task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_BinarySem_Task; /* 二值信号量任务函数入口 */
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)HighPriority_Mux_Task; /* 互斥锁高优先级任务函数入口 */
  task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&Receive_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}
/*******************************************************************
  * @ 函数名  ： Creat_Send_Task
  * @ 功能说明： 创建Send_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Send_Task()
{
	// 定义一个创建任务的返回类型，初始化为创建成功的返回值
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Send_Task";	/* 任务名*/
  //task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_Task; /*  消息队列任务函数入口 */
  //task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_BinarySem_Task; /*  二值信号量任务函数入口 */
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LowPriority_Mux_Task; /* 互斥锁低优先级任务函数入口 */
  task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&Send_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}

/************************消息队列开始*******************************************************/

/******************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Receive_Task(void)
{
  /* 任务都是一个无限循环，不能返回 */
  UINT32 uwRet = LOS_OK;
  UINT32 *r_queue;
  UINT32 buffersize = 10;

  while (1)
  {
    /* 读取队列消息 */
    uwRet = LOS_QueueRead(Test_Queue_Handle, &r_queue, buffersize, LOS_WAIT_FOREVER);
    if (LOS_OK == uwRet)
    {
      printf("本次读取的消息是：0x%d\r\n", *(UINT32 *)r_queue);
    }
    else
    {
      printf("消息读取失败，错误代码：0x%X\r\n", uwRet);
    }
  }
}

/******************************************************************
  * @ 函数名  ： Send_Task
  * @ 功能说明： Send_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Send_Task(void)
{
  UINT32 uwRet = LOS_OK;
  /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
    /* key1按下 */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      uwRet = LOS_QueueWrite(Test_Queue_Handle, &send_data1, sizeof(send_data1), 0);

      if (uwRet != LOS_OK)
      {
        printf("消息写入失败，错误代码：0x%X\n", uwRet);
      }
    }

    /* key0按下 */
    if (Key_Scan(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == KEY_ON)
    {
      uwRet = LOS_QueueWrite(Test_Queue_Handle, &send_data2, sizeof(send_data2), 0);

      if (uwRet != LOS_OK)
      {
        printf("消息写入失败，错误代码：0x%X\n", uwRet);
      }
    }

    /* 20ms扫描一次 */
    LOS_TaskDelay(20);
  }
}
/************************消息队列结束*******************************************************/

/************************二值信号量开始*****************************************************/
/******************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Receive_BinarySem_Task(void)
{
  /* 任务都是一个无限循环，不能返回 */
  UINT32 uwRet = LOS_OK;
  UINT32 *r_queue;
  UINT32 buffersize=10;

  while (1)
  {
    /* 获取二值信号量 */
    LOS_SemPend(BinarySem_Handle,LOS_WAIT_FOREVER);
    
    if (ucValue[0]==ucValue[1])
    {
      printf("\r\n获取成功\r\n");
    }else{
      printf("\r\n获取失败\r\n");
    }
    
    /* 释放二值信号量 */
    LOS_SemPost(BinarySem_Handle);
    
  }
}
/******************************************************************
  * @ 函数名  ： Send_Task
  * @ 功能说明： Send_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Send_BinarySem_Task(void)
{
  UINT32 uwRet = LOS_OK;
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
    /* key1按下 */
    if (Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)==KEY_ON)
    {
      uwRet=LOS_QueueWrite(Test_Queue_Handle,&send_data1,sizeof(send_data1),0);

      if (uwRet!=LOS_OK)
      {
         printf("消息写入失败，错误代码：0x%X\n",uwRet);
      }
    }

    /* key0按下 */
    if (Key_Scan(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == KEY_ON)
    {
      uwRet = LOS_QueueWrite(Test_Queue_Handle, &send_data2, sizeof(send_data2),0);

      if (uwRet != LOS_OK)
      {
        printf("消息写入失败，错误代码：0x%X\n", uwRet);
      }
    }

    /* 20ms扫描一次 */
    LOS_TaskDelay(20);
    
  }
}
/************************二值信号量结束*********************************************************/



/*****************************互斥锁开始********************************************************/
//互斥锁高优先级实现函数
static void HighPriority_Mux_Task(void)
{
  UINT32 uwRet = LOS_OK;
  while(1)
  {
    uwRet = LOS_MuxPend(Mutex_Handle,LOS_WAIT_FOREVER);//获取互斥锁，一直等待获取成功

    if(uwRet == LOS_OK)
    {
      printf("高优先级函数在运行中...");
    }
    LOS_MuxPost(Mutex_Handle);//释放互斥锁

    LOS_TaskDelay(1000);//任务进入阻塞
  }
}

//互斥锁低优先级实现函数
static void LowPriority_Mux_Task(void)
{
  UINT32 uwRet = LOS_OK;
  while (1)
  {
    uwRet = LOS_MuxPend(Mutex_Handle, LOS_WAIT_FOREVER); //获取互斥锁，一直等待获取成功

    if (uwRet == LOS_OK)
    {
      printf("低优先级函数在运行中...");
    }
    LOS_MuxPost(Mutex_Handle); //释放互斥锁

    LOS_TaskDelay(1000); //任务进入阻塞
  }

}
/******************************互斥锁结束*******************************************************/

  /*******************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 串口初始化	*/
	USART_Config();

    /* 按键初始化 */
  Key_GPIO_Config();

}


