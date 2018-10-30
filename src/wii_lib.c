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

/*/ 
TO DO:
	-	Read bytes func (device, addr, len)
		-	Wrapper for reading status (0x00)
		-	Wrapper for reading ID (0xFA)
		-	Wrapper for finding home/zero position (save to different bytes)
		**	memset to zero before reading!
	-	Change TxRx I2C function to opt not to send stop command if desired.
// */




//==================================================================================================
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------
static	uint8_t							mBuff[WII_LIB_MAX_PAYLOAD_SIZE];		// Buffer used as common sandbox for transmission requests.




//==================================================================================================
// PRIVATE FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
static WII_LIB_RC				WiiLib_Decrypt(				uint8_t *data,			int8_t len	);
static WII_LIB_TARGET_DEVICE	WiiLib_DetermineDeviceType(	WiiLib_Device *device				);




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
//!	@param[in]		*device				Instance of 'WiiLib_Device{}' to populate/utilize.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_Init( I2C_MODULE module, uint32_t pbClk, WII_LIB_TARGET_DEVICE target, BOOL decryptData, WiiLib_Device *device )
{
	WII_LIB_TARGET_DEVICE		targetValueRead;
	
	// Presume delay not yet initialized and initialize delay module. Even if this is not the case, 
	// should have no harm (in theory/so long as pbClk not different between devices).
	Delay_Init(pbClk);
	
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
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_NUNCHUCK:
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC:
			device->i2c.addr		= WII_LIB_I2C_ADDR_STANDARD;
			break;
		
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS:
			device->i2c.addr		= WII_LIB_I2C_ADDR_WII_MOTION_PLUS;
			break;
		
		default:
			return WII_LIB_RC_UNSUPPORTED_DEVICE;
		
	}
	
	// Initialize I2C port and push out settings to initialize device.
	if( I2C_InitPort(&device->i2c.port, pbClk) != I2C_RC_SUCCESS )
		return WII_LIB_RC_I2C_ERROR;
	
	Delay_Ms(10);
	
	if( WiiLib_ConfigureDevice( device ) != WII_LIB_RC_SUCCESS )
		return WII_LIB_RC_TARGET_NOT_INITIALIZED;
	
	// Confirm target device ID. Override value and return error if mismatch detected.
	targetValueRead = WiiLib_DetermineDeviceType(device);
	if( targetValueRead != target )
	{
		device->target = targetValueRead;
		return WII_LIB_RC_TARGET_ID_MISMATCH;
	}
	
	return WII_LIB_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Pushes out configuration to target device.
