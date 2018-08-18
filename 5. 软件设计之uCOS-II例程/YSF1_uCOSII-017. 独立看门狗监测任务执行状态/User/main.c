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

/* 创建事件标志组 */
static OS_FLAG_GRP *FlagStat; 

/* 私有函数原形 --------------------------------------------------------------*/

static  void AppTaskCreate(void);
static	void AppEventCreate(void);
static  void AppTaskStart(void *p_arg);
static  void AppTask1(void *p_arg);
static  void AppTask2(void *p_arg);
static  void AppTask3(void *p_arg);
static  void DispTaskInfo(void);
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
  uint8_t err;
  
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
  
  /* 任务主体，必须是一个死循环 */
  while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
    /* 等待所有任务发事件 */
		OSFlagPend(FlagStat, 
				   0x07,  
				   OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME,
				   0,
				   &err); 
		
		/* 接受到所有任务正常执行的事件标志，执行喂狗 */
		if(err == OS_ERR_NONE)
		{
			IWDG_Feed();
			printf("4个用户任务都正常运行\r\n");
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
  uint8_t err;
  KeyCreate(&key1,GetPinStateOfKey1);
	KeyCreate(&key2,GetPinStateOfKey2);
  
  /* 创建事件标志组 */
  FlagStat = OSFlagCreate(0x00, &err); 
}

/**
  * 函数功能: 任务2服务函数
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：任务优先级：3
  */
static void AppTask1(void *p_arg)
{
  uint8_t err;
	(void)p_arg;
		  
	while(1)
	{	
    /* 发送事件标准，表示任务正常运行 */
		OSFlagPost(FlagStat, 0x01, OS_FLAG_SET, &err);
    
	 	LED1_TOGGLE;
    LED3_TOGGLE;
    OSTimeDlyHMSM(0, 0, 1, 0);    
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
  uint8_t err;
	(void)p_arg;
	 
	while(1)
	{
		 LED2_TOGGLE;
		 OSTimeDlyHMSM(0, 0, 0, 100);	   
     /* 发送事件标志，表示此任务正常运行 */
		 OSFlagPost(FlagStat, 0x02, OS_FLAG_SET, &err); 
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
  uint8_t err;
  (void)p_arg;

  
	while(1)
	{      
    Key_RefreshState(&key1);//刷新按键状态
    Key_RefreshState(&key2);//刷新按键状态
    if(Key_AccessTimes(&key1,KEY_ACCESS_READ)!=0)//按键被按下过
    {
      DispTaskInfo();	//打印任务执行情况
      Key_AccessTimes(&key1,KEY_ACCESS_WRITE_CLEAR);
    }
    if(Key_AccessTimes(&key2,KEY_ACCESS_READ)!=0)//按键被按下过
    {
      printf("Key2按键按下，让AppTask3任务延迟20s，以实现看门狗复位情况\r\n");
      OSTimeDlyHMSM(0, 0, 20, 0);	
      Key_AccessTimes(&key2,KEY_ACCESS_WRITE_CLEAR);
    }
    /* 发送事件标志，表示此任务正常运行 */
		OSFlagPost(FlagStat, 0x04, OS_FLAG_SET, &err);
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
