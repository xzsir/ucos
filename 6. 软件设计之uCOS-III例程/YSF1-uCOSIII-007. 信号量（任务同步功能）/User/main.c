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

static  OS_SEM   SEM_SYNCH;	   /* 用于同步 */

/* 私有函数原形 --------------------------------------------------------------*/
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  AppTaskStart(void *p_arg);
static  void  AppTask1(void *p_arg);
static  void  AppTask2(void *p_arg);
static  void  AppTask3(void *p_arg);
static  void  DispTaskInfo(void);

/* 函数体 --------------------------------------------------------------------*/

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

  /* 删除任务 */   	
	OSTaskDel(&AppTaskStartTCB,&err);
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
  
	/* 创建计数值为0，用于实现任务同步功能 */
	OSSemCreate((OS_SEM    *)&SEM_SYNCH,
				(CPU_CHAR  *)"SEM_SYNCH",
				(OS_SEM_CTR )0,
				(OS_ERR    *)&err);
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
	CPU_TS ts;
	(void)p_arg;
		  
	while(1)
	{	
	 	/* 等待获取信号量同步消息，接收到后执行串口打印 */
		 OSSemPend((OS_SEM *)&SEM_SYNCH,
				   (OS_TICK )0,
				   (OS_OPT  )OS_OPT_PEND_BLOCKING,
				   &ts,
				   (OS_ERR *)&err);
		
		printf("接收到信号量同步消息\r\n");
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
	OS_ERR       err;
	(void)p_arg;
	 
	while(1)
	{
		LED2_TOGGLE;
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);	
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
	OS_ERR      err;
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
        case 2:                                  /* 发送信号量同步消息 */
          OSSemPost((OS_SEM *)&SEM_SYNCH,
						      (OS_OPT  )OS_OPT_POST_1,
						      (OS_ERR *)&err);
          break;
        default:
					break;

      }
      Key_AccessTimes(&key2,KEY_ACCESS_WRITE_CLEAR);
    }  
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
