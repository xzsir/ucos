/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : EHS
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_SERIAL_EN                          DEF_ENABLED


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/
#define  APP_TASK_START_NAME                       "Start Task"
#define  APP_TASK1_NAME                            "TASK1"
#define  APP_TASK2_NAME                            "TASK2"
#define  APP_TASK3_NAME                            "TASK3"

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                        2u
#define  APP_TASK1_PRIO                             3u
#define  APP_TASK2_PRIO                             4u
#define  APP_TASK3_PRIO                             5u

#define  OS_TASK_TMR_PRIO         (OS_LOWEST_PRIO - 2u)


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                    128u
#define  APP_TASK1_STK_SIZE                         512u   
#define  APP_TASK2_STK_SIZE                         512u     
#define  APP_TASK3_STK_SIZE                         512u

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#if 0
#define  TRACE_LEVEL_OFF                       		0
#define  TRACE_LEVEL_INFO                      		1
#define  TRACE_LEVEL_DEBUG                     		2
#endif

#define  APP_TRACE_LEVEL                			TRACE_LEVEL_INFO
#define  APP_TRACE                      			printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)



#endif
