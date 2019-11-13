/*
 * UART.c
 *
 * Created: 11/12/2019 11:37:02 AM
 *  Author: AVE-LAP-44
 */ 

/************************************************************************/
/*                            Files Includes                            */
/************************************************************************/
#include "Typedefs.h"
#include "UART_Lcfg.h"
#include "UART.h"
#include "BitMasking_Macros.h"
#include "Interrupts.h"
#include "stdlib.h"
#include "string.h"

/************************************************************************/
/*                          UART Registers                              */
/************************************************************************/
#define UART_UDR   *((reg_type)0x2C)
#define UART_UCSRA *((reg_type)0x2B)
#define UART_UCSRB *((reg_type)0x2A)
#define UART_UCSRC *((reg_type)0x40)
#define UART_UBRRL *((reg_type)0x29)
#define UART_UBRRH *((reg_type)0x40)


/************************************************************************/
/*                       Registers Pins Macros                          */
/************************************************************************/
/* UCSRA */
#define RXC     7
#define TXC     6
#define UDRE    5
#define FE      4
#define DOR     3
#define PE      2
#define U2X     1
#define MPCM    0

/* UCSRB */
#define RXCIE   7
#define TXCIE   6
#define UDRIE   5
#define RXEN    4
#define TXEN    3
#define UCSZ2   2
#define RXB8    1
#define TXB8    0

/* UCSRC */
#define URSEL   7
#define UMSEL   6
#define UPM1    5
#define UPM0    4
#define USBS    3
#define UCSZ1   2
#define UCSZ0   1
#define UCPOL   0

#define BAUDRATE_FACTOR_16 16
#define BAUDRATE_FACTOR_8  8
#define BAUDRATE_FACTOR_1  1
/************************************************************************/
/*                   UART Functions Implementations                     */
/************************************************************************/
/**
 * Function : UART_Init
 * Description: this function is to initialize the UART 
 * @param cfg_s the Configuration Structure which include:
 * 								1- the baud rate of the UART
 *								2- the Enable Receiver state
 * 								3- the Enable Transmitter state
 * 								4- the Enable Parity state
 * 								5- the Enable 2StopBits state
 * 								6- the Enable Interrupt_Receive state
 *                              7- the MCU Frequency
 * @return the Status of the initialization [OK Or NOT_OK] 
 */
uint8 UART_Init(UART_CFG_S* cfg_s)
{
	uint8 cfg_State = OK;
	
	/* Setting the Baud rate */
	uint16 BaudRate = (cfg_s->MCU_Freq / (BAUDRATE_FACTOR_16 * cfg_s->BaudRate)) - BAUDRATE_FACTOR_1;
	UART_UBRRL = (uint8) BaudRate;
	//UART_UBRRH |= (uint8) (BaudRate >> BAUDRATE_FACTOR_8);
	SET_BIT(UART_UCSRC,URSEL);
	
	/* Setting the character size to be 8bit*/
	SET_BIT(UART_UCSRC,UCSZ0);
	SET_BIT(UART_UCSRC,UCSZ1);
	CLR_BIT(UART_UCSRB,UCSZ2);
	
	/* Setting the Receiver State */
	switch(cfg_s->Enable_Receiver)
	{
		case ENABLED: 
			SET_BIT(UART_UCSRB,RXEN);
			break;
		case DISABLED:
			CLR_BIT(UART_UCSRB,RXEN);
			break;
		default:
			cfg_State = NOT_OK;
			break;
	}
	
	/* Setting the Transmitter State */
	switch(cfg_s->Enable_Transmitter)
	{
		case ENABLED:
		SET_BIT(UART_UCSRB,TXEN);
		break;
		case DISABLED:
		CLR_BIT(UART_UCSRB,TXEN);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	/* Setting the Parity State */
	switch(cfg_s->Enable_Parity)
	{
		case DISABLED:
		CLR_BIT(UART_UCSRC,UPM0);
		CLR_BIT(UART_UCSRC,UPM1);
		break;
		case ENABLED_EVEN:
		CLR_BIT(UART_UCSRC,UPM0);
		SET_BIT(UART_UCSRC,UPM1);
		break;
		case ENABLED_ODD:
		SET_BIT(UART_UCSRC,UPM0);
		SET_BIT(UART_UCSRC,UPM1);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	/* Setting the StopBits State */
	switch(cfg_s->StopBits)
	{
		case SINGLE_STOPBIT:
		CLR_BIT(UART_UCSRC,USBS);
		break;
		case DOUBLE_STOPBIT:
		SET_BIT(UART_UCSRC,USBS);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	/* Setting the Interrupt_Receive State */
	switch(cfg_s->Interrupt_Receive)
	{
		case ENABLED:
		SET_BIT(UART_UCSRB,RXCIE);
		Interrupts_On();
		break;
		case DISABLED:
		CLR_BIT(UART_UCSRB,RXCIE);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	return cfg_State;
}


/**
 * Function : UART_Send
 * Description: this function is to send a data(8 bits) by the UART 
 * @param data the symbol to be sent
 * @return the Status of the function [OK Or NOT_OK] 
 */
uint8 UART_Send(uint8 data)
{
	/* Wait for empty transmit buffer */
	while( GET_BIT(UART_UCSRA,UDRE) == FALSE);
	UART_UDR = data;
	return OK;
}


/**
 * Function : UART_Receive
 * Description: this function is to Receive a data(8 bits) by the UART 
 * @param data the symbol to be sent
 * @return the Received data
 */
uint8 UART_Receive()
{
	/* Wait for Receive complete Flag */
	while ( GET_BIT(UART_UCSRA,RXC) == FALSE );
	return UART_UDR;
}

/**
 * Function : UART_Write_string
 * Description: this function is to Send a String(array of char) by the UART 
 * @param data the pointer to the string to be sent
 * @return the Status of the function [OK Or NOT_OK] 
 */
uint8 UART_SendString(uint8 *data){
	uint8 i = FALSE;
	while(data[i]){	
		UART_Send(data[i]);
		i++;
	}
	return OK;
}


uint8 UART_SendToTTL(uint32 Send_String)
{
	uint8 Send_Char[10],i;
	itoa(Send_String , Send_Char, 10);
	for( i = 0 ; i < strlen(Send_Char) ; i++)
	{
		UART_Send((Send_Char[i]));
	}
	UART_Send('-');
	return OK;
}