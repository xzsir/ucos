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
static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];

/* 私有函数原形 --------------------------------------------------------------*/

static  void AppTaskCreate(void);
static	void AppEventCreate(void);
static  void AppTaskStart(void *p_arg);

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

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt(AppTaskStart,                               /* Create the start task                                    */
                    (void *)0,
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
                    APP_TASK_START_PRIO,
                    APP_TASK_START_PRIO,
                    (OS_STK *)&AppTaskStartStk[0],
                    APP_TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_TASK_START_PRIO, (uint8_t *)"Start Task", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

/**
  * 函数功能: 启动任务函数体。
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：无
  */
static  void  AppTaskStart (void *p_arg)
{
   (void)p_arg;

    BSP_Init();                                                 /* Init BSP fncts.                                          */

    CPU_Init();                                                 /* Init CPU name & int. dis. time measuring fncts.          */

    Mem_Init();                                                 /* Init Memory Management Module.                           */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

//    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppEventCreate();                                           /* Create Application Kernel objects                        */

//    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create application tasks                                 */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
        LED1_TOGGLE;
		OSTimeDlyHMSM(0, 0, 0, 100);
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
}

/**
  * 函数功能: uCOSIII内核对象创建
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void AppEventCreate(void)
{
}
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
