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

/* �����¼���־�� */
static OS_FLAG_GRP *FlagStat; 

/* ˽�к���ԭ�� --------------------------------------------------------------*/

static  void AppTaskCreate(void);
static	void AppEventCreate(void);
static  void AppTaskStart(void *p_arg);
static  void AppTask1(void *p_arg);
static  void AppTask2(void *p_arg);
static  void AppTask3(void *p_arg);
static  void DispTaskInfo(void);
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
  uint8_t err;
  
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
  
  /* �������壬������һ����ѭ�� */
  while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
    /* �ȴ����������¼� */
		OSFlagPend(FlagStat, 
				   0x07,  
				   OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME,
				   0,
				   &err); 
		
		/* ���ܵ�������������ִ�е��¼���־��ִ��ι�� */
		if(err == OS_ERR_NONE)
		{
			IWDG_Feed();
			printf("4���û�������������\r\n");
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
  uint8_t err;
  KeyCreate(&key1,GetPinStateOfKey1);
	KeyCreate(&key2,GetPinStateOfKey2);
  
  /* �����¼���־�� */
  FlagStat = OSFlagCreate(0x00, &err); 
}

/**
  * ��������: ����2������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    �����������ȼ���3
  */
static void AppTask1(void *p_arg)
{
  uint8_t err;
	(void)p_arg;
		  
	while(1)
	{	
    /* �����¼���׼����ʾ������������ */
		OSFlagPost(FlagStat, 0x01, OS_FLAG_SET, &err);
    
	 	LED1_TOGGLE;
    LED3_TOGGLE;
    OSTimeDlyHMSM(0, 0, 1, 0);    
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
  uint8_t err;
	(void)p_arg;
	 
	while(1)
	{
		 LED2_TOGGLE;
		 OSTimeDlyHMSM(0, 0, 0, 100);	   
     /* �����¼���־����ʾ�������������� */
		 OSFlagPost(FlagStat, 0x02, OS_FLAG_SET, &err); 
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
  uint8_t err;
  (void)p_arg;

  
	while(1)
	{      
    Key_RefreshState(&key1);//ˢ�°���״̬
    Key_RefreshState(&key2);//ˢ�°���״̬
    if(Key_AccessTimes(&key1,KEY_ACCESS_READ)!=0)//���������¹�
    {
      DispTaskInfo();	//��ӡ����ִ�����
      Key_AccessTimes(&key1,KEY_ACCESS_WRITE_CLEAR);
    }
    if(Key_AccessTimes(&key2,KEY_ACCESS_READ)!=0)//���������¹�
    {
      printf("Key2�������£���AppTask3�����ӳ�20s����ʵ�ֿ��Ź���λ���\r\n");
      OSTimeDlyHMSM(0, 0, 20, 0);	
      Key_AccessTimes(&key2,KEY_ACCESS_WRITE_CLEAR);
    }
    /* �����¼���־����ʾ�������������� */
		OSFlagPost(FlagStat, 0x04, OS_FLAG_SET, &err);
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
