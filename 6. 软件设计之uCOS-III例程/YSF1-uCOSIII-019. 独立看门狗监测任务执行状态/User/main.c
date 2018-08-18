/******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: uCOSIII������
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/

#include <includes.h>
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/*
********************************************************************************
*                                ������ƿ�
********************************************************************************
*/
static  OS_TCB   AppTaskStartTCB;
static  OS_TCB   AppTask1TCB;
static  OS_TCB   AppTask2TCB;
static  OS_TCB   AppTask3TCB;

/*
********************************************************************************
*                                  �����ջ
********************************************************************************
*/
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTask1Stk[APP_TASK1_STK_SIZE];
static  CPU_STK  AppTask2Stk[APP_TASK2_STK_SIZE];
static  CPU_STK  AppTask3Stk[APP_TASK3_STK_SIZE];


KEY key1,key2;

static  OS_FLAG_GRP	FLAG_TaskRunStatus;

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  AppTaskStart(void *p_arg);
static  void  AppTask1(void *p_arg);
static  void  AppTask2(void *p_arg);
static  void  AppTask3(void *p_arg);
static  void  DispTaskInfo(void);

/* ������ --------------------------------------------------------------------*/
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitIwdg
*	����˵��: �������Ź�ʱ�����ú���
*	��    �Σ�IWDGTime: 0 - 0x0FFF�����õ���128��Ƶ��LSI��ʱ��Ƶ�ʰ�40KHz���㡣
*             128��Ƶ������£���С3.2ms�����13107.2ms��
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void bsp_InitIwdg(uint32_t _ulIWDGTime)
{
		
	/* ���ϵͳ�Ƿ�Ӷ������Ź���λ�лָ� */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{		
		/* �����λ��־ */
		RCC_ClearFlag();
	}
	
	/* ʹ��LSI */
	RCC_LSICmd(ENABLE);
	
	/* �ȴ�ֱ��LSI���� */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* д��0x5555��ʾ�������IWDG_PR ��IWDG_RLR�Ĵ��� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/*  LSI/32 ��Ƶ*/
	IWDG_SetPrescaler(IWDG_Prescaler_128);
	
	IWDG_SetReload(_ulIWDGTime);
	
	/* ����IWDG���� */
	IWDG_ReloadCounter();
	
	/* ʹ�� IWDG (LSI oscillator ��Ӳ��ʹ��) */
	IWDG_Enable();		
}

/*
*********************************************************************************************************
*	�� �� ��: IWDG_Feed
*	����˵��: ι������
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

/**
  * ��������: ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int  main (void)
{
	OS_ERR  err;

	/* ��ʼ��"uC/OS-III"�ں� */ 
	OSInit(&err);                                               /* Init uC/OS-III.                                      */
	/* �������� */
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
				 (CPU_CHAR   *)"App Task Start",
				 (OS_TASK_PTR ) AppTaskStart,
				 (void       *) 0,
				 (OS_PRIO     ) APP_TASK_START_PRIO,
				 (CPU_STK    *)&AppTaskStartStk[0],
				 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
				 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
				 (OS_MSG_QTY  ) 0,
				 (OS_TICK     ) 0,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&err);
	/* ����������ϵͳ������Ȩ����uC/OS-III */
	OSStart(&err);            /* Start multitasking (i.e. give control to uC/OS-III). */   
  
  (void)&err;
    
  return (0);
}


/**
  * ��������: �����������塣
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    ������
  */
static  void  AppTaskStart (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  CPU_Init();

  Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
  AppTaskCreate();                                            /* Create Application Tasks                             */

  APP_TRACE_INFO(("Creating Application Events...\n\r"));
  AppObjCreate();                                             /* Create Application Objects                           */

	/* 
	  ��ʼִ����������������ǰʹ�ܶ������Ź���
	  ����LSI��128��Ƶ�����溯��������Χ0-0xFFF���ֱ������Сֵ3.2ms�����ֵ13107.2ms
	  �������õ���10s�����10s��û��ι����ϵͳ��λ��
	*/
	bsp_InitIwdg(0xC35);
	
	/* ��ӡϵͳ����״̬������鿴ϵͳ�Ƿ�λ */
	printf("=====================================================\r\n");
	printf("=ϵͳ����ִ��\r\n");
	printf("=====================================================\r\n");
	
  while (1)
	{  
		/* �ȴ����������¼���׼ */
		OSFlagPend ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
                    (OS_FLAGS      )0x07,
                    (OS_TICK       )0,
                    (OS_OPT        )OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME,
                    (CPU_TS       *)0,
                    (OS_ERR       *)&err);
		
		/* ���ܵ�������������ִ�е��¼���־��ִ��ι�� */
		if(err == OS_ERR_NONE)
		{
			IWDG_Feed();
			printf("�����û�������������\r\n");
		}
    }
}

/**
  * ��������: ����Ӧ������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    ������
  */
static  void  AppTaskCreate (void)
{
  OS_ERR      err;
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTask1TCB,             
                 (CPU_CHAR     *)"App Task1",
                 (OS_TASK_PTR   )AppTask1, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK1_PRIO,
                 (CPU_STK      *)&AppTask1Stk[0],
                 (CPU_STK_SIZE  )APP_TASK1_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK1_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTask2TCB,            
                 (CPU_CHAR     *)"App Task2",
                 (OS_TASK_PTR   )AppTask2, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK2_PRIO,
                 (CPU_STK      *)&AppTask2Stk[0],
                 (CPU_STK_SIZE  )APP_TASK2_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK2_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTask3TCB,             
                 (CPU_CHAR     *)"App Task3",
                 (OS_TASK_PTR   )AppTask3, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK3_PRIO,
                 (CPU_STK      *)&AppTask3Stk[0],
                 (CPU_STK_SIZE  )APP_TASK3_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK3_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);				 
}


