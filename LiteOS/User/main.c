/***************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   STM32ȫϵ�п�����-LiteOS��
  **************************************************************
  ***************************************************************
  */ 
 /* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_queue.h"
#include "los_sem.h"
#include "los_mux.h"

/* �弶����ͷ�ļ� */
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h"

/* ���������� */
UINT32 Receive_Task_Handle;
UINT32 Send_Task_Handle;

/* Ҫ���͵���Ϣ */
UINT32 send_data1 = 1;
UINT32 send_data2 = 2;

/* ������Ϣ���б��������ȡ���С */
UINT32 Test_Queue_Handle;
#define TEST_QUEUE_LEN 10
#define TEST_QUEUE_SIZE 10

/* ��ֵ�ź���id,�õ��ı��� */
UINT32 BinarySem_Handle;
uint32_t ucValue[]={0x00,0x00};

/* �������ź��� */
 UINT32 Mutex_Handle; //������ID

/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_Receive_Task(void);
static UINT32 Creat_Send_Task(void);

static void Receive_Task(void); //��Ϣ������ں���
static void Send_Task(void);    //��Ϣ������ں���

static void Receive_BinarySem_Task(void);//��ֵ�ź�����ں���
static void Send_BinarySem_Task(void);//��ֵ�ź�����ں���


static void HighPriority_Mux_Task(void);//��������ں���
static void LowPriority_Mux_Task(void); //��������ں���

//����Ӳ����ʼ������
static void BSP_Init(void);

/***************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
			�ڶ���������APPӦ������
			������������LiteOS����ʼ��������ȣ�����ʧ�������������Ϣ
  **************************************************************/
