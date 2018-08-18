#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stdio.h>

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define DEBUG_USARTx_BAUDRATE                        115200

#define DEBUG_USARTx                                 USART1
#define DEBUG_USARTx_ClockCmd                        RCC_APB2PeriphClockCmd
#define DEBUG_USARTx_CLK                             RCC_APB2Periph_USART1

#define DEBUG_USARTx_GPIO_ClockCmd                   RCC_APB2PeriphClockCmd    
#define DEBUG_USARTx_TX_PORT                         GPIOA   
#define DEBUG_USARTx_TX_PIN                          GPIO_Pin_9
#define DEBUG_USARTx_TX_CLK                          RCC_APB2Periph_GPIOA 
#define DEBUG_USARTx_RX_PORT                         GPIOA 
#define DEBUG_USARTx_RX_PIN                          GPIO_Pin_10
#define DEBUG_USARTx_RX_CLK                          RCC_APB2Periph_GPIOA

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void DEBUG_USART_Init(void);

#endif  // __BSP_DEBUG_USART_H__

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
