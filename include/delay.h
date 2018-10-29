////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants and prototypes related to delaying processing.
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DELAY__
#define	__DELAY__


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include <stdint.h>




//==================================================================================================
// CONSTANTS
//--------------------------------------------------------------------------------------------------
#define MICRO_SECONDS_PER_TICK			1000000									//!< Number of microseconds that will occur within one tick of the system clock.
#define MILLI_SECONDS_PER_TICK			1000									//!< Number of milliseconds that will occur within one tick of the system clock.




//==================================================================================================
// PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
void	Delay_Init(	uint32_t sysClk		);
void	Delay_Us(	uint32_t duration	);
void	Delay_Ms(	uint32_t duration	);


#endif	// __DELAY__
