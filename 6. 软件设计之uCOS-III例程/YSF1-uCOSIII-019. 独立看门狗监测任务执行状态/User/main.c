/******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: uCOSIII主函数
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/

#include <includes.h>
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/*
********************************************************************************
*                                任务控制块
********************************************************************************
*/
static  OS_TCB   AppTaskStartTCB;
static  OS_TCB   AppTask1TCB;
static  OS_TCB   AppTask2TCB;
static  OS_TCB   AppTask3TCB;

/*
********************************************************************************
*                                  任务堆栈
********************************************************************************
*/
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTask1Stk[APP_TASK1_STK_SIZE];
static  CPU_STK  AppTask2Stk[APP_TASK2_STK_SIZE];
static  CPU_STK  AppTask3Stk[APP_TASK3_STK_SIZE];


KEY key1,key2;

static  OS_FLAG_GRP	FLAG_TaskRunStatus;

/* 私有函数原形 --------------------------------------------------------------*/
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  AppTaskStart(void *p_arg);
static  void  AppTask1(void *p_arg);
static  void  AppTask2(void *p_arg);
static  void  AppTask3(void *p_arg);
static  void  DispTaskInfo(void);

/* 函数体 --------------------------------------------------------------------*/
/*
*********************************************************************************************************
*	函 数 名: bsp_InitIwdg
*	功能说明: 独立看门狗时间配置函数
*	形    参：IWDGTime: 0 - 0x0FFF，设置的是128分频，LSI的时钟频率按40KHz计算。
*             128分频的情况下，最小3.2ms，最大13107.2ms。
*	返 回 值: 无		        
*********************************************************************************************************
*/
void bsp_InitIwdg(uint32_t _ulIWDGTime)
{
		
	/* 检测系统是否从独立看门狗复位中恢复 */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{		
		/* 清除复位标志 */
		RCC_ClearFlag();
	}
	
	/* 使能LSI */
	RCC_LSICmd(ENABLE);
	
	/* 等待直到LSI就绪 */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* 写入0x5555表示允许访问IWDG_PR 和IWDG_RLR寄存器 */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/*  LSI/32 分频*/
	IWDG_SetPrescaler(IWDG_Prescaler_128);
	
	IWDG_SetReload(_ulIWDGTime);
	
	/* 重载IWDG计数 */
	IWDG_ReloadCounter();
	
	/* 使能 IWDG (LSI oscillator 由硬件使能) */
	IWDG_Enable();		
}

/*
*********************************************************************************************************
*	函 数 名: IWDG_Feed
*	功能说明: 喂狗函数
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

/**
  * 函数功能: 主函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int  main (void)
{
	OS_ERR  err;

	/* 初始化"uC/OS-III"内核 */ 
	OSInit(&err);                                               /* Init uC/OS-III.                                      */
	/* 创建任务 */
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
	/* 启动多任务系统，控制权交给uC/OS-III */
	OSStart(&err);            /* Start multitasking (i.e. give control to uC/OS-III). */   
  
  (void)&err;
    
  return (0);
}


/**
  * 函数功能: 启动任务函数体。
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：无
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
	  开始执行启动任务主函数前使能独立看门狗。
	  设置LSI是128分频，下面函数参数范围0-0xFFF，分别代表最小值3.2ms和最大值13107.2ms
	  下面设置的是10s，如果10s内没有喂狗，系统复位。
	*/
	bsp_InitIwdg(0xC35);
	
	/* 打印系统开机状态，方便查看系统是否复位 */
	printf("=====================================================\r\n");
	printf("=系统开机执行\r\n");
	printf("=====================================================\r\n");
	
  while (1)
	{  
		/* 等待所有任务发事件标准 */
		OSFlagPend ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
                    (OS_FLAGS      )0x07,
                    (OS_TICK       )0,
                    (OS_OPT        )OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME,
                    (CPU_TS       *)0,
                    (OS_ERR       *)&err);
		
		/* 接受到所有任务正常执行的事件标志，执行喂狗 */
		if(err == OS_ERR_NONE)
		{
			IWDG_Feed();
			printf("三个用户任务都正常运行\r\n");
		}
    }
}

/**
  * 函数功能: 创建应用任务
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：无
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
  * 函数功能: uCOSIII内核对象创建
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static  void  AppObjCreate (void)
{
  OS_ERR      err;

  KeyCreate(&key1,GetPinStateOfKey1);
	KeyCreate(&key2,GetPinStateOfKey2);
  
	/* 创建事件标志组 */
	OSFlagCreate ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
                  (CPU_CHAR     *)"FLAG TaskRunStatus",
                  (OS_FLAGS      )0,
                  (OS_ERR       *)&err);
}

/**
  * 函数功能: 任务2服务函数
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：3
  */
static void AppTask1(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
		  
	while(1)
	{	
	 	/* 发送事件标志，表示任务正常运行 */
		OSFlagPost ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
					(OS_FLAGS      )0x01,  /* 设置bit0 */
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
  * 函数功能: 任务2服务函数
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：4
  */
static void AppTask2(void *p_arg)
{	
	OS_ERR      err;

	(void)p_arg;
	 
	while(1)
	{
		 LED2_TOGGLE;
		 /* 发送事件标志，表示任务正常运行 */
		 OSFlagPost ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
					(OS_FLAGS      )0x02,  /* 设置bit1 */
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
  * 函数功能: 用户自定义任务内容
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：5
  */
static void AppTask3(void *p_arg)
{
	OS_ERR err;
  static uint8_t key1count=1;
  (void)p_arg;
  
  printf("key1count=%d\n",key1count);
  printf("KEY1按键值1~2对应不同任务情况，KEY2确定\n");
  
	while(1)
	{                                 
    Key_RefreshState(&key1);//刷新按键状态
    Key_RefreshState(&key2);//刷新按键状态
    if(Key_AccessTimes(&key1,KEY_ACCESS_READ)!=0)//按键被按下过
    {
      key1count++;
      if(key1count==3) key1count=1;
      printf("key1count=%d\n",key1count);
      printf("KEY1按键值1~2对应不同任务情况，KEY2确定\n");
      Key_AccessTimes(&key1,KEY_ACCESS_WRITE_CLEAR);
    }
    if(Key_AccessTimes(&key2,KEY_ACCESS_READ)!=0)//按键被按下过
    {
      switch(key1count)
      {
        case 1:
          DispTaskInfo();	//打印任务执行情况
          break; 
        case 2:
          printf("K2按键按下，让App Task UserIF任务延迟10s，以实现看门狗复位情况\r\n");
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
    /* 发送事件标志，表示任务正常运行 */
		OSFlagPost ((OS_FLAG_GRP  *)&FLAG_TaskRunStatus,
					(OS_FLAGS      )0x04,  /* 设置bit3 */
					(OS_OPT        )OS_OPT_POST_FLAG_SET,
					(OS_ERR       *)&err);
		OSTimeDlyHMSM(0, 0, 0, 20,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
  } 
}


/**
  * 函数功能: 将uCOS-III任务信息通过串口打印出来
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DispTaskInfo(void)
{
  __IO float CPU;
	OS_TCB      *p_tcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
  p_tcb = OSTaskDbgListPtr;
  CPU_CRITICAL_EXIT();
	
	/* 打印标题 */
	printf("===============================================================\r\n");
	printf(" 优先级 使用栈 剩余栈 百分比 利用率   任务名\r\n");
	printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
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
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
