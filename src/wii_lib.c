////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to abstract away interacting with Wii devices over 
//!					I2C.
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include "i2c.h"
#include "wii_lib.h"




//==================================================================================================
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------
static	I2C_Device	i2c_device;													// Instance of I2C port communication will occur over when talking with target. Defined within initialization function.




//==================================================================================================
// PUBLIC METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Initializes the Wii target device (e.g. nunchuk).
//!	
//!	@details		Initializes the I2C bus and pushes initialization messages to target device.
//!	
//!	@param[in]		module				Which I2C module (port) to use(e.g. I2C1) when communicating 
//!										to target device.
//!	@param[in]		pbClk				Current peripheral bus clock for device (referenced during 
//!										I2C initialization).
//!	@param[in]		target				Target type. Should be of type 'WII_LIB_TARGET_DEVICE'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_Init(I2C_MODULE module, uint32_t pbClk, WII_LIB_TARGET_DEVICE target)
{
	// Prepare I2C port for communication as a master device.
	i2c_device.config		= I2C_ENABLE_SLAVE_CLOCK_STRETCHING | I2C_STOP_IN_IDLE;
	i2c_device.module		= module;
	i2c_device.clkFreq		= I2C_CLOCK_RATE_STANDARD;
	i2c_device.mode			= I2C_MODE_MASTER;
	i2c_device.addr			= WII_LIB_I2C_TARGET_ADDR;
	
	I2C_InitPort(&i2c_device, pbClk);
	
	// Push out initialization settings to target.
	switch(target)
	{
		case WII_LIB_TARGET_DEVICE_NUNCHUK:
			// TO DO:  Define and push initialization settings.
			break;
		
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER:
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER_PRO:
		default:
			break;
		
	}
	
	return WII_LIB_RC_SUCCESS;
	
}
