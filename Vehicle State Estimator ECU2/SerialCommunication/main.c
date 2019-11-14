/*
 * ECU1_Recive.c
 *
 * Created: 11/12/2019 2:09:21 PM
 * Author : Ammar Shahin
 */ 

/************************************************************************/
/*                          Files Includes                              */
/************************************************************************/
#include "communication.h"

/************************************************************************/
/*                     Functions Implementation                        */
/************************************************************************/
int main(void)
{
	Measurement_Init();
	while (TRUE) 
    {
		Measurement_Calc();
		Measurement_Send();
	}
}
