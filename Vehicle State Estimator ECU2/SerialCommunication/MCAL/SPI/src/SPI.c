/*
 * SPI.c
 *
 * Created: 11/12/2019 11:37:02 AM
 *  Author: Ammar Shain
 */ 

/************************************************************************/
/*                            Files Includes                            */
/************************************************************************/
#include "Typedefs.h"
#include "SPI_Lcfg.h"
#include "SPI.h"
#include "BitMasking_Macros.h"
#include "Interrupts.h"
#include "Gpio.h"
#include "Timers.h"
#include "communication.h"
#include "math.h"

/************************************************************************/
/*                          SPI Registers                              */
/************************************************************************/
#define SPI_SPCR *((reg_type)0x2D)
#define SPI_SPSR *((reg_type)0x2E)
#define SPI_SPDR *((reg_type)0x2F)


/************************************************************************/
/*                       Registers Pins Macros                          */
/************************************************************************/
/* SPSR */
#define SPIF    7
#define WCOL    6
/* bits 5-1 reserved */
#define SPI2X   0

/* SPCR */
#define SPIE    7
#define SPE     6
#define DORD    5
#define MSTR    4
#define CPOL    3
#define CPHA    2
#define SPR1    1
#define SPR0    0

/************************************************************************/
/*                   Global Variables                                   */
/************************************************************************/
volatile uint32 Speed = 0;
volatile int Count_Flag = 0;
volatile int Speed_Flag = 0;
volatile uint8 Recived    = 0;
/************************************************************************/
/*                   SPI Functions Implementations                     */
/************************************************************************/
/**
 * Function : SPI_Init
 * Description: this function is to initialize the SPI 
 * @param cfg_s the Configuration Structure which include:
 * 								1- the mode of the SPI {MASTER or SLAVE}
 *								2- the clk_Polarity {RISING or FALLING}
 * 								3- the Interrupt_Enable  {POLLING or INTERRUPTS}
 * 								4- the DataOrder  {MSB_ORDER or LSB_ORDER}
 * 								5- the ClockPhase  {SAMPLE or SETUP}
 * 								6- the SPI_Prescaler  {4,16,64,128}
 * @return the Status of the initialization [OK Or NOT_OK] 
 */
uint8 SPI_Init(SPI_CFG_S* cfg_s)
{
	uint8 cfg_State = OK;	
	/* Setting the Mode State */
	switch(cfg_s->Mode)
	{
		case MASTER:
		Gpio_PinDirection(SPI_PORT,SPI_SS,SET_OUT);
		Gpio_PinDirection(SPI_PORT,SPI_MOSI,SET_OUT);
		Gpio_PinDirection(SPI_PORT,SPI_SCK,SET_OUT);
		SET_BIT(SPI_SPCR,MSTR);
		break;
		case SLAVE:
		Gpio_PinDirection(SPI_PORT,SPI_MISO,SET_OUT);
		CLR_BIT(SPI_SPCR,MSTR);
		break;
		default:
			cfg_State = NOT_OK;
			break;
	}
	
	/* Setting the clk_Polarity State */
	switch(cfg_s->clk_Polarity)
	{
		case RISING:
		CLR_BIT(SPI_SPCR,CPOL);
		break;
		case FALLING:
		SET_BIT(SPI_SPCR,CPOL);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	/* Setting the Interrupt_Enable State */
	switch(cfg_s->Interrupt_Enable)
	{
		case INTERRUPT:
		SET_BIT(SPI_SPCR,SPIE);
		Interrupts_On();
		break;
		case POLLING:
		CLR_BIT(SPI_SPCR,SPIE);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	/* Setting the DataOrder State */
	switch(cfg_s->DataOrder)
	{
		case MSB_ORDER:
		CLR_BIT(SPI_SPCR,DORD);
		break;
		case LSB_ORDER:
		SET_BIT(SPI_SPCR,DORD);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	/* Setting the ClockPhase State */
	switch(cfg_s->ClockPhase)
	{
		case SAMPLE:
		CLR_BIT(SPI_SPCR,CPHA);
		break;
		case SETUP:
		SET_BIT(SPI_SPCR,CPHA);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	
	
	/* Setting the Prescaler State */
	switch(cfg_s->SPI_Prescaler)
	{
		case SPI_PRESCALER_4:
		CLR_BIT(SPI_SPCR,SPR0);
		CLR_BIT(SPI_SPCR,SPR1);
		break;
		case SPI_PRESCALER_16:
		SET_BIT(SPI_SPCR,SPR0);
		CLR_BIT(SPI_SPCR,SPR1);
		break;
		case SPI_PRESCALER_64:
		CLR_BIT(SPI_SPCR,SPR0);
		SET_BIT(SPI_SPCR,SPR1);
		break;
		case SPI_PRESCALER_128:
		SET_BIT(SPI_SPCR,SPR0);
		SET_BIT(SPI_SPCR,SPR1);
		break;
		default:
		cfg_State = NOT_OK;
		break;
	}
	SET_BIT(SPI_SPCR,SPE);
	
	Gpio_PortDirection(MYPORTA,SET_OUT);
	//Gpio_PortWrite(MYPORTA,15); 
	return cfg_State;
}


/**
 * Function : SPI_Send
 * Description: this function is to send a data(8 bits) by the SPI 
 * @param data the symbol to be sent
 * @return the Status of the function [OK Or NOT_OK] 
 */
uint8 SPI_Send(uint8 data)
{
	SPI_SPDR = data;
	while(GET_BIT(SPI_SPSR,SPIF) == FALSE);
	return OK;
}


/**
 * Function : SPI_Receive
 * Description: this function is to Receive a data by the SPI 
 * @param data the symbol to be sent
 * @return the Received data
 */
uint8 SPI_Receive()
{
	while(GET_BIT(SPI_SPSR,SPIF) == FALSE);
	return SPI_SPDR;
}

/**
 * Function : SPI_Read
 * Description: this function is to Receive a data by the SPI 
 * @param data the symbol to be sent
 * @return the Received data
 */
uint8 SPI_Read()
{
	return SPI_SPDR;
}


ISR_T(SPI_STC_vect)
{
	Recived = SPI_Read();
	Gpio_PortWrite(MYPORTA,Recived);
	/* This Block of code only once at the first send which is the init time */
	if(Count_Flag == FALSE)
	{
	Time_Init = Recived ;
	Count_Flag = TRUE;
	}
	Speed = Recived;
}