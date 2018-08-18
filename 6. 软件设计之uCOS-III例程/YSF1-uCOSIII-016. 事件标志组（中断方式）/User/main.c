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
static  OS_FLAG_GRP	FLAG_KEY;

/* 私有函数原形 --------------------------------------------------------------*/
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  AppTaskStart(void *p_arg);
static  void  AppTask1(void *p_arg);
static  void  AppTask2(void *p_arg);
static  void  AppTask3(void *p_arg);
static  void  TIM_CallBack1(void);
static  void  TIM_CallBack2         (void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

/* 保存 TIM定时中断到后执行的回调函数指针 */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);
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

 	/* 创建事件标志组 */
	OSFlagCreate ((OS_FLAG_GRP  *)&FLAG_KEY,
                  (CPU_CHAR     *)"FLAG KEY",
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
	OS_ERR       err;
	CPU_TS	     ts;	
	(void)p_arg;
		  
	while(1)
	{	
	  /* 接收到bit0和bit1都被设置为1，那么执行串口打印消息*/
		OSFlagPend ((OS_FLAG_GRP  *)&FLAG_KEY,
                    (OS_FLAGS      )0x03,
                    (OS_TICK       )0,
                    (OS_OPT        )OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME,
                    (CPU_TS       *)&ts,
                    (OS_ERR       *)&err);
		
		printf("接受到按键Key1和按键Key2都被按下的消息\r\n");  
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
		LED1_TOGGLE;
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
  (void)p_arg;
  
	while(1)
	{                                 
    Key_RefreshState(&key1);//刷新按键状态
    Key_RefreshState(&key2);//刷新按键状态
    if(Key_AccessTimes(&key1,KEY_ACCESS_READ)!=0)//按键被按下过
    {
      /* Key1键按下 启动单次定时器中断，50ms后进入中断函数 */
      bsp_StartHardTimer(1 ,50000, (void *)TIM_CallBack1);
      Key_AccessTimes(&key1,KEY_ACCESS_WRITE_CLEAR);
    }
    if(Key_AccessTimes(&key2,KEY_ACCESS_READ)!=0)//按键被按下过
    {
      /* Key2键按下 启动单次定时器中断，50ms后进入中断函数 */
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
*	函 数 名: TIM_CallBack1
*	功能说明: 定时器中断的回调函数，此函数被bsp_StartHardTimer所调用。		  			  
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void TIM_CallBack1(void)
{
	OS_ERR      err;
	
	OSFlagPost ((OS_FLAG_GRP  *)&FLAG_KEY,
				(OS_FLAGS      )0x01,  /* 设置bit0 */
				(OS_OPT        )OS_OPT_POST_FLAG_SET,
				(OS_ERR       *)&err);
}

static void TIM_CallBack2(void)
{
	OS_ERR      err;
	
	OSFlagPost ((OS_FLAG_GRP  *)&FLAG_KEY,
				(OS_FLAGS      )0x02,  /* 设置bit1 */
				(OS_OPT        )OS_OPT_POST_FLAG_SET,
				(OS_ERR       *)&err);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StartHardTimer
*	功能说明: 使用TIM2-5做单次定时器使用, 定时时间到后执行回调函数。可以同时启动4个定时器，互不干扰。
*             定时精度正负10us （主要耗费在调用本函数的执行时间，函数内部进行了补偿减小误差）
*			 TIM2和TIM5 是16位定时器。
*			 TIM3和TIM4 是16位定时器。
*	形    参: _CC : 捕获通道几，1，2，3, 4
*             _uiTimeOut : 超时时间, 单位 1us.       对于16位定时器，最大 65.5ms; 对于32位定时器，最大 4294秒
*             _pCallBack : 定时时间到后，被执行的函数
*	返 回 值: 无
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

    cnt_now = TIM_GetCounter(TIM2);    	/* 读取当前的计数器值 */
    cnt_tar = cnt_now + _uiTimeOut;			/* 计算捕获的计数器值 */
    if (_CC == 1)
    {
      s_TIM_CallBack1 = (void (*)(void))_pCallBack;
      TIM_SetCompare1(TIM2, cnt_tar);      	/* 设置捕获比较计数器CC1 */
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
      TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);	/* 使能CC1中断 */

    }
    else if (_CC == 2)
    {
		s_TIM_CallBack2 = (void (*)(void))_pCallBack;
    TIM_SetCompare2(TIM2, cnt_tar);      	/* 设置捕获比较计数器CC2 */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);	/* 使能CC2中断 */
    }
    else if (_CC == 3)
    {
      s_TIM_CallBack3 = (void (*)(void))_pCallBack;
      TIM_SetCompare3(TIM2, cnt_tar);      	/* 设置捕获比较计数器CC3 */
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
      TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);	/* 使能CC3中断 */
    }
    else if (_CC == 4)
    {
      s_TIM_CallBack4 = (void (*)(void))_pCallBack;
      TIM_SetCompare4(TIM2, cnt_tar);      	/* 设置捕获比较计数器CC4 */
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
      TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);	/* 使能CC4中断 */
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
      TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);	/* 禁能CC1中断 */

      /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
      s_TIM_CallBack1();
  }

  if (TIM_GetITStatus(TIM2, TIM_IT_CC2))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
      TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);	/* 禁能CC2中断 */

      /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
      s_TIM_CallBack2();
  }

  if (TIM_GetITStatus(TIM2, TIM_IT_CC3))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
      TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);	/* 禁能CC3中断 */

      /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
      s_TIM_CallBack3();
  }

  if (TIM_GetITStatus(TIM2, TIM_IT_CC4))
  {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
      TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);	/* 禁能CC4中断 */

      /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
      s_TIM_CallBack4();
  }
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
