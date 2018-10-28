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
static	uint8_t							mBuff[WII_LIB_MAX_PAYLOAD_SIZE];		// Buffer used as common sandbox for transmission requests.




//==================================================================================================
// PUBLIC METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Initializes the Wii target device (e.g. nunchuck).
//!	
//!	@details		Initializes the I2C bus and pushes initialization messages to target device.
//!	
//!	@param[in]		module				Which I2C module (port) to use(e.g. I2C1) when communicating 
//!										to target device.
//!	@param[in]		pbClk				Current peripheral bus clock for device (referenced during 
//!										I2C initialization).
//!	@param[in]		target				Target type. Should be of type 'WII_LIB_TARGET_DEVICE'.
//!	@param[in]		decryptData			Boolean flag indicating if data should be initialized as 
//!										deecrypted.
//!	@param[out]		*device				Instance of 'WiiLib_Device{}' to populate/utilize.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_Init(I2C_MODULE module, uint32_t pbClk, WII_LIB_TARGET_DEVICE target, BOOL decryptData, WiiLib_Device *device)
{
	// Prepare I2C port for communication as a master device.
	device->i2c.port.config			= I2C_ENABLE_SLAVE_CLOCK_STRETCHING | I2C_STOP_IN_IDLE;
	device->i2c.port.module			= module;
	device->i2c.port.clkFreq		= I2C_CLOCK_RATE_STANDARD;
	device->i2c.port.ackMode		= I2C_ACK_MODE_ACK;
	
	// Define common I2C device characteristics (common for communicating with all supported Wii devices).
	device->i2c.mode				= I2C_MODE_MASTER;
	device->i2c.addrLength			= I2C_ADDR_LEN_7_BITS;
	device->target					= target;
	device->dataEncrypted			= (uint8_t)!(decryptData);
	
	// Define device-specific settings.
	switch(device->target)
	{
		case WII_LIB_TARGET_DEVICE_NUNCHUCK:
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER:
			device->i2c.addr		= WII_LIB_I2C_ADDR_STANDARD;
			break;
		
		case WII_LIB_TARGET_DEVICE_WII_MOTION_PLUS:
			device->i2c.addr		= WII_LIB_I2C_ADDR_WII_MOTION_PLUS;
			break;
		
		default:
			return WII_LIB_RC_UNSUPPORTED_DEVICE;
		
	}
	
	// Initialize I2C port and push out settings to initialize device.
	if( I2C_InitPort(&device->i2c.port, pbClk) != I2C_RC_SUCCESS )
		return WII_LIB_RC_I2C_ERROR;
	if( WiiLib_ConfigureDevice( device ) != WII_LIB_RC_SUCCESS )
		return WII_LIB_RC_TARGET_NOT_INITIALIZED;
	
	return WII_LIB_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Pushes out configuration to target device.
//!	
//!	@details		Initializes target device in an encrypted or decrypted state based on the 
//!					configuration flags in the provided device.
//!	
//!	@param[out]		*device				Instance of 'WiiLib_Device{}' defining target device 
//!										interaction.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_ConfigureDevice( WiiLib_Device *device)
{
	// Initialize in most basic form. This leaves data in an encypted state.
	if( device->dataEncrypted )
	{
		mBuff[0] = 0x40;
		mBuff[1] = 0x00;
		if( I2C_Transmit( &device->i2c, &mBuff[0], 2, TRUE ) != I2C_RC_SUCCESS )
			return WII_LIB_RC_I2C_ERROR; 
	}
	// Initialize such that future data transmitted is no longer encrypted.
	else
	{
		mBuff[0] = 0xF0;
		mBuff[1] = 0x55;
		if( I2C_Transmit( &device->i2c, &mBuff[0], 2, TRUE ) == I2C_RC_SUCCESS )
		{
			uint32_t delay = 16666;while(--delay); // Roughly 10 ms delay
			
			mBuff[0] = 0xFB;
			mBuff[1] = 0x00;
			if( I2C_Transmit( &device->i2c, &mBuff[0], 2, TRUE ) != I2C_RC_SUCCESS )
				return WII_LIB_RC_I2C_ERROR;
		}
		else
		{
			return WII_LIB_RC_I2C_ERROR;
		}
	}
	
	return WII_LIB_RC_SUCCESS;
	
}
