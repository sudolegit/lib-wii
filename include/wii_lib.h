////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants, macros, and constant functions available for the "wii" 
//!					library module.
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WII_LIB__
#define	__WII_LIB__


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include <stdint.h>
#include "i2c.h"




//==================================================================================================
//	CONSTANTS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of return code values.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_RC
{
	WII_LIB_RC_SUCCESS								= 0,											//!< Successfully completed task(s).
	WII_LIB_RC_UNSUPPORTED_DEVICE					= 1,											//!< Wii target type presently unsupported.
	WII_LIB_RC_TARGET_NOT_INITIALIZED				= 2,											//!< Target not initialized.
	WII_LIB_RC_I2C_ERROR							= 3,											//!< Failed to communicate with device over I2C.
	WII_LIB_RC_TARGET_ID_MISMATCH					= 4,											//!< Value read from target does not match expected value.
	WII_LIB_RC_UNKOWN_PARAMETER						= 5,											//!< Parameter requested is unknown to this library.
	WII_LIB_RC_DATA_RECEIVED_IS_INVALID				= 6,											//!< Data received from target device but value(s) is(are) invalid.
	WII_LIB_RC_UNABLE_TO_DECRYPT_DATA_RECEIVED		= 7												//!< Unable to decrypt data received over I2C.
} WII_LIB_RC;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines constants used as abstractions to indicate target device type. 
//!					Referenced to determine initialization process, register settings, and how to 
//!					interpret received data.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_TARGET_DEVICE
{
	WII_LIB_TARGET_DEVICE_UNKNOWN					= -1,											//!< Placeholder for when a target device ID cannot be read.
	WII_LIB_TARGET_DEVICE_UNSUPPORTED				= 0,											//!< Placeholder for unsupported target device type.
	WII_LIB_TARGET_DEVICE_NUNCHUCK					= 1,											//!< Wii target type == Nunchuk.
	WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER		= 2,											//!< Wii target type == Classic Controller.
	WII_LIB_TARGET_DEVICE_MOTION_PLUS				= 3,											//!< Wii target type == Wii Motion Plus.
	WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_NUNCHUCK	= 4,											//!< Wii target type == Wii Motion Plus that is passing through the Wii Nunchuck.
	WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC	= 5												//!< Wii target type == Wii Motion Plus that is passing through the Wii Classic Controller.
} WII_LIB_TARGET_DEVICE;


// Define byte identifiers that are associated with each target device type.
#define	WII_LIB_ID_LENGTH							6												//!< Length (in bytes) for a ID read from a target device.
#define	WII_LIB_ID_NUNCHUCK							{ 0x00, 0x00, 0xA4, 0x20, 0x00, 0x00 }			//!< Identifier read when device is Wii Nunchuck.
#define	WII_LIB_ID_CLASSIC_CONTROLLER				{ 0x00, 0x00, 0xA4, 0x20, 0x01, 0x01 }			//!< Identifier read when device is Wii Classic Controller.
#define	WII_LIB_ID_WII_MOTION_PLUS					{ 0x00, 0x00, 0xA4, 0x20, 0x04, 0x05 }			//!< Identifier read when device is Wii Motion Plus.
#define	WII_LIB_ID_WII_MOTION_PLUS_PASS_NUNCHUCK	{ 0x00, 0x00, 0xA4, 0x20, 0x05, 0x05 }			//!< Identifier read when device is Wii Motion Plus passing through the Wii Nunchuck.
#define	WII_LIB_ID_WII_MOTION_PLUS_PASS_CLASSIC		{ 0x00, 0x00, 0xA4, 0x20, 0x07, 0x05 }			//!< Identifier read when device is Wii Motion Plus passing through the Wii Classic Controller.


#define	WII_LIB_MAX_PAYLOAD_SIZE					20												//!< Largest size (in bytes) of a I2C payload supported by Wii targets.


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines all known I2C address values for communicating with Wii targets.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_I2C_ADDR
{
	WII_LIB_I2C_ADDR_STANDARD						= 0x52,											//!< Standard I2C address for Wii extension controllers. The same address is used across most devices.
	WII_LIB_I2C_ADDR_WII_MOTION_PLUS				= 0x53											//!< I2C address for Wii Motion Plus.
} WII_LIB_I2C_ADDR;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines all known paramters (registers) available for library to read and/or 
//!					write.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_PARAM
{
	WII_LIB_PARAM_STATUS							= 0x00,											//!< Parameter ID (register address) for querying the status flags from a target device.
	WII_LIB_PARAM_RAW_DATA							= 0x20,											//!< Parameter ID (register address) for querying raw data from a target dvice.
	WII_LIB_PARAM_DEVICE_TYPE						= 0xFA											//!< Parameter ID (register address) for querying the device identifier from a target device.
} WII_LIB_PARAM;

#define	WII_LIB_PARAM_REQUEST_LEN					1												//!< Number of bytes to push when starting parameter query.
#define	WII_LIB_PARAM_RESPONSE_LEN_DEFAULT			6												//!< Number of bytes to read for standard [most] parameter queries.
#define	WII_LIB_PARAM_RESPONSE_LEN_EXTENDED			20												//!< Number of bytes to read for long parameter queries


#define	WII_LIB_MAX_CONNECTION_ATTEMPTS				5												//!< Maximum number of connectoin attempts to try before presuming device not available. May not exceed 255.


// Define delay constants used in library.
#define	WII_LIB_DELAY_I2C_SETTLE_TIME_MS			10												//!< Time to delay in milliseconds after initializing the I2C bus before sending any traffic.
#define	WII_LIB_DELAY_AFTER_CONNECTION_ATTEMPT_MS	500												//!< Time to delay in milliseconds after a failed connection attempt (before next attempt in the initialization function).
#define	WII_LIB_DELAY_AFTER_CONFIG_MESSAGE_MS		20												//!< Time to delay in milliseconds after after sending a configuration message to the target.




//==================================================================================================
//	TYPEDEFS
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
//	PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
WII_LIB_RC		WiiLib_Init(				I2C_MODULE module,		uint32_t pbClk,	WII_LIB_TARGET_DEVICE target,	BOOL decryptData,	WiiLib_Device *device	);
WII_LIB_RC		WiiLib_ConnectToTarget(		WiiLib_Device *device 																								);
WII_LIB_RC		WiiLib_ConfigureDevice(		WiiLib_Device *device																								);
WII_LIB_RC		WiiLib_QueryParameter(		WiiLib_Device *device,	WII_LIB_PARAM param																			);
WII_LIB_RC		WiiLib_SetNewHomePosition(	WiiLib_Device *device																								);
WII_LIB_RC		WiiLib_PollStatus(			WiiLib_Device *device																								);


#endif	// __WII_LIB__
