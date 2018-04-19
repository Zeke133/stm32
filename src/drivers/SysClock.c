#include "inc/SysClock.h"



void SetSysClockTo72(void)
{
    ErrorStatus HSEStartUpStatus;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* ��������� RESET RCC (������ �� �����������, �� ������� �� ����� �������) */
    RCC_DeInit();

    /* �������� HSE (������� �����) */
    RCC_HSEConfig( RCC_HSE_ON);

    /* ���� ���� HSE ����� ����� */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    /* ���� � HSE ��� � ������� */
    if (HSEStartUpStatus == SUCCESS)
    {
    /* ��������� ��� ������� �������� ������������� ������ � FLASH.
    ���� �� �� ����������� ������������ � ����� ��������� ������� ������ � Flash,
    FLASH_PrefetchBufferCmd( ) �� FLASH_SetLatency( ) ����� ���������������� */

        /* �������� Prefetch Buffer */
        //FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

        /* FLASH Latency.
    ������������� �������������:
        FLASH_Latency_0 - 0 < SYSCLK<= 24 MHz
        FLASH_Latency_1 - 24 MHz < SYSCLK <= 48 MHz
        FLASH_Latency_2 - 48 MHz < SYSCLK <= 72 MHz */
        //FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */ /* ������ �� ����� AHB Prescaler. ������� �� ������� (RCC_SYSCLK_Div1) */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */ /* ������ �� ����� APB2 Prescaler. ������� �� ������� (RCC_HCLK_Div1)  */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */ /* ������ �� ����� APB1 Prescaler. ������� ������� �� 2 (RCC_HCLK_Div2)
        ������ ��� �� ������ APB1 ������ ���� �� ����� 36��� (������ �����) */
        RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        /* ��������� PLL �� ���� ����� ������� (RCC_PLLSource_HSE_Div1) � �� ������� �� �������� (RCC_PLLMul_9) */
        /* PLL ����� ����� ������� � ������ ��� ���� (RCC_PLLSource_HSE_Div1) ��� ���������� �� 2 (RCC_PLLSource_HSE_Div2). ������ ����� */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* �������� PLL */
        RCC_PLLCmd( ENABLE);

        /* ���� ���� PLL ����� ����� */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* ����������� ��������� ������������ �� PLL */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* ���� ���� ������������� */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* �������� � HSE. ��� ����� �������� ���� ���, ���� ���� ���-�� ������ ����� ��������������� �� ���� ������� �� ������ � ������� ������� */

        /* ���� ��� �������� - ������ ����*/
        while (1)
        {
        }
    }
}

