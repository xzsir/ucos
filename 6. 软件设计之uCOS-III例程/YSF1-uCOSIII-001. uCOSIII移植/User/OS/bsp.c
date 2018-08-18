/**
  ******************************************************************************
  * �ļ�����: bsp.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: ����uCOSIII��Ӳ���ײ�ʵ��
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
#include "includes.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
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

/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

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
*	�� �� ��: NVIC_Configuration
*	����˵��: �ж����ȼ����ã������Ȱ����ȼ��������ã�Ȼ����
*			  �����ģ������ʵ�����ȼ����� 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void NVIC_Configuration(void)
{			
	/* ����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/**
  * ��������: ��Ӳ��ģ���ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void BSP_Init(void)
{
  /* �ж����ȼ��������� */
  NVIC_Configuration(); 
  
  /* LED�Ƴ�ʼ��*/
	LED_GPIO_Init();
  
  /* ϵͳ�δ��ʼ�� */
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

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
