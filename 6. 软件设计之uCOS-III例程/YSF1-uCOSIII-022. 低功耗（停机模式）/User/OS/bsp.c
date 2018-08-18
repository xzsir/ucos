/**
  ******************************************************************************
  * 文件名程: bsp.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 基于uCOSIII的硬件底层实现
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
#include "includes.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define  DWT_CR      *(CPU_REG32 *)0xE0001000
#define  DWT_CYCCNT  *(CPU_REG32 *)0xE0001004
#define  DEM_CR      *(CPU_REG32 *)0xE000EDFC
#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004
 
 
/*
 ***********************************************************************
 *                            REGISTER BITS
 ***********************************************************************
 */
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)

/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/*
 ***********************************************************************
 *                           REGISTERS
 ***********************************************************************
 */
 

/*
 ***********************************************************************
 *                             BSP_CPU_ClkFreq()
 *
 * Description : Read CPU registers to determine the CPU clock frequency of the chip.
 *
 * Argument(s) : none.
 *
 * Return(s)   : The CPU clock frequency, in Hz.
 *
 * Caller(s)   : Application.
 *
 * Note(s)     : none.
 ***********************************************************************
 */
 
CPU_INT32U  BSP_CPU_ClkFreq (void)
{
  RCC_ClocksTypeDef  rcc_clocks;
	
	
  RCC_GetClocksFreq(&rcc_clocks);
  return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
 *********************************************************************
 *                           BSP_Tick_Init()
 *
 * Description : Initialize all the peripherals that required OS Tick services (OS initialized)
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : Application.
 *
 * Note(s)     : none.
 *********************************************************************
 */
void BSP_Tick_Init(void)
{
	CPU_INT32U  cpu_clk_freq;
  CPU_INT32U  cnts;	
  cpu_clk_freq = BSP_CPU_ClkFreq(); 
  /* Determine SysTick reference freq. */ 
#if (OS_VERSION >= 30000u)
  cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;       
	/* Determine nbr SysTick increments. */
#else
  cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;       
  /* Determine nbr SysTick increments. */
#endif 
  OS_CPU_SysTickInit(cnts);                              
//  SysTick_Config(cnts);  	
}

/*
*********************************************************************************************************
*	函 数 名: NVIC_Configuration
*	功能说明: 中断优先级配置，这里先把优先级分组分配好，然后在
*			  具体的模块里面实现优先级配置 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void NVIC_Configuration(void)
{			
	/* 设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*
*********************************************************************************************************
*	函 数 名: TouchInterrupt_Configuration
*	功能说明: 停机模式唤醒按键 
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void bsp_InitKeyStopMODE(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

	/* 配置引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

  /* 配置外部中断事件 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

	/* 16个抢占式优先级，0个响应式优先级 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * 函数功能: 各硬件模块初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void BSP_Init(void)
{
  /* 中断优先级分组配置 */
  NVIC_Configuration(); 
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* 使能WKUP 引脚PA0，用于将系统从待机模式唤醒 */
	PWR_WakeUpPinCmd(ENABLE);
  
  /* LED灯初始化*/
	LED_GPIO_Init();
  
  Key1_GPIO_Config();
  Key2_GPIO_Config();
  
  bsp_InitKeyStopMODE();
  
  /* 调试串口初始化 */
  DEBUG_USART_Init();
  
  /* 系统滴答初始化 */
  BSP_Tick_Init();
  
}

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
	CPU_INT32U  cpu_clk_freq_hz;
  
  DEM_CR         |= (CPU_INT32U)DEM_CR_TRCENA;                
	/* Enable Cortex-M3's DWT CYCCNT reg. */
  DWT_CYCCNT      = (CPU_INT32U)0u;
  DWT_CR         |= (CPU_INT32U)DWT_CR_CYCCNTENA;
 
  cpu_clk_freq_hz = BSP_CPU_ClkFreq();
  CPU_TS_TmrFreqSet(cpu_clk_freq_hz);
}
#endif
 
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
  return ((CPU_TS_TMR)DWT_CYCCNT);
}
#endif
 
 
#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
	CPU_INT64U  ts_us;
  CPU_INT64U  fclk_freq;

 
  fclk_freq = BSP_CPU_ClkFreq();
  ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

  return (ts_us);
}
#endif
 
 
#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
	CPU_INT64U  ts_us;
	CPU_INT64U  fclk_freq;


  fclk_freq = BSP_CPU_ClkFreq();
  ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);
	
  return (ts_us);
}
#endif

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
