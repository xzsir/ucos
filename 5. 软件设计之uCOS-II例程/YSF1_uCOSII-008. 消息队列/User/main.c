/******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: uCOSII������
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
*                                  �����ջ
********************************************************************************
*/
static  OS_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  OS_STK  AppTask1Stk[APP_TASK1_STK_SIZE];
static  OS_STK  AppTask2Stk[APP_TASK2_STK_SIZE];
static  OS_STK  AppTask3Stk[APP_TASK3_STK_SIZE];

KEY key1,key2;
/* ������Ϣ���� */
static OS_EVENT *CommQ; 
static void  *CommMsg[10]; 

/* ˽�к���ԭ�� --------------------------------------------------------------*/

static  void AppTaskCreate(void);
static	void AppEventCreate(void);
static  void AppTaskStart(void *p_arg);
static  void AppTask1(void *p_arg);
static  void AppTask2(void *p_arg);
static  void AppTask3(void *p_arg);
static  void DispTaskInfo(void);
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int  main (void)
{
  CPU_INT08U  err;
  
  /* ��ʼ��"uC/OS-II"�ں� */
	OSInit();
  
	/* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
	OSTaskCreateExt(AppTaskStart,	/* ����������ָ�� */
                    (void *)0,		/* ���ݸ�����Ĳ��� */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* ָ������ջջ����ָ�� */
                    APP_TASK_START_PRIO,	/* ��������ȼ�������Ψһ������Խ�����ȼ�Խ�� */
                    APP_TASK_START_PRIO,	/* ����ID��һ����������ȼ���ͬ */
                    (OS_STK *)&AppTaskStartStk[0],/* ָ������ջջ�׵�ָ�롣OS_STK_GROWTH ������ջ�������� */
                    APP_TASK_START_STK_SIZE, /* ����ջ��С */
                    (void *)0,	/* һ���û��ڴ�����ָ�룬����������ƿ�TCB����չ����
                       ���������л�ʱ����CPU����Ĵ��������ݣ���һ�㲻�ã���0���� */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* ����ѡ���� */
                    /*  �������£�
                      OS_TASK_OPT_STK_CHK      ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
                      OS_TASK_OPT_STK_CLR      �ڴ�������ʱ����������ջ
                      OS_TASK_OPT_SAVE_FP      ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
                    */                 
	
	/*ucosII�Ľ��ļ�������0    ���ļ�������0-4294967295*/ 
	OSTimeSet(0);	
	
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_TASK_START_PRIO,(uint8_t *)APP_TASK_START_NAME, &err);
#endif
  /* ����������ϵͳ������Ȩ����uC/OS-II */
  OSStart();
}

/**
  * ��������: �����������塣
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    ������
  */
static  void  AppTaskStart (void *p_arg)
{
 /* �����ڱ���������澯����������������κ�Ŀ����� */	
 (void)p_arg;
  /* BSP ��ʼ���� BSP = Board Support Package �弶֧�ְ����������Ϊ�ײ�������*/
  BSP_Init();                                                 /* Init BSP fncts.                                          */

  CPU_Init();                                                 /* Init CPU name & int. dis. time measuring fncts.          */

  Mem_Init();                                                 /* Init Memory Management Module.                           */
  /* ���CPU������ͳ��ģ���ʼ�����ú�����������CPUռ���� */
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

  APP_TRACE_INFO(("Creating Application Events...\n\r"));
  AppEventCreate();                                           /* Create Application Kernel objects                        */

  APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
  AppTaskCreate();     /* Create application tasks                                 */
  
  /* �������壬������һ����ѭ�� */
  while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
    OSTimeDlyHMSM(0, 0, 10, 0);
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
	INT8U      err;
  
	/* ����AppTask1���� */
	OSTaskCreateExt(AppTask1,
                    (void *)0,
                    (OS_STK *)&AppTask1Stk[APP_TASK1_STK_SIZE - 1],
                    APP_TASK1_PRIO,
                    APP_TASK1_PRIO,
                    (OS_STK *)&AppTask1Stk[0],
                    APP_TASK1_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK1_PRIO, (uint8_t *)APP_TASK1_NAME, &err);
    
	/* ����AppTask2���� */
	OSTaskCreateExt(AppTask2,
                    (void *)0,
                    (OS_STK *)&AppTask2Stk[APP_TASK2_STK_SIZE - 1],
                    APP_TASK2_PRIO,
                    APP_TASK2_PRIO,
                    (OS_STK *)&AppTask2Stk[0],
                    APP_TASK2_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK2_PRIO, (uint8_t *)APP_TASK2_NAME, &err);
    
	/* ����AppTask3���� */
	OSTaskCreateExt(AppTask3,
                    (void *)0,
                    (OS_STK *)&AppTask3Stk[APP_TASK3_STK_SIZE-1],
                    APP_TASK3_PRIO,
                    APP_TASK3_PRIO,
                    (OS_STK *)&AppTask3Stk[0],
                    APP_TASK3_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK3_PRIO, (uint8_t *)APP_TASK3_NAME, &err);
}

