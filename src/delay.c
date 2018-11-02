////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to abstract away interacting with Wii devices over 
//!					I2C.
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include "delay.h"




//==================================================================================================
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------
static	uint32_t						mCoreTimerFreq;							// Core timer frequency value to reference when executing delays (populated during initialization.




//==================================================================================================
// PUBLIC METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Initializes internal variable(s) used to determine delay time in system ticks.
//!	
//!	@paramp[in]		sysClk				Current system clock value in Hz (e.g. 80000000).
////////////////////////////////////////////////////////////////////////////////////////////////////
void Delay_Init(uint32_t sysClk)
{
	mCoreTimerFreq = sysClk/2;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Delays processing for the given number of microseconds.
//!	
//!	@details		Uses the number of core processor ticks to determine the number of ticks 
//!					to execute a while-loop. This loop effectively delays non-interrupt driven 
//!					processing. Bare in mind, this is not a precise implementation but will 
//!					[minimally] provide the delay requested.
//!	
//!	@paramp[in]		duration			Number of microseconds to delay processing.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Delay_Us(uint32_t duration)
{
	uint32_t		tickStop			= ReadCoreTimer() + mCoreTimerFreq/MICRO_SECONDS_PER_TICK * duration;
	while( tickStop > ReadCoreTimer() );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Delays processing for the given number of milliseconds.
//!	
//!	@details		Uses the number of core processor ticks to determine the number of ticks 
//!					to execute a while-loop. This loop effectively delays non-interrupt driven 
//!					processing. Bare in mind, this is not a precise implementation but will 
//!					[minimally] provide the delay requested.
//!	
//!	@paramp[in]		duration			Number of milliseconds to delay processing.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Delay_Ms(uint32_t duration)
{
	uint32_t		tickStop			= ReadCoreTimer() + mCoreTimerFreq/MILLI_SECONDS_PER_TICK * duration;
	while( tickStop > ReadCoreTimer() );
}
