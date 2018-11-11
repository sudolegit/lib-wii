////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to abstract away interacting with Wii devices over 
//!					I2C.
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include "i2c.h"
#include "wii_lib.h"




//==================================================================================================
//	PRIVATE FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
static WII_LIB_TARGET_DEVICE	WiiLib_DetermineDeviceType(		WiiLib_Device *device					);
static BOOL						WiiLib_ValidateDataReceived(	uint8_t *data,			uint32_t len	);
static WII_LIB_RC				WiiLib_Decrypt(					uint8_t *data,			int8_t len		);
static WII_LIB_RC				WiiLib_UpdateInterfaceTracking(	WiiLib_Device *device					);




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
//!										decrypted.
//!	@param[in]		*device				Instance of 'WiiLib_Device{}' to populate/utilize.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_Init( I2C_MODULE module, uint32_t pbClk, WII_LIB_TARGET_DEVICE target, BOOL decryptData, WiiLib_Device *device )
{
	// Presume delay not yet initialized and initialize delay module. Even if this is not the case, 
	// should have no harm (in theory/so long as pbClk not different between devices).
	Delay_Init(pbClk);
	
	// Define I2C port for communication as a master device.
	device->i2c.port.config				= I2C_ENABLE_SLAVE_CLOCK_STRETCHING | I2C_STOP_IN_IDLE;
	device->i2c.port.module				= module;
	device->i2c.port.clkFreq			= I2C_CLOCK_RATE_STANDARD;
	device->i2c.port.ackMode			= I2C_ACK_MODE_ACK;
	
	// Define processing delays for I2C communication.
	device->i2c.delayAfterSend_Ms		= WII_LIB_I2C_DELAY_POST_SEND_MS;
	device->i2c.delayAfterReceive_Ms	= WII_LIB_I2C_DELAY_POST_READ_MS;
	device->i2c.delayBetweenTxRx_Ms		= WII_LIB_I2C_DELAY_BETWEEN_TX_RX_MS;
	
	// Set flag controlling if relative positioning is enabled (when enabled, automatically 
	// calculates relative position each time status data is received).
	#if defined(WII_LIB_DEFAULT_CALCULATE_RELATIVE_POSITION) && WII_LIB_DEFAULT_CALCULATE_RELATIVE_POSITION == TRUE
	WiiLib_EnableRelativePosition( device );
	#else
	device->calculateRelativePosition	= WiiLib_DisableRelativePosition( device );
	#endif
	
	// Define common I2C device characteristics (common for communicating with all supported Wii devices).
	device->i2c.mode					= I2C_MODE_MASTER;
	device->i2c.addrLength				= I2C_ADDR_LEN_7_BITS;
	device->target						= target;
	device->dataEncrypted				= (uint8_t)!(decryptData);
	
	// Define device-specific settings.
	switch(device->target)
	{
		case WII_LIB_TARGET_DEVICE_UNKNOWN:
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
	if( I2C_InitPort(&device->i2c.port, pbClk, FALSE) != I2C_RC_SUCCESS )
		return WII_LIB_RC_I2C_ERROR;
	
	Delay_Ms(WII_LIB_DELAY_I2C_SETTLE_TIME_MS);
	
	// Define initial device status and execute maintenance tasks to handle initialization / etc.
	device->failedParamQueryCount		= 0;
	device->status						= WII_LIB_DEVICE_STATUS_NOT_INITIALIZED;
	return WiiLib_DoMaintenance(device);
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Checks error count and status for provided device and performs any maintenance 
//!					tasks that are deemed necessary. Aim is to use this method to gracefully handle 
//!					initialization and error conditions.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}' defining target device 
//!										interaction.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_DoMaintenance( WiiLib_Device *device )
{
	if( device->status == WII_LIB_DEVICE_STATUS_STRUCTURE_NOT_DEFINED )
	{
		// We cannot maintain anything without first being provided configuration details through 
		// the call to the initialization function.
		return WII_LIB_RC_TARGET_STRUCTURE_NOT_DEFINED;
	}
	else if( device->failedParamQueryCount > WII_LIB_MAX_FAILURES_BEFORE_DISABLING )
	{
		device->status = WII_LIB_DEVICE_STATUS_DISABLED;
		return WII_LIB_RC_DEVICE_DISABLED;
	}
	else if( device->failedParamQueryCount > WII_LIB_MAX_FAILURES_BEFORE_RECONFIGURING )
	{
		device->status = WII_LIB_DEVICE_STATUS_CONFIGURING;
		return WiiLib_ConfigureDevice(device);
	}
	else if( device->status == WII_LIB_DEVICE_STATUS_NOT_INITIALIZED )
	{
		// NOTE:	It is vital that the 'not initialized' state follows after the checks against 
		//			error counts. Thus, if the device is not found at boot, querying for it will 
		//			halt eventually.
		WII_LIB_RC		returnCode						= WII_LIB_RC_SUCCESS;
		uint8_t			connectionAttemptsReamining		= WII_LIB_MAX_CONNECTION_ATTEMPTS;
		
		// Attempt to connect to the target device until we are successful, run out of attempts, or connect successfully but to the wrong target type.
		do
		{
			if( connectionAttemptsReamining != WII_LIB_MAX_CONNECTION_ATTEMPTS )
				Delay_Ms( WII_LIB_DELAY_AFTER_CONNECTION_ATTEMPT_MS );

			returnCode = WiiLib_ConnectToTarget( device );

			if( returnCode == WII_LIB_RC_SUCCESS || returnCode == WII_LIB_RC_TARGET_ID_MISMATCH )
			{
				device->status = WII_LIB_DEVICE_STATUS_ACTIVE;
				return WII_LIB_RC_SUCCESS;
			}

		} while( --connectionAttemptsReamining );
		
		return WII_LIB_RC_TARGET_NOT_INITIALIZED;
	} 
	else
	{
		// Presume no error at this point or error not large enough to matter.
		device->status = WII_LIB_DEVICE_STATUS_ACTIVE;
		return WII_LIB_RC_SUCCESS;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Attempts to connect to target device.
//!	
//!	@details		Pushes out initialization messages to target device. If device ack's messages, 
//!					attempts to validate the target ID. If successful, device is up and running, 
//!					but before exiting the function, grabs the initial device status (queries 
//!					WII_LIB_PARAM_STATUS).
//!	
//!	@note			Only attempts to connect once. Repeated connection attempts (and any desired 
//!					delays) should be handled by caller.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}' to utilize.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_ConnectToTarget( WiiLib_Device *device )
{
	WII_LIB_TARGET_DEVICE		targetValueRead;
	
	// Push out configuration settings to device (uses flags defined when 'device' was initialized).
	if( WiiLib_ConfigureDevice( device ) != WII_LIB_RC_SUCCESS )
		return WII_LIB_RC_TARGET_NOT_INITIALIZED;
	
	// Confirm target is correct target by confirming able to query device ID and that the returned 
	// value matches the desired value.
	targetValueRead = WiiLib_DetermineDeviceType(device);
	if( targetValueRead != device->target  && device->target != WII_LIB_TARGET_DEVICE_UNKNOWN )
	{
		device->target	= targetValueRead;
		return WII_LIB_RC_TARGET_ID_MISMATCH;
	}
	
	Delay_Ms(WII_LIB_DELAY_AFTER_CONFIRM_ID_MS);
	
	// Record current status values from target and use those as the home position for the device.
	return WiiLib_SetNewHomePosition( device );
	
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
	uint8_t			buff[2];
	
	// Initialize in most basic form. This leaves data in an encypted state.
	if( device->dataEncrypted )
	{
		buff[0] = 0x40;
		buff[1] = 0x00;
		if( I2C_Transmit( &device->i2c, &buff[0], 2, TRUE ) != I2C_RC_SUCCESS )
			return WII_LIB_RC_I2C_ERROR;
	}
	// Initialize such that future data transmitted is no longer encrypted.
	else
	{
		buff[0] = 0xF0;
		buff[1] = 0x55;
		if( I2C_Transmit( &device->i2c, &buff[0], 2, TRUE ) == I2C_RC_SUCCESS )
		{
			Delay_Ms(WII_LIB_DELAY_AFTER_CONFIG_MESSAGE_MS);
			
			buff[0] = 0xFB;
			buff[1] = 0x00;
			if( I2C_Transmit( &device->i2c, &buff[0], 2, TRUE ) != I2C_RC_SUCCESS )
				return WII_LIB_RC_I2C_ERROR;
		}
		else
		{
			return WII_LIB_RC_I2C_ERROR;
		}
	}
	
	Delay_Ms(WII_LIB_DELAY_AFTER_CONFIG_MESSAGE_MS);
	
	return WII_LIB_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Handles process of initiating and reading the response for querying a parameter 
//!					value from the target device.
//!	
//!	@details		Verifies the parameter requested is supported [a known parameter] before 
//!					utilizing temporary buffers to request and read data over the I2C bus. If the 
//!					data is read correctly (valid reply, decrypted appropriately, etc.), the results 
//!					are copied into the 'device->dataCurrent[]' before returning success.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}' defining target device 
//!										interaction.
//!	@param[in]		param				Parameter value to query. Must match one of the supported 
//!										values defined in the 'WII_LIB_PARAM{}' enum.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_QueryParameter( WiiLib_Device *device, WII_LIB_PARAM param )
{
	uint8_t			buffIn[WII_LIB_PARAM_REQUEST_LEN]		= { param };
	uint8_t			buffOut[WII_LIB_MAX_PAYLOAD_SIZE]		= {0};
	uint8_t			decryptData								= device->dataEncrypted;
	uint32_t		lenIn									= WII_LIB_PARAM_REQUEST_LEN;
	uint32_t		lenOut;
	
	// Disable queries based on device status to prevent flooding the I2C bus if device 
	// non-responsive.
	if( device->status == WII_LIB_DEVICE_STATUS_DISABLED )
		return WII_LIB_RC_DEVICE_DISABLED;
	
	// Validate parameter ID provided and define response length (amount to query over I2C bus).
	switch( param )
	{
		case WII_LIB_PARAM_STATUS:
			// HACK:	Presently we need to push the configuration data out each time we communicate 
			//			with the classic controller. Handling this automatically for status data queries.
			if( device->target == WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER || device->target == WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC )
				WiiLib_ConfigureDevice(device);
		case WII_LIB_PARAM_DEVICE_TYPE:
			lenOut		= WII_LIB_PARAM_RESPONSE_LEN_DEFAULT;
			break;
		
		case WII_LIB_PARAM_RAW_DATA:
			lenOut = WII_LIB_PARAM_RESPONSE_LEN_EXTENDED;
			break;
		
		default:
			return WII_LIB_RC_UNKOWN_PARAMETER;
			break;
	}
	
	// Execute I2C query, validate results, and (if necessary) decrypt value(s) received.
	if( I2C_TxRx( &device->i2c, &buffIn[0], lenIn, &buffOut[0], lenOut, TRUE, FALSE ) == I2C_RC_SUCCESS )
	{
		if( !WiiLib_ValidateDataReceived(&buffOut[0], lenOut) )
		{
			memset( &device->dataCurrent[0], 0, WII_LIB_MAX_PAYLOAD_SIZE );
			++device->failedParamQueryCount;
			return WII_LIB_RC_DATA_RECEIVED_IS_INVALID;
		}
		
		if( decryptData )
		{
			if( WiiLib_Decrypt( &buffOut[0], WII_LIB_ID_LENGTH ) != WII_LIB_RC_SUCCESS )
			{
				++device->failedParamQueryCount;
				return WII_LIB_RC_UNABLE_TO_DECRYPT_DATA_RECEIVED;
			}
		}
		
		// Save to store date received. Copy temporary buffer over to destination.
		memcpy( &device->dataCurrent[0], &buffOut[0], WII_LIB_MAX_PAYLOAD_SIZE );
		
		// If we reach this point we know communication over I2C is valid and can clear the error 
		// flag count.
		device->failedParamQueryCount = 0;
		
		// Process data to infer the state of the user interface if query was for status:
		if( param == WII_LIB_PARAM_STATUS )
			return WiiLib_UpdateInterfaceTracking( device );
		
		return WII_LIB_RC_SUCCESS;
		
	}
	
	++device->failedParamQueryCount;
	return WII_LIB_RC_I2C_ERROR;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Refreshes tracking values for the target device's status bits.
//!	
//!	@details		Uses the 'WiiLib_QueryParameter()' to execute a query for 'WII_LIB_PARAM_STATUS' 
//!					and store the result within the buffer in the 'device->dataCurrent[]' buffer.
//!	
//!	@note			This is mainly meant to serve as a simple wrapper to make it easier for app 
//!					development to not need to know much about the internals of the I2C query 
//!					process.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_PollStatus( WiiLib_Device *device )
{
	return WiiLib_QueryParameter( device, WII_LIB_PARAM_STATUS );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Refreshes tracking values for the target device's status bits.
//!	
//!	@details		Uses the 'WiiLib_QueryParameter()' to execute a query for 'WII_LIB_PARAM_STATUS' 
//!					and store the result within the buffer in the 'device->dataBaseline[]' buffer.
//!	
//!	@note			This is mainly meant to serve as a simple wrapper to make it easier for app 
//!					development to not need to know much about the internals of the I2C query 
//!					process.
//!	
//!	@note			This could be handled more efficiently, but presently focused on encapsulation 
//!					and not too worried about the secondary memcpy() event.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_SetNewHomePosition( WiiLib_Device *device )
{
	WII_LIB_RC		returnCode;
	
	if( ! device->calculateRelativePosition )
		return WII_LIB_RC_RELATIVE_POSITION_FEATURE_DISABLED;
	
	returnCode = WiiLib_PollStatus( device );
	
	if( returnCode == WII_LIB_RC_SUCCESS )
		memcpy( (void *)&device->interfaceHome, (void *)&device->interfaceCurrent, sizeof(WiiLib_Device) );
	
	return returnCode;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Simple wrapper to handle enabling of relative positioning.
//!	
//!	@note			No values interface tracking values are modified by this function. This function 
//!					sole aim is to wrap the enable/disable flag for if relative position information 
//!					is tracked and calculated.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_EnableRelativePosition( WiiLib_Device *device )
{
	device->calculateRelativePosition = TRUE;
	return WII_LIB_RC_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Simple wrapper to handle enabling of relative positioning.
//!	
//!	@note			No values interface tracking values are modified by this function. This function 
//!					sole aim is to wrap the enable/disable flag for if relative position information 
//!					is tracked and calculated.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_DisableRelativePosition( WiiLib_Device *device )
{
	device->calculateRelativePosition = FALSE;
	return WII_LIB_RC_SUCCESS;
}




//==================================================================================================
//	PRIVATE METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Handles the process of determining the target device type based on reading its
//!					device ID register.
//!	
//!	@details		Queries the device for it's identifier by writing 'WII_LIB_PARAM_DEVICE_TYPE' to 
//!					the target and reading back the 6-byte value. The value is decrypted if 
//!					necessary before then comparing it against the expected ID values.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Entry from 'WII_LIB_TARGET_DEVICE{}' that represents the target device 
//!					determined.
////////////////////////////////////////////////////////////////////////////////////////////////////
static WII_LIB_TARGET_DEVICE WiiLib_DetermineDeviceType( WiiLib_Device *device )
{
	if( WiiLib_QueryParameter( device, WII_LIB_PARAM_DEVICE_TYPE ) == I2C_RC_SUCCESS )
	{
		if( !memcmp( (uint8_t [])WII_LIB_ID_NUNCHUCK,						&device->dataCurrent[0],	WII_LIB_ID_LENGTH ) )
			return WII_LIB_TARGET_DEVICE_NUNCHUCK;
		
		if( !memcmp( (uint8_t [])WII_LIB_ID_CLASSIC_CONTROLLER,				&device->dataCurrent[0],	WII_LIB_ID_LENGTH ) )
			return WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER;
		
		if( !memcmp( (uint8_t [])WII_LIB_ID_WII_MOTION_PLUS, 				&device->dataCurrent[0],	WII_LIB_ID_LENGTH ) )
			return WII_LIB_TARGET_DEVICE_MOTION_PLUS;
		
		if( !memcmp( (uint8_t [])WII_LIB_ID_WII_MOTION_PLUS_PASS_NUNCHUCK,	&device->dataCurrent[0],	WII_LIB_ID_LENGTH ) )
			return WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_NUNCHUCK;
		
		if( !memcmp( (uint8_t [])WII_LIB_ID_WII_MOTION_PLUS_PASS_CLASSIC,	&device->dataCurrent[0],	WII_LIB_ID_LENGTH ) )
			return WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC;
		
		return WII_LIB_TARGET_DEVICE_UNSUPPORTED;
		
	}
	
	return WII_LIB_TARGET_DEVICE_UNKNOWN;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Verifies the data provided is not a known set of invalid byte(s).
//!	
//!	@details		Confirms data is ready was ready to be read from the target device (did not 
//!					receive all 0xFF bytes) and returns the result. Long term, any error codes can 
//!					and should be checked by this function.
//!	
//!	@param[in]		*data				Pointer to data to validate.
//!	@param[in]		len					Number of bytes of data to validate.
//!	
//!	@retval			TRUE				Data is valid.
//!	@retval			TRUE				Data is not valid.
////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL WiiLib_ValidateDataReceived( uint8_t *data, uint32_t len )
{
	static uint8_t	notReady[WII_LIB_MAX_PAYLOAD_SIZE];
	
	// Confirm data is not all '0xFF' (indicates no data ready to read).
	memset(&notReady[0], 0xFF, WII_LIB_MAX_PAYLOAD_SIZE);
	if( memcmp( &notReady[0], data, len ) == 0 )
		return FALSE;
	
	return TRUE;
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


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Wrapper to invoke the appropriate target-specific processing function to 
//!					interpret the current status data.
//!	
//!	@note			Presumes data available in 'device->dataCurrent[]' is a valid payload from 
//!					querying status data.
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Entry from 'WII_LIB_TARGET_DEVICE{}' that represents the target device 
//!					determined.
////////////////////////////////////////////////////////////////////////////////////////////////////
static WII_LIB_RC WiiLib_UpdateInterfaceTracking( WiiLib_Device *device )
{
	WII_LIB_RC		returnCode;
	
	switch(device->target)
	{
		case WII_LIB_TARGET_DEVICE_NUNCHUCK:
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_NUNCHUCK:
			returnCode = WiiNunchuck_ProcessStatusParam( device );
			break;
		
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER:
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC:
			returnCode = WiiClassic_ProcessStatusParam( device );
			break;
		
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS:
			// DEVELOPMENT NEEDED.
			break;
		
		default:
			return WII_LIB_RC_UNSUPPORTED_DEVICE;
		
	}
	
	// Calculate relative positioning values.
	if( returnCode == WII_LIB_RC_SUCCESS && device->calculateRelativePosition )
	{
		memcpy( (void*)&device->interfaceRelative, (void*)&device->interfaceHome, WII_LIB_MAX_PAYLOAD_SIZE );
		
		device->interfaceRelative.triggerLeft	= device->interfaceCurrent.triggerLeft	- device->interfaceHome.triggerLeft;
		device->interfaceRelative.triggerRight	= device->interfaceCurrent.triggerRight	- device->interfaceHome.triggerRight;
		device->interfaceRelative.analogLeftX	= device->interfaceCurrent.analogLeftX	- device->interfaceHome.analogLeftX;
		device->interfaceRelative.analogLeftY	= device->interfaceCurrent.analogLeftY	- device->interfaceHome.analogLeftY;
		device->interfaceRelative.analogRightX	= device->interfaceCurrent.analogRightX	- device->interfaceHome.analogRightX;
		device->interfaceRelative.analogRightY	= device->interfaceCurrent.analogRightY	- device->interfaceHome.analogRightY;
		device->interfaceRelative.accelX		= device->interfaceCurrent.accelX		- device->interfaceHome.accelX;
		device->interfaceRelative.accelY		= device->interfaceCurrent.accelY		- device->interfaceHome.accelY;
		device->interfaceRelative.accelZ		= device->interfaceCurrent.accelZ		- device->interfaceHome.accelZ;
		device->interfaceRelative.gyroX			= device->interfaceCurrent.gyroX		- device->interfaceHome.gyroX;
		device->interfaceRelative.gyroY			= device->interfaceCurrent.gyroY		- device->interfaceHome.gyroY;
		device->interfaceRelative.gyroZ			= device->interfaceCurrent.gyroZ		- device->interfaceHome.gyroZ;
	}
	
	return returnCode;
	
}

