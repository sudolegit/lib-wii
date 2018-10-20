////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to interact with the I2C bus and peripheral devices.
//!	
//!	@note			For references to functions, macros, and constants provided by Microchip, please 
//!					see the library file:
//!						-	[mc32-install]\pic32-libs\include\peripheral\i2c.h
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include <plib.h>
#include "i2c.h"


//==================================================================================================
// PRIVATE FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
I2C_RC		I2C_StartTransfer(I2C_Device *device, BOOL restart);
I2C_RC		I2C_StopTransfer( I2C_Device *device );




//==================================================================================================
// PUBLIC METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Initializes the target I2C port.
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct. Values used to define 
//!										target to initialize and how it should be initialized.
//!	@param[in]		pbClk				Current peripheral bus clock for device (referenced during 
//!										I2C initialization).
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_InitPort(I2C_Device *device, uint32_t pbClk)
{
	I2CEnable(device->module, FALSE);
	
	I2CConfigure(device->module, device->config);
	I2CSetFrequency(device->module, pbClk, device->clkFreq);
	
	I2CEnable(device->module, TRUE);
	
	return I2C_RC_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Transmit a single byte of data.
//!	
//!	@details		Starts I2C transfer process and waits until bus ready to transmit. Once ready, 
//!					sends out the provided byte of data and polls until tranmission is done. The 
//!					I2C bus is brought to an inactive [free] state before returning a code 
//!					indicating if message was acknowledged or not (fully ack'd message == success).
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		data				Byte of data to transmit out over bus.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_TransmitByte(I2C_Device *device, uint8_t data)
{
	while( I2C_StartTransfer(device, FALSE) != I2C_SUCCESS );
	
	while( !I2CTransmitterIsReady( device->module ) );
	
	if( I2CSendByte( device->module, data ) != I2C_SUCCESS )
		return I2C_RC_SEND_BYTE_BUFFER_FAILED;
	
	while( !I2CTransmissionHasCompleted( device->module ) );
	
	I2C_StopTransfer(device);
	
	if( !I2CByteWasAcknowledged( device->module ) )
		return I2C_RC_NO_ACK;
	
	return I2C_RC_SUCCESS;
	
}




//==================================================================================================
// PRIVATE METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Set start condition for I2C bus.
//!	
//!	@details		Logic branches based on the 'restart' flag provided. For both start and restart 
//!					conditions, functions from the Microchip peripheral library are invoked to 
//!					execute the relevant command.
//!	
//!	@warning		This is a blocking function. It will not return until the bus is idle.
//!	
//!	@note			A "restart" condition is sending a start message while a data payload is already 
//!					actively in transfer. 
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		restart				Flag indicating if a 'restart' message should be sent.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_StartTransfer(I2C_Device *device, BOOL restart)
{
	if( restart )
	{
		if( I2CRepeatStart( device->module ) != I2C_SUCCESS )
			return I2C_RC_RESTART_FAILED;
	} else
	{
		while( !I2CBusIsIdle( device->module ) );
		
		if( I2CStart( device->module ) != I2C_SUCCESS )
			return I2C_RC_START_FAILED;
		
	}
	
	while( !(I2CGetStatus(device->module) & I2C_START) );
	
	return I2C_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Set stop condition for I2C bus.
//!	
//!	@details		Utilizes functions from the Microchip peripheral library to execute the relevant 
//!					command.
//!	
//!	@warning		This is a blocking function. It will not return until the bus is idle.
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_StopTransfer( I2C_Device *device )
{
	I2CStop( device->module ); 
	
	while( !(I2CGetStatus(device->module) & I2C_STOP) );
	
	return I2C_RC_SUCCESS;
}
