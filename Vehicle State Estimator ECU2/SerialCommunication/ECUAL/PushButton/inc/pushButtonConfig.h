/*
 * pushButtonConfig.h
 *
 * Created: 10/22/2019 10:21:54 AM
 *  Author: Ammar Shahin
 */ 


#ifndef PUSHBUTTONCONFIG_H_
#define PUSHBUTTONCONFIG_H_
/************************************************************************/
/*                          Files Includes                              */
/************************************************************************/
#include "Gpio.h"

/************************************************************************/
/*  Define the ports and pins where the Push Buttons are connected      */
/************************************************************************/
#define  pushButton0_PORT MYPORTC
#define  pushButton1_PORT MYPORTB


typedef enum
{
	pushButton0 = BIT4,
	pushButton1 = BIT2,
}pushButton;
	
typedef enum
	{
		pushButton0_PIN = 4,
		pushButton1_PIN = 2,
	}pushButton_PIN;

#endif /* PUSHBUTTONCONFIG_H_ */