/**
  * ��������: uCOSIII�ں˶��󴴽�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void AppEventCreate(void)
{  
  KeyCreate(&key1,GetPinStateOfKey1);
	KeyCreate(&key2,GetPinStateOfKey2);
  
  CommQ  = OSQCreate(&CommMsg[0], 10); /* ����һ����Ϣ���� */
}

/**
  * ��������: ����2������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    �����������ȼ���3
  */
static void AppTask1(void *p_arg)
{
  uint8_t   err,*pmsg;
  
	(void)p_arg;
	 
	while(1)
	{
		pmsg = OSQPend(CommQ, 100, &err); 
		if (err == OS_ERR_NONE) 
		{ 
			printf("maseege[0] = %d\r\n",pmsg[0]);
			printf("maseege[1] = %d\r\n",pmsg[1]);
		} 
		else
		{
			LED1_TOGGLE;	
      LED3_TOGGLE;
		}	  
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
	(void)p_arg;
	 
	while(1)
	{
		LED2_TOGGLE;
    OSTimeDlyHMSM(0, 0, 0, 500);	    
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
	uint8_t i = 0;
	uint8_t CommRxBuf[2];
  uint8_t key1count=1;
  
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
          /* ��ֵҪ���͵����� */
          i++;
          if(i == 10) 
          {
            i = 0;
          }
          CommRxBuf[0] = i;
          CommRxBuf[1] = i + 1;
          OSQPost(CommQ, (void *)&CommRxBuf[0]);
          break; 
      }
      Key_AccessTimes(&key2,KEY_ACCESS_WRITE_CLEAR);
    }
		OSTimeDlyHMSM(0, 0, 0, 20);	
  } 
}

/**
  * ��������: ��uCOS-II������Ϣ��ӡ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void DispTaskInfo(void)
{ 
	OS_TCB      *ptcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	OS_STK_DATA stk;	        /* ���ڶ�ȡ����ջʹ����� */

	ptcb    = &OSTCBTbl[0];		/* ָ���1��������ƿ�(TCB) */
    
	/* ��ӡ���� */
	printf("==================================================\r\n");
	printf("  ���ȼ�   ʹ��ջ  ʣ��ջ  �ٷֱ�   ������\r\n");
	printf("  Prio     Used    Free    Per      Taskname\r\n");

	OSTimeDly(10);
	
	/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
	while (ptcb != NULL)
	{
		/* 
		 * ptcb->OSTCBPrio : ������ƿ��б������������ȼ�
		 * ptcb->OSTCBTaskName : ������ƿ��б�����������ƣ���Ҫ�ڴ��������ʱ��
		 * ����OSTaskNameSet()ָ���������ƣ����磺
		 * OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
		 */
		OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* �������ջ���ÿռ� */
		printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
			stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
			ptcb->OSTCBTaskName);		
		ptcb = ptcb->OSTCBPrev;		            /* ָ����һ��������ƿ� */
	}
}
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
