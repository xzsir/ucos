/******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: uCOSII主函数
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
*                                  任务堆栈
********************************************************************************
*/
static  OS_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  OS_STK  AppTask1Stk[APP_TASK1_STK_SIZE];
static  OS_STK  AppTask2Stk[APP_TASK2_STK_SIZE];
static  OS_STK  AppTask3Stk[APP_TASK3_STK_SIZE];

KEY key1,key2;
/* 创建消息队列 */
static OS_EVENT *CommQ; 
static void  *CommMsg[10]; 

/* 私有函数原形 --------------------------------------------------------------*/

static  void AppTaskCreate(void);
static	void AppEventCreate(void);
static  void AppTaskStart(void *p_arg);
static  void AppTask1(void *p_arg);
static  void AppTask2(void *p_arg);
static  void AppTask3(void *p_arg);
static  void DispTaskInfo(void);
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 主函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int  main (void)
{
  CPU_INT08U  err;
  
  /* 初始化"uC/OS-II"内核 */
	OSInit();
  
	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreateExt(AppTaskStart,	/* 启动任务函数指针 */
                    (void *)0,		/* 传递给任务的参数 */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* 指向任务栈栈顶的指针 */
                    APP_TASK_START_PRIO,	/* 任务的优先级，必须唯一，数字越低优先级越高 */
                    APP_TASK_START_PRIO,	/* 任务ID，一般和任务优先级相同 */
                    (OS_STK *)&AppTaskStartStk[0],/* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                    APP_TASK_START_STK_SIZE, /* 任务栈大小 */
                    (void *)0,	/* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                       （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
                    /*  定义如下：
                      OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
                      OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
                      OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
                    */                 
	
	/*ucosII的节拍计数器清0    节拍计数器是0-4294967295*/ 
	OSTimeSet(0);	
	
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_TASK_START_PRIO,(uint8_t *)APP_TASK_START_NAME, &err);
#endif
  /* 启动多任务系统，控制权交给uC/OS-II */
  OSStart();
}

/**
  * 函数功能: 启动任务函数体。
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：无
  */
static  void  AppTaskStart (void *p_arg)
{
 /* 仅用于避免编译器告警，编译器不会产生任何目标代码 */	
 (void)p_arg;
  /* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
  BSP_Init();                                                 /* Init BSP fncts.                                          */

  CPU_Init();                                                 /* Init CPU name & int. dis. time measuring fncts.          */

  Mem_Init();                                                 /* Init Memory Management Module.                           */
  /* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率 */
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

  APP_TRACE_INFO(("Creating Application Events...\n\r"));
  AppEventCreate();                                           /* Create Application Kernel objects                        */

  APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
  AppTaskCreate();     /* Create application tasks                                 */
  
  /* 任务主体，必须是一个死循环 */
  while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
    OSTimeDlyHMSM(0, 0, 10, 0);
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
	INT8U      err;
  
	/* 创建AppTask1任务 */
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
    
	/* 创建AppTask2任务 */
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
    
	/* 创建AppTask3任务 */
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
  * 函数功能: uCOSIII内核对象创建
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void AppEventCreate(void)
{  
  KeyCreate(&key1,GetPinStateOfKey1);
	KeyCreate(&key2,GetPinStateOfKey2);
  
  CommQ  = OSQCreate(&CommMsg[0], 10); /* 创建一个消息队列 */
}

/**
  * 函数功能: 任务2服务函数
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：3
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
  * 函数功能: 任务2服务函数
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：4
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
  * 函数功能: 用户自定义任务内容
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：5
  */
static void AppTask3(void *p_arg)
{
	uint8_t i = 0;
	uint8_t CommRxBuf[2];
  uint8_t key1count=1;
  
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
          /* 赋值要发送的数据 */
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
  * 函数功能: 将uCOS-II任务信息打印到串口
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DispTaskInfo(void)
{ 
	OS_TCB      *ptcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	OS_STK_DATA stk;	        /* 用于读取任务栈使用情况 */

	ptcb    = &OSTCBTbl[0];		/* 指向第1个任务控制块(TCB) */
    
	/* 打印标题 */
	printf("==================================================\r\n");
	printf("  优先级   使用栈  剩余栈  百分比   任务名\r\n");
	printf("  Prio     Used    Free    Per      Taskname\r\n");

	OSTimeDly(10);
	
	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
	while (ptcb != NULL)
	{
		/* 
		 * ptcb->OSTCBPrio : 任务控制块中保存的任务的优先级
		 * ptcb->OSTCBTaskName : 任务控制块中保存的任务名称，需要在创建任务的时候
		 * 调用OSTaskNameSet()指定任务名称，比如：
		 * OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
		 */
		OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* 获得任务栈已用空间 */
		printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
			stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
			ptcb->OSTCBTaskName);		
		ptcb = ptcb->OSTCBPrev;		            /* 指向上一个任务控制块 */
	}
}
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
