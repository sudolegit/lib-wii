////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants, macros, and constant functions available for the "wii" 
//!					library module.
//!	
//!	@note			This is the core header file for the Wii library support (includes all other 
//!					"wii_...h" header files).
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WII_LIB__
#define	__WII_LIB__


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include <stdint.h>
#include "i2c.h"




//==================================================================================================
//	CONSTANTS => GENERAL
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
	WII_LIB_RC_UNABLE_TO_DECRYPT_DATA_RECEIVED		= 7,											//!< Unable to decrypt data received over I2C.
	WII_LIB_RC_DEVICE_DISABLED						= 8,											//!< Device instance is disabled (too many errors).
	WII_LIB_RC_RELATIVE_POSITION_FEATURE_DISABLED	= 9												//!< Relative position feature disabled presently.
} WII_LIB_RC;

#define	WII_LIB_DEFAULT_CALCULATE_RELATIVE_POSITION	TRUE											//!< Default value for flag controlling whether or not relative position is automatically calculated.

#define	WII_LIB_MAX_CONNECTION_ATTEMPTS				5												//!< Maximum number of connection attempts to try before presuming device not available. Used during initializatoin. May not exceed 255.

// Limits used when monitoring error counts and determining any graceful recovery attempts that 
// should be attempted.
#define	WII_LIB_MAX_FAILURES_BEFORE_RECONFIGURING	3												//!< Number of failed I2C communication attempts before attempting to reconfigure the target device.
#define	WII_LIB_MAX_FAILURES_BEFORE_DISABLING		20												//!< Number of failed I2C communication attempts before disabling communication with the target device.




//==================================================================================================
//	CONSTANTS => WII COMMUNICATION PROTOCOL
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines operational status for interacting with target a target device. Used 
//!					to track next state of communication with Wii targets.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _WII_LIB_DEVICE_STATUS
{
	WII_LIB_DEVICE_STATUS_NOT_INITIALIZED			= 0,											//!< Target device needs to be initialized.
	WII_LIB_DEVICE_STATUS_CONFIGURING 				= 1,											//!< Target device needs to be (re)configured.
	WII_LIB_DEVICE_STATUS_ACTIVE 					= 2,											//!< Target device is operating as expected.
	WII_LIB_DEVICE_STATUS_DISABLED 					= 3												//!< Too many failures have occurred. Target device is disabled (no communication permitted without re-initializing.
} WII_LIB_DEVICE_STATUS;


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
//!	@brief			Defines all known parameters (registers) available for library to read and/or 
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




//==================================================================================================
//	CONSTANTS => DELAYS
//--------------------------------------------------------------------------------------------------
#define	WII_LIB_DELAY_I2C_SETTLE_TIME_MS			10												//!< Time to delay in milliseconds after initializing the I2C bus before sending any traffic.
#define	WII_LIB_DELAY_AFTER_CONFIRM_ID_MS			10												//!< Time to delay in milliseconds after confirming the target device ID and before determining the home position.
#define	WII_LIB_DELAY_AFTER_CONNECTION_ATTEMPT_MS	500												//!< Time to delay in milliseconds after a failed connection attempt (before next attempt in the initialization function).
#define	WII_LIB_DELAY_AFTER_CONFIG_MESSAGE_MS		20												//!< Time to delay in milliseconds after after sending a configuration message to the target.


// The following delays are executed prior to raising the stop condition on the bus.
#define	WII_LIB_I2C_DELAY_POST_SEND_MS				0												//!< Delay in milliseconds after trasnmitting a payload across the I2C bus.
#define	WII_LIB_I2C_DELAY_POST_READ_MS				10												//!< Delay in milliseconds after reading a payload from the I2C bus.
#define	WII_LIB_I2C_DELAY_BETWEEN_TX_RX_MS			1												//!< Delay in milliseconds between sending a TX request and starting the following RX reqeust to read the reply.




