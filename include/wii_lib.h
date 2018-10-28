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
#include "i2c.h"




//==================================================================================================
// CONSTANTS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of return code values.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_RC
{
	WII_LIB_RC_SUCCESS								= 0,											//!< Successfully completed task(s).
	WII_LIB_RC_UNSUPPORTED_DEVICE					= 1,											//!< Wii target type presently unsupported.
	WII_LIB_RC_TARGET_NOT_INITIALIZED				= 2,											//!< Target not initialized.
	WII_LIB_RC_I2C_ERROR							= 3												//!< Failed to communicate with device over I2C.
} WII_LIB_RC;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines constants used as abstractions to indicate target device type. 
//!					Referenced to determine initialization process, register settings, and how to 
//!					interpret received data.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_TARGET_DEVICE
{
	WII_LIB_TARGET_DEVICE_NUNCHUCK					= 1,											//!< Wii target type == Nunchuk.
	WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER		= 2,											//!< Wii target type == Classic Controller.
	WII_LIB_TARGET_DEVICE_WII_MOTION_PLUS			= 3												//!< Wii target type == Wii Motion Plus.
} WII_LIB_TARGET_DEVICE;


#define	WII_LIB_MAX_PAYLOAD_SIZE					20												//!< Largest size (in bytes) of a I2C payload supported by Wii targets.


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines all known I2C address values for communicating with Wii targets.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_I2C_ADDR
{
	WII_LIB_I2C_ADDR_STANDARD						= 0x52,											//!< Standard I2C address for Wii extension controllers. The same address is used across most devices.
	WII_LIB_I2C_ADDR_WII_MOTION_PLUS				= 0x53											//!< I2C address for Wii Motion Plus.
} WII_LIB_I2C_ADDR;




//==================================================================================================
// TYPEDEFS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines the tracking information used when communicating with Wii targets.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiLib_Device
{
	I2C_Device										i2c;											//!< I2C device information. Used when communicating with Wii device over I2C.
	WII_LIB_TARGET_DEVICE							target;											//!< Target device type intended for communication.
	uint8_t											dataEncrypted;									//!< Flag indicating if data read is encrypted.
	uint8_t											dataCurrent[WII_LIB_MAX_PAYLOAD_SIZE];			//!< Payload used when storing the most recently read data in from the target device.
	uint8_t											dataBaseline[WII_LIB_MAX_PAYLOAD_SIZE];			//!< Payload used when storing the baseline data at boot/initialization (zero-points at boot).
} WiiLib_Device;




//==================================================================================================
// PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
WII_LIB_RC		WiiLib_Init(				I2C_MODULE module,	uint32_t pbClk,	WII_LIB_TARGET_DEVICE target,	BOOL decryptData,	WiiLib_Device *device	);
WII_LIB_RC		WiiLib_ConfigureDevice(		WiiLib_Device *device																							);




#endif	// __WII_LIB__
