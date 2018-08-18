/**
  ******************************************************************************
  * �ļ�����: bsp_usart.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: ���ص��Դ��ڵײ���������
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
#include "bsp/usart/bsp_debug_usart.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ���ص��Դ��ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ʹ�ú궨�巽������������źţ����������ֲ��ֻҪ���޸�bsp_led.h
  *           �ļ���غ궨��Ϳ��Է����޸����š�
  */
void DEBUG_USART_Init(void)
{
  /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* ����USART��ʼ���ṹ����� */
	USART_InitTypeDef USART_InitStructure;
  /* ʹ��USARTʱ�� */
  DEBUG_USARTx_ClockCmd(DEBUG_USARTx_CLK,ENABLE);
  /* ʹ��USART����GPIOʱ�� */
  DEBUG_USARTx_GPIO_ClockCmd(DEBUG_USARTx_TX_CLK | DEBUG_USARTx_RX_CLK | RCC_APB2Periph_AFIO,ENABLE);
  
	/* ����USART����GPIO��ʼ�� */
	/* �趨USART���Ͷ�ӦIO��� */
	GPIO_InitStructure.GPIO_Pin =  DEBUG_USARTx_TX_PIN;
  /* �趨USART���Ͷ�ӦIOģʽ������������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  /* �趨USART���Ͷ�ӦIO�������ٶ� ��GPIO_Speed_50MHz */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  /* ��ʼ��USART���Ͷ�ӦIO */
	GPIO_Init(DEBUG_USARTx_TX_PORT, &GPIO_InitStructure);    
  
	/* �趨USART���ն�ӦIO��� */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USARTx_RX_PIN;
  /* �趨USART���Ͷ�ӦIOģʽ���������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  /* ����û�����¸�ֵ�ĳ�Աʹ���봮�ڷ�����ͬ���� */
  /* ��ʼ��USART���ն�ӦIO */
	GPIO_Init(DEBUG_USARTx_RX_PORT, &GPIO_InitStructure);	
			
	/* USART������������ */
  /* USART������:115200 */
	USART_InitStructure.USART_BaudRate = DEBUG_USARTx_BAUDRATE;
  /* USART�ֳ�(��Чλ)��8λ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* USARTֹͣλ��1λ */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* USARTУ��λ���� */
	USART_InitStructure.USART_Parity = USART_Parity_No ;
  /* USARTӲ������������(Ӳ���źſ��ƴ���ֹͣ)���� */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	/* USART����ģʽʹ�ܣ�������պͷ��� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ��ʼ��USART */
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
  /* ʹ��USART */
	USART_Cmd(DEBUG_USARTx, ENABLE);
	
}

/**
  * ��������: �ض���c�⺯��printf��USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
  /* ����һ���ֽ����ݵ����Դ��� */
  USART_SendData(DEBUG_USARTx, (uint8_t) ch);

  /* �ȴ��������ݷ������ */
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

  return (ch);
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE *f)
{
  /* �ȴ������������� */
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

  return (int)USART_ReceiveData(DEBUG_USARTx);
}

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