/**
  * ��������: uCOSIII�ں˶��󴴽�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static  void  AppObjCreate (void)
{
  OS_ERR      err;

  KeyCreate(&key1,GetPinStateOfKey1);
	KeyCreate(&key2,GetPinStateOfKey2);
  
	/* �����¼���־�� */
	OSFlagCreate ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
                  (CPU_CHAR     *)"FLAG TaskRunStatus",
                  (OS_FLAGS      )0,
                  (OS_ERR       *)&err);
}

/**
  * ��������: ����2������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    �����������ȼ���3
  */
static void AppTask1(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
		  
	while(1)
	{	
	 	/* �����¼���־����ʾ������������ */
		OSFlagPost ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
					(OS_FLAGS      )0x01,  /* ����bit0 */
					(OS_OPT        )OS_OPT_POST_FLAG_SET,
					(OS_ERR       *)&err);	
		
	     OSTimeDlyHMSM((CPU_INT16U) 0u,
					   (CPU_INT16U) 0u,
					   (CPU_INT16U) 0u,
					   (CPU_INT32U) 500u,
					   (OS_OPT    ) OS_OPT_TIME_DLY,
					   (OS_ERR   *)&err);		  
	}   
}

/**
  * ��������: ����2������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    �����������ȼ���4
  */
static void AppTask2(void *p_arg)
{	
	OS_ERR      err;

	(void)p_arg;
	 
	while(1)
	{
		 LED2_TOGGLE;
		 /* �����¼���־����ʾ������������ */
		 OSFlagPost ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
					(OS_FLAGS      )0x02,  /* ����bit1 */
					(OS_OPT        )OS_OPT_POST_FLAG_SET,
					(OS_ERR       *)&err);	
		
	     OSTimeDlyHMSM((CPU_INT16U) 0u,
					   (CPU_INT16U) 0u,
					   (CPU_INT16U) 0u,
					   (CPU_INT32U) 500u,
					   (OS_OPT    ) OS_OPT_TIME_DLY,
					   (OS_ERR   *)&err);	                               
	} 											 
}

/**
  * ��������: �û��Զ�����������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    �����������ȼ���5
  */
static void AppTask3(void *p_arg)
{
	OS_ERR err;
  static uint8_t key1count=1;
  (void)p_arg;
  
  printf("key1count=%d\n",key1count);
  printf("KEY1����ֵ1~2��Ӧ��ͬ���������KEY2ȷ��\n");
  
	while(1)
	{                                 
    Key_RefreshState(&key1);//ˢ�°���״̬
    Key_RefreshState(&key2);//ˢ�°���״̬
    if(Key_AccessTimes(&key1,KEY_ACCESS_READ)!=0)//���������¹�
    {
      key1count++;
      if(key1count==3) key1count=1;
      printf("key1count=%d\n",key1count);
      printf("KEY1����ֵ1~2��Ӧ��ͬ���������KEY2ȷ��\n");
      Key_AccessTimes(&key1,KEY_ACCESS_WRITE_CLEAR);
    }
    if(Key_AccessTimes(&key2,KEY_ACCESS_READ)!=0)//���������¹�
    {
      switch(key1count)
      {
        case 1:
          DispTaskInfo();	//��ӡ����ִ�����
          break; 
        case 2:
          printf("K2�������£���App Task UserIF�����ӳ�10s����ʵ�ֿ��Ź���λ���\r\n");
					OSTimeDlyHMSM((CPU_INT16U) 0u,
								  (CPU_INT16U) 0u,
								  (CPU_INT16U) 10u,
								  (CPU_INT32U) 0u,
								  (OS_OPT    ) OS_OPT_TIME_DLY,
								  (OS_ERR   *)&err);
          break;      
      }
      Key_AccessTimes(&key2,KEY_ACCESS_WRITE_CLEAR);
    }  
    /* �����¼���־����ʾ������������ */
		OSFlagPost ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
					(OS_FLAGS      )0x04,  /* ����bit3 */
					(OS_OPT        )OS_OPT_POST_FLAG_SET,
					(OS_ERR       *)&err);
		OSTimeDlyHMSM(0, 0, 0, 20,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
  } 
}


/**
  * ��������: ��uCOS-III������Ϣͨ�����ڴ�ӡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void DispTaskInfo(void)
{
  __IO float CPU;
	OS_TCB      *p_tcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
  p_tcb = OSTaskDbgListPtr;
  CPU_CRITICAL_EXIT();
	
	/* ��ӡ���� */
	printf("===============================================================\r\n");
	printf(" ���ȼ� ʹ��ջ ʣ��ջ �ٷֱ� ������   ������\r\n");
	printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
	while (p_tcb != (OS_TCB *)0) 
	{
		CPU =(float)p_tcb->CPUUsage;
		printf("   %2d  %5d  %5d   %02d%%   %5.2f%%   %s\r\n", 
		p_tcb->Prio, 
		p_tcb->StkUsed, 
		p_tcb->StkFree, 
		(p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree),
		CPU/100,
		p_tcb->NamePtr);		
	 	
		CPU_CRITICAL_ENTER();
      p_tcb = p_tcb->DbgNextPtr;
      CPU_CRITICAL_EXIT();
	}
}
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