int main(void)
{	
	UINT32 uwRet = LOS_OK;  //����һ�����񴴽��ķ���ֵ��Ĭ��Ϊ�����ɹ�
	
	/* ������س�ʼ�� */
  BSP_Init();
	
	printf("����һ��STM32ȫϵ�п�����-LiteOS-SDRAM��̬����������\n\n");
	
	/* LiteOS �ں˳�ʼ�� */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }

	uwRet = AppTaskCreate();
	if (uwRet != LOS_OK)
  {
		printf("AppTaskCreate��������ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }

  /* ����LiteOS������� */
  LOS_Start();
	
	//��������²���ִ�е�����
	while(1);
	
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� ���񴴽���Ϊ�˷���������е����񴴽����������Է��������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  *****************************************************************/
static UINT32 AppTaskCreate(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;

  /*����һ��������Ϣ����  */
  // uwRet=LOS_QueueCreate("Test_Queue",TEST_QUEUE_LEN,&Test_Queue_Handle,0,TEST_QUEUE_SIZE);

 

  /* ������ֵ�ź��� */
  //uwRet= LOS_BinarySemCreate(1,&BinarySem_Handle);

  /* ���������� */
  uwRet = LOS_MuxCreate(&Mutex_Handle);

  if (uwRet != LOS_OK)
  {
    printf("Test_Queue���д���ʧ�ܣ�ʧ�ܴ���0x%d\n", uwRet);
    return uwRet;
  }

  uwRet = Creat_Receive_Task();
  if (uwRet != LOS_OK)
  {
		printf("Receive_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Send_Task();
  if (uwRet != LOS_OK)
  {
		printf("Send_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ ������  �� Creat_Receive_Task
  * @ ����˵���� ����receive_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Receive_Task()
{
	//����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Receive_Task";/* ������ */
	//task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_Task;/* ��������� */
  //task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_BinarySem_Task; /* ��ֵ�ź������������ */
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)HighPriority_Mux_Task; /* �����������ȼ���������� */
  task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Receive_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Send_Task
  * @ ����˵���� ����Send_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Send_Task()
{
	// ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Send_Task";	/* ������*/
  //task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_Task; /*  ��Ϣ������������� */
  //task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_BinarySem_Task; /*  ��ֵ�ź������������ */
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LowPriority_Mux_Task; /* �����������ȼ���������� */
  task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Send_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/************************��Ϣ���п�ʼ*******************************************************/

/******************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Receive_Task(void)
{
  /* ������һ������ѭ�������ܷ��� */
  UINT32 uwRet = LOS_OK;
  UINT32 *r_queue;
  UINT32 buffersize = 10;

  while (1)
  {
    /* ��ȡ������Ϣ */
    uwRet = LOS_QueueRead(Test_Queue_Handle, &r_queue, buffersize, LOS_WAIT_FOREVER);
    if (LOS_OK == uwRet)
    {
      printf("���ζ�ȡ����Ϣ�ǣ�0x%d\r\n", *(UINT32 *)r_queue);
    }
    else
    {
      printf("��Ϣ��ȡʧ�ܣ�������룺0x%X\r\n", uwRet);
    }
  }
}

/******************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Send_Task(void)
{
  UINT32 uwRet = LOS_OK;
  /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    /* key1���� */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      uwRet = LOS_QueueWrite(Test_Queue_Handle, &send_data1, sizeof(send_data1), 0);

      if (uwRet != LOS_OK)
      {
        printf("��Ϣд��ʧ�ܣ�������룺0x%X\n", uwRet);
      }
    }

    /* key0���� */
    if (Key_Scan(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == KEY_ON)
    {
      uwRet = LOS_QueueWrite(Test_Queue_Handle, &send_data2, sizeof(send_data2), 0);

      if (uwRet != LOS_OK)
      {
        printf("��Ϣд��ʧ�ܣ�������룺0x%X\n", uwRet);
      }
    }

    /* 20msɨ��һ�� */
    LOS_TaskDelay(20);
  }
}
/************************��Ϣ���н���*******************************************************/

/************************��ֵ�ź�����ʼ*****************************************************/
/******************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Receive_BinarySem_Task(void)
{
  /* ������һ������ѭ�������ܷ��� */
  UINT32 uwRet = LOS_OK;
  UINT32 *r_queue;
  UINT32 buffersize=10;

  while (1)
  {
    /* ��ȡ��ֵ�ź��� */
    LOS_SemPend(BinarySem_Handle,LOS_WAIT_FOREVER);
    
    if (ucValue[0]==ucValue[1])
    {
      printf("\r\n��ȡ�ɹ�\r\n");
    }else{
      printf("\r\n��ȡʧ��\r\n");
    }
    
    /* �ͷŶ�ֵ�ź��� */
    LOS_SemPost(BinarySem_Handle);
    
  }
}
/******************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Send_BinarySem_Task(void)
{
  UINT32 uwRet = LOS_OK;
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
    /* key1���� */
    if (Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)==KEY_ON)
    {
      uwRet=LOS_QueueWrite(Test_Queue_Handle,&send_data1,sizeof(send_data1),0);

      if (uwRet!=LOS_OK)
      {
         printf("��Ϣд��ʧ�ܣ�������룺0x%X\n",uwRet);
      }
    }

    /* key0���� */
    if (Key_Scan(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == KEY_ON)
    {
      uwRet = LOS_QueueWrite(Test_Queue_Handle, &send_data2, sizeof(send_data2),0);

      if (uwRet != LOS_OK)
      {
        printf("��Ϣд��ʧ�ܣ�������룺0x%X\n", uwRet);
      }
    }

    /* 20msɨ��һ�� */
    LOS_TaskDelay(20);
    
  }
}
/************************��ֵ�ź�������*********************************************************/



/*****************************��������ʼ********************************************************/
//�����������ȼ�ʵ�ֺ���
static void HighPriority_Mux_Task(void)
{
  UINT32 uwRet = LOS_OK;
  while(1)
  {
    uwRet = LOS_MuxPend(Mutex_Handle,LOS_WAIT_FOREVER);//��ȡ��������һֱ�ȴ���ȡ�ɹ�

    if(uwRet == LOS_OK)
    {
      printf("�����ȼ�������������...");
    }
    LOS_MuxPost(Mutex_Handle);//�ͷŻ�����

    LOS_TaskDelay(1000);//�����������
  }
}

//�����������ȼ�ʵ�ֺ���
static void LowPriority_Mux_Task(void)
{
  UINT32 uwRet = LOS_OK;
  while (1)
  {
    uwRet = LOS_MuxPend(Mutex_Handle, LOS_WAIT_FOREVER); //��ȡ��������һֱ�ȴ���ȡ�ɹ�

    if (uwRet == LOS_OK)
    {
      printf("�����ȼ�������������...");
    }
    LOS_MuxPost(Mutex_Handle); //�ͷŻ�����

    LOS_TaskDelay(1000); //�����������
  }

}
/******************************����������*******************************************************/

  /*******************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	USART_Config();

    /* ������ʼ�� */
  Key_GPIO_Config();

}