//==================================================================================================
//	TYPEDEFS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Used to track the state of a Wii controller's buttons, accel, etc.
//!	
//!	@details		Defines every known type of feature across Wii controllers.
//! 
//! @note           Wii nunchuck's use a single Z button and have one joystick, however the classic 
//!                 controller has a left and right version of both. For the purposes of tracking, 
//!                 a non-sided / generic joystick and z button options are not provided.
//!	
//!	@note			Using signed integers to make it easier to do a relative position tracking 
//!					array.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiLib_Interface
{
	// Discrete Buttons:
	uint8_t											buttonA;										//!< Flag indicating status of A button (pressed == high).
	uint8_t											buttonB;										//!< Flag indicating status of B button (pressed == high).
	uint8_t											buttonC;										//!< Flag indicating status of C button (pressed == high).
	uint8_t											buttonX;										//!< Flag indicating status of X button (pressed == high).
	uint8_t											buttonY;										//!< Flag indicating status of Y button (pressed == high).
	uint8_t											buttonZL;										//!< Flag indicating status of the left  z button (pressed == high).
	uint8_t											buttonZR;										//!< Flag indicating status of the right z button (pressed == high).
	uint8_t											buttonMinus;									//!< Flag indicating status of minus [-] button.
	uint8_t											buttonHome;										//!< Flag indicating status of home button.
	uint8_t											buttonPlus;										//!< Flag indicating status of plus [+] button.
	// D-Pad Buttons:
	uint8_t											dpadLeft;										//!< Flag indicating status of the left   d-pad button (pressed == high).
	uint8_t											dpadUp;											//!< Flag indicating status of the top    d-pad button (pressed == high).
	uint8_t											dpadRight;										//!< Flag indicating status of the right  d-pad button (pressed == high).
	uint8_t											dpadDown;										//!< Flag indicating status of the bottom d-pad button (pressed == high).
	// Triggers:
	uint8_t											buttonLeftTrigger;								//!< Flag indicating status of left trigger button.
	uint8_t											buttonRightTrigger;								//!< Flag indicating status of right trigger button.
	int8_t											triggerLeft;									//!< Value of the left [analog] trigger.
	int8_t											triggerRight;									//!< Value of the right [analog] trigger.
	// Analog Joysticks:
	int16_t											analogLeftX;									//!< Value of the left analog joystick along the x-axis.
	int16_t											analogLeftY;									//!< Value of the left analog joystick along the y-axis.
	int16_t											analogRightX;									//!< Value of the right analog joystick along the x-axis.
	int16_t											analogRightY;									//!< Value of the right analog joystick along the y-axis.
	// Accelerometers:
	int16_t											accelX;											//!< Value of the [10-bit] accelerometer along the x-axis.
	int16_t											accelY;											//!< Value of the [10-bit] accelerometer along the y-axis.
	int16_t											accelZ;											//!< Value of the [10-bit] accelerometer along the z-axis.
	// Gyroscopes:
	int16_t											gyroX;											//!< Value of the gyroscope along the x-axis.
	int16_t											gyroY;											//!< Value of the gyroscope along the y-axis.
	int16_t											gyroZ;											//!< Value of the gyroscope along the z-axis.
} WiiLib_Interface;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines the tracking information used when communicating with Wii targets.
//!	
//!	@note			All data presented has been processed and can be easily indexed by treating it 
//!					as a structure of bitfields where the corresponding bitfield mapping is defined 
//!					in the target-specific header file.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiLib_Device
{
	I2C_Device										i2c;											//!< I2C device information. Used when communicating with Wii device over I2C.
	WII_LIB_TARGET_DEVICE							target;											//!< Target device type intended for communication.
	uint8_t											dataEncrypted;									//!< Flag indicating if data read is encrypted.
	uint8_t											calculateRelativePosition;						//!< Flag indicating if the relative position values should be calculated (defaults to 'WII_LIB_DEFAULT_CALCULATE_RELATIVE_POSITION').
	uint8_t											dataCurrent[WII_LIB_MAX_PAYLOAD_SIZE];			//!< Payload used when storing the most recently read data in from the target device.
	WiiLib_Interface								interfaceCurrent;								//!< Instance of most recently read-in status values for interface (buttons, accelerometers, etc.) on the target device.
	WiiLib_Interface								interfaceHome;									//!< Instance of status values associated with the home position for the interface (buttons, accelerometers, etc.) on the target device.
	WiiLib_Interface								interfaceRelative;								//!< Relative interface values obtained by taking 'interfaceCurrent' and subtracting 'interfaceHome' for all interface values.
	uint8_t											failedParamQueryCount;							//!< Tracks number of failed queries over I2C and referenced during maintenance tasks. Updated after each parameter query.
	WII_LIB_DEVICE_STATUS							status;											//!< Status for device. Updated throughout the first initialization process and when (and if) maintenance tasks are run.
} WiiLib_Device;


//==================================================================================================
//	PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
WII_LIB_RC		WiiLib_Init(					I2C_MODULE module,		uint32_t pbClk,	WII_LIB_TARGET_DEVICE target,	BOOL decryptData,	WiiLib_Device *device	);
WII_LIB_RC		WiiLib_ConnectToTarget(			WiiLib_Device *device 																								);
WII_LIB_RC		WiiLib_ConfigureDevice(			WiiLib_Device *device																								);
WII_LIB_RC		WiiLib_DoMaintenance(			WiiLib_Device *device 																								);
WII_LIB_RC		WiiLib_QueryParameter(			WiiLib_Device *device,	WII_LIB_PARAM param																			);
WII_LIB_RC		WiiLib_SetNewHomePosition(		WiiLib_Device *device																								);
WII_LIB_RC		WiiLib_PollStatus(				WiiLib_Device *device																								);
WII_LIB_RC		WiiLib_EnableRelativePosition(	WiiLib_Device *device 																								);
WII_LIB_RC		WiiLib_DisableRelativePosition(	WiiLib_Device *device 																								);




//==================================================================================================
//	WRAPPER INCLUDES
//--------------------------------------------------------------------------------------------------
// Additional includes that may be dependent upon items above but allow this file to be both the 
// core file and single wrapper for the library.
#include "wii_nunchuck.h"
#include "wii_classic_controller.h"


#endif	// __WII_LIB__
