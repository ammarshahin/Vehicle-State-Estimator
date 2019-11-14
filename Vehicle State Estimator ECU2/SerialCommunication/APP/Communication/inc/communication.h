/*
 * communication.h
 *
 * Created: 11/13/2019 2:01:45 PM
 *  Author: AVE-LAP-44
 */ 


#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

/************************************************************************/
/*                            Files Includes                            */
/************************************************************************/
#include "Typedefs.h"

/************************************************************************/
/*                       Extern Global Variables                        */
/************************************************************************/


/************************************************************************/
/*                     Functions Prototypes                             */
/************************************************************************/
/**
 * Function : Measurement_Init
 * Description: this function is to initialize the Measurement ECU 
 * @return void
 */
void Measurement_Init();
/**
 * Function : Measurement_Calc
 * Description: this function is to Start the Measurement and update the result
 * @return void
 */
void Measurement_Calc();

/**
 * Function : Measurement_Send
 * Description: This function is to Send the Measurement To PC through TTL UART 
 * @return void
 */
void Measurement_Send();
#endif /* COMMUNICATION_H_ */