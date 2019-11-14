/*
 * communication.c
 *
 * Created: 11/13/2019 2:02:49 PM
 *  Author: AVE-LAP-44
 */ 

/************************************************************************/
/*                          Files Includes                              */
/************************************************************************/
#include "SPI.h"
#include "UART.h"
#include "Timers.h"
#include "pushButton.h"
#include "SwDelay.h"

/************************************************************************/
/*                         Global Variables                             */
/************************************************************************/
volatile uint8 Distance;

/************************************************************************/
/*                      Macros Used in the Code                         */
/************************************************************************/
#define DELAY_TIME 200
#define TIME_FACTOR 0
#define MAX_ONESECOND 3

/************************************************************************/
/*                     Functions Implementations                        */
/************************************************************************/
/**
 * Function : Measurement_Init
 * Description: this function is to initialize the Measurement ECU 
 * @return void
 */
void Measurement_Init()
{
	TimerOneSecond_Flag = FALSE;
	Distance = FALSE;
	Speed = FALSE;
	Time_Init = FALSE;
	SPI_Init(&SPI_Cfg_s);
	UART_Init(&UART_Cfg_s);
	Timers_Init(&timer1_cfg_s);
	pushButton_Init(pushButton0);
	pushButton_Init(pushButton1);
}


/**
 * Function : Measurement_Calc
 * Description: this function is to Start the Measurement and update the result
 * @return void
 */
void Measurement_Calc()
{
	static uint8 Button_Flag = FALSE;
	if( pushButton_Get_Status(pushButton0))
	{
		Button_Flag = TRUE;
		Timers_Start(TIMER1);
		Distance = 0;
	}
	
	if( pushButton_Get_Status(pushButton1))
	{
		Button_Flag = FALSE;
	}

	if( Count_Flag  && Button_Flag && (TimerOneSecond_Flag < MAX_ONESECOND) )
	{
		Distance  += Speed;
		TimerOneSecond_Flag++;
	}

}


/**
 * Function : Measurement_Send
 * Description: This function is to Send the Measurement To PC through TTL UART 
 * @return void
 */
void Measurement_Send()
{
	if(TimerOneSecond_Flag < 3)
	{
		UART_SendToTTL(Distance);
		TimerOneSecond_Flag++;
	}
	else
	{
		//Do nothing
	}
}
