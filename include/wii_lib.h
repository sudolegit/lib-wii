////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants, macros, and constant functions available for the "wii" 
//!					library module.
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WII_LIB__
#define	__WII_LIB__


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include <stdint.h>




//==================================================================================================
// CONSTANTS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of return code values.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_RC
{
	WII_LIB_RC_SUCCESS								= 0							//!< Successfully completed task(s).
} WII_LIB_RC;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines constants used as abstractions to indicate target device type. 
//!					Referenced to determine initialization process, register settings, and how to 
//!					interpret received data.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_TARGET_DEVICE
{
	WII_LIB_TARGET_DEVICE_NUNCHUK					= 1,						//!< Wii target type == Nunchuk
	WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER		= 2,						//!< Wii target type == Classic Controller
	WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER_PRO	= 3							//!< Wii target type == Classic Controller Pro
} WII_LIB_TARGET_DEVICE;


//==================================================================================================
// FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
WII_LIB_RC	WiiLib_Init(I2C_MODULE module, uint32_t pbClk, WII_LIB_TARGET_DEVICE target);




#endif	// __WII_LIB__