//!	
//!	@details		Initializes target device in an encrypted or decrypted state based on the 
//!					configuration flags in the provided device.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}' defining target device 
//!										interaction.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_ConfigureDevice( WiiLib_Device *device )
{
	// Initialize in most basic form. This leaves data in an encypted state.
	if( device->dataEncrypted )
	{
		mBuff[0] = 0x40;
		mBuff[1] = 0x00;
		if( I2C_Transmit( &device->i2c, &mBuff[0], 2, TRUE ) != I2C_RC_SUCCESS )
			return WII_LIB_RC_I2C_ERROR;
		Delay_Ms(20);
	}
	// Initialize such that future data transmitted is no longer encrypted.
	else
	{
		mBuff[0] = 0xF0;
		mBuff[1] = 0x55;
		if( I2C_Transmit( &device->i2c, &mBuff[0], 2, TRUE ) == I2C_RC_SUCCESS )
		{
			Delay_Ms(10);
			
			mBuff[0] = 0xFB;
			mBuff[1] = 0x00;
			if( I2C_Transmit( &device->i2c, &mBuff[0], 2, TRUE ) != I2C_RC_SUCCESS )
				return WII_LIB_RC_I2C_ERROR;
			Delay_Ms(20);
		}
		else
		{
			return WII_LIB_RC_I2C_ERROR;
		}
	}
	
	return WII_LIB_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Handles the process of decrypting data received from a target device.
//!	
//!	@details		Queries the device for it's identifier by writing '0xFA' to the target and 
//!					reading back the 6-byte value. The value is decrypted if necessary before then 
//!					comparing it against the expected ID values.
//!	
//!	@note			Presently, the ID comparison method feels a bit hacky, but working for now 
//!					(function may be revised later).
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Entry from 'WII_LIB_TARGET_DEVICE{}' that represents the target device 
//!					determined.
////////////////////////////////////////////////////////////////////////////////////////////////////
static WII_LIB_TARGET_DEVICE WiiLib_DetermineDeviceType( WiiLib_Device *device )
{
	mBuff[0] = 0xFA;
	
	I2C_TxRx( &device->i2c, &mBuff[0], 1, &device->dataCurrent[0], 6, TRUE, FALSE );
	Delay_Ms(50);
	I2C_TxRx( &device->i2c, &mBuff[0], 1, &device->dataCurrent[0], 6, TRUE, FALSE );
	Delay_Ms(50);
	
	if( I2C_Transmit( &device->i2c, &mBuff[0], 1, TRUE ) == I2C_RC_SUCCESS )
	{
		Delay_Ms(1);
		if( I2C_Receive( &device->i2c, &device->dataCurrent[0], WII_LIB_ID_LENGTH, TRUE ) == I2C_RC_SUCCESS )
		{
			if(device->dataEncrypted)
				WiiLib_Decrypt( &device->dataCurrent[0], WII_LIB_ID_LENGTH );
			
			{
				uint8_t	tmp[WII_LIB_ID_LENGTH] = WII_LIB_ID_NUNCHUCK;
				if( !memcmp( &tmp[0], &device->dataCurrent[0], WII_LIB_ID_LENGTH ) )
					return WII_LIB_TARGET_DEVICE_NUNCHUCK;
			}
			{
				uint8_t	tmp[WII_LIB_ID_LENGTH] = WII_LIB_ID_CLASSIC_CONTROLLER;
				if( !memcmp( &tmp[0], &device->dataCurrent[0], WII_LIB_ID_LENGTH ) )
					return WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER;
			}
			{
				uint8_t	tmp[WII_LIB_ID_LENGTH] = WII_LIB_ID_WII_MOTION_PLUS;
				if( !memcmp( &tmp[0], &device->dataCurrent[0], WII_LIB_ID_LENGTH ) )
					return WII_LIB_TARGET_DEVICE_MOTION_PLUS;
			}
			{
				uint8_t	tmp[WII_LIB_ID_LENGTH] = WII_LIB_ID_WII_MOTION_PLUS_PASS_NUNCHUCK;
				if( !memcmp( &tmp[0], &device->dataCurrent[0], WII_LIB_ID_LENGTH ) )
					return WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_NUNCHUCK;
			}
			{
				uint8_t	tmp[WII_LIB_ID_LENGTH] = WII_LIB_ID_WII_MOTION_PLUS_PASS_CLASSIC;
				if( !memcmp( &tmp[0], &device->dataCurrent[0], WII_LIB_ID_LENGTH ) )
					return WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC;
			}
			
			return WII_LIB_TARGET_DEVICE_UNSUPPORTED;
			
		}
	}
	
	return WII_LIB_TARGET_DEVICE_UNKNOWN;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Handles the process of decrypting data received from a target device.
//!	
//!	@details		Executes the following to decrypt:
//!						-	x = (x [xor] 0x17) + 0x17
//!	
//!	@param[in]		*data				Pointer to data to decrypt.
//!	@param[in]		len					Number of bytes of data to decrypt.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
static WII_LIB_RC WiiLib_Decrypt( uint8_t *data, int8_t len )
{
	for(;len > 0; --len, ++data)
	{
		*data = (((*data ^ 0x17) + 0x17) & 0x00FF);
	}
	
	return WII_LIB_RC_SUCCESS;
	
}