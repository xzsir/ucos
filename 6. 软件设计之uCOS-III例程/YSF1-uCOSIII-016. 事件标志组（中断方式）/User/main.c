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
static  OS_FLAG_GRP	FLAG_KEY;

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  AppTaskStart(void *p_arg);
static  void  AppTask1(void *p_arg);
static  void  AppTask2(void *p_arg);
static  void  AppTask3(void *p_arg);
static  void  TIM_CallBack1(void);
static  void  TIM_CallBack2         (void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

/* ���� TIM��ʱ�жϵ���ִ�еĻص�����ָ�� */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);
/* ������ --------------------------------------------------------------------*/

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

  /* ɾ������ */   	
	OSTaskDel(&AppTaskStartTCB,&err);
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
	OSFlagCreate ((OS_FLAG_GRP  *)&FLAG_KEY,
                  (CPU_CHAR     *)"FLAG KEY",
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
	OS_ERR       err;
	CPU_TS	     ts;	
	(void)p_arg;
		  
	while(1)
	{	
	  /* ���յ�bit0��bit1��������Ϊ1����ôִ�д��ڴ�ӡ��Ϣ*/
		OSFlagPend ((OS_FLAG_GRP  *)&FLAG_KEY,
                    (OS_FLAGS      )0x03,
                    (OS_TICK       )0,
                    (OS_OPT        )OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME,
                    (CPU_TS       *)&ts,
                    (OS_ERR       *)&err);
		
		printf("���ܵ�����Key1�Ͱ���Key2�������µ���Ϣ\r\n");  
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
	OS_ERR       err;

	(void)p_arg;
	 
	while(1)
	{
		LED1_TOGGLE;
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);
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
	OS_ERR      err;  
  (void)p_arg;
  
	while(1)
	{                                 
    Key_RefreshState(&key1);//ˢ�°���״̬
    Key_RefreshState(&key2);//ˢ�°���״̬
    if(Key_AccessTimes(&key1,KEY_ACCESS_READ)!=0)//���������¹�
    {
      /* Key1������ �������ζ�ʱ���жϣ�50ms������жϺ��� */
      bsp_StartHardTimer(1 ,50000, (void *)TIM_CallBack1);
      Key_AccessTimes(&key1,KEY_ACCESS_WRITE_CLEAR);
    }
    if(Key_AccessTimes(&key2,KEY_ACCESS_READ)!=0)//���������¹�
    {
      /* Key2������ �������ζ�ʱ���жϣ�50ms������жϺ��� */
      bsp_StartHardTimer(2 ,50000, (void *)TIM_CallBack2);
      Key_AccessTimes(&key2,KEY_ACCESS_WRITE_CLEAR);
    }  
		OSTimeDlyHMSM(0, 0, 0, 20,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
  } 
}

/*
*********************************************************************************************************
*	�� �� ��: TIM_CallBack1
*	����˵��: ��ʱ���жϵĻص��������˺�����bsp_StartHardTimer�����á�		  			  
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TIM_CallBack1(void)
{
	OS_ERR      err;
	
	OSFlagPost ((OS_FLAG_GRP  *)&FLAG_KEY,
				(OS_FLAGS      )0x01,  /* ����bit0 */
				(OS_OPT        )OS_OPT_POST_FLAG_SET,
				(OS_ERR       *)&err);
}

static void TIM_CallBack2(void)
{
	OS_ERR      err;
	
	OSFlagPost ((OS_FLAG_GRP  *)&FLAG_KEY,
				(OS_FLAGS      )0x02,  /* ����bit1 */
				(OS_OPT        )OS_OPT_POST_FLAG_SET,
				(OS_ERR       *)&err);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartHardTimer
*	����˵��: ʹ��TIM2-5�����ζ�ʱ��ʹ��, ��ʱʱ�䵽��ִ�лص�����������ͬʱ����4����ʱ�����������š�
*             ��ʱ��������10us ����Ҫ�ķ��ڵ��ñ�������ִ��ʱ�䣬�����ڲ������˲�����С��
*			 TIM2��TIM5 ��16λ��ʱ����
*			 TIM3��TIM4 ��16λ��ʱ����
*	��    ��: _CC : ����ͨ������1��2��3, 4
*             _uiTimeOut : ��ʱʱ��, ��λ 1us.       ����16λ��ʱ������� 65.5ms; ����32λ��ʱ������� 4294��
*             _pCallBack : ��ʱʱ�䵽�󣬱�ִ�еĺ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;

    if (_uiTimeOut < 5)
    {
      ;
    }
    else
    {
      _uiTimeOut -= 5;
    }

    cnt_now = TIM_GetCounter(TIM2);    	/* ��ȡ��ǰ�ļ�����ֵ */
    cnt_tar = cnt_now + _uiTimeOut;			/* ���㲶��ļ�����ֵ */
    if (_CC == 1)
    {
      s_TIM_CallBack1 = (void (*)(void))_pCallBack;
      TIM_SetCompare1(TIM2, cnt_tar);      	/* ���ò���Ƚϼ�����CC1 */
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
      TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);	/* ʹ��CC1�ж� */

    }
    else if (_CC == 2)
    {
		s_TIM_CallBack2 = (void (*)(void))_pCallBack;
    TIM_SetCompare2(TIM2, cnt_tar);      	/* ���ò���Ƚϼ�����CC2 */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);	/* ʹ��CC2�ж� */
    }
    else if (_CC == 3)
    {
      s_TIM_CallBack3 = (void (*)(void))_pCallBack;
      TIM_SetCompare3(TIM2, cnt_tar);      	/* ���ò���Ƚϼ�����CC3 */
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
      TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);	/* ʹ��CC3�ж� */
    }
    else if (_CC == 4)
    {
      s_TIM_CallBack4 = (void (*)(void))_pCallBack;
      TIM_SetCompare4(TIM2, cnt_tar);      	/* ���ò���Ƚϼ�����CC4 */
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
      TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);	/* ʹ��CC4�ж� */
    }
	  else
    {
      return;
    }
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
      TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);	/* ����CC1�ж� */

      /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
      s_TIM_CallBack1();
  }

  if (TIM_GetITStatus(TIM2, TIM_IT_CC2))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
      TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);	/* ����CC2�ж� */

      /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
      s_TIM_CallBack2();
  }

  if (TIM_GetITStatus(TIM2, TIM_IT_CC3))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
      TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);	/* ����CC3�ж� */

      /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
      s_TIM_CallBack3();
  }

  if (TIM_GetITStatus(TIM2, TIM_IT_CC4))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
      TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);	/* ����CC4�ж� */

      /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
      s_TIM_CallBack4();
  }
}

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
