////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to interact with the I2C bus and peripheral devices.
//!	
//!	@note			For references to functions, macros, and constants provided by Microchip, please 
//!					see the library file:
//!						-	[mc32-install]\pic32-libs\include\peripheral\i2c.h
//!	
//!	@warning		Many of the functions used in this file are blocking. Long term, a timeout would 
//!					be a good idea, but skipping for current development timeframe.
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include <plib.h>
#include "i2c.h"




//==================================================================================================
// PRIVATE FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
PRIVATE I2C_RC		I2C_StartTransfer(	I2C_Device *device,	BOOL restart					);
PRIVATE I2C_RC		I2C_StopTransfer(	I2C_Device *device 									);
PRIVATE I2C_RC		I2C_SendByte(		I2C_Device *device,	uint8_t data					);
PRIVATE I2C_RC		I2C_ReadByte(		I2C_Device *device,	uint8_t *data,	BOOL ackByte	);
PRIVATE I2C_RC		I2C_SendAddr(		I2C_Device *device,	BOOL isReadRequest				);




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
//!	@brief			Transmit the identifier + requested data out over I2C.
//!	
//!	@details		Handles bus arbitration (start and stop) and prepends the address indicated 
//!					within the provided 'device' structure's address field.
//!	
//!	@note			The address for the I2C is defined in the 'device' parameter provided to the 
//!					function and is transmitted prior to sending the provided 'len' of data (total 
//!					data transmitted == 'len + 1').
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		*data				Buffer of data to send out over bus.
//!	@param[in]		len					Amount of data to transfer [excluding address of target].
//!	@param[in]		ackRequired			Flag indicating if an acknowledgement of message 
//!										transmission is required.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_Transmit( I2C_Device *device, uint8_t *data, uint32_t len, BOOL ackRequired	)
{
	I2C_RC			returnCode				= I2C_RC_SUCCESS;
	uint32_t		index					= 0;
	
	while( I2C_StartTransfer(device, FALSE) != I2C_SUCCESS );
	
	I2C_SendAddr( device, FALSE );
	
	while( (returnCode == I2C_RC_SUCCESS) && (len > index) )
	{
		I2C_SendByte( device, data[index++] );
		
		if( ackRequired && !I2CByteWasAcknowledged(device->module) )
			returnCode = I2C_RC_NO_ACK;
		
	}
	
	I2C_StopTransfer(device);
	
	return returnCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Read data from the 
//!	
//!	@details		Handles bus arbitration (start and stop). Ensures data is ready to be read and 
//!					supervise the process of invoking a byte-by-byte read of all necessary data.
//!	
//!	@warning		Presumes caller has appropriately allocated space for copying read data into.
//!	
//!	@note			The address for the I2C is defined in the 'device' parameter provided to the 
//!					function and is transmitted prior to sending the provided 'len' of data (total 
//!					data transmitted == 'len + 1').
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		*data				Buffer of data to populate.
//!	@param[in]		len					Amount of data to read (in bytes).
//!	@param[in]		ackRequired			Flag indicating if an acknowledgement of data read should 
//!										occur (the ack mode is defined in the 'device' instance).
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_Receive( I2C_Device *device, uint8_t *data, uint32_t len, BOOL ackMessages )
{
	I2C_RC			returnCode				= I2C_RC_SUCCESS;
	uint32_t		index					= 0;
	
	while( I2C_StartTransfer(device, FALSE) != I2C_SUCCESS );
	
	I2C_SendAddr( device, TRUE );
	
	while( (returnCode == I2C_RC_SUCCESS) && (len > index) )
	{
		returnCode = I2C_ReadByte( device, (data + index++), ackMessages );
	}
	
	I2CReceiverEnable( device->module, FALSE );
	
	I2C_StopTransfer(device);
	
	return returnCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Handle a combined write + read process over I2C to a target device.
//!	
//!	@details		Handles bus arbitration (start and stop) and prepends the address indicated 
//!					within the provided 'device' structure's address field. Ensures data is ready to 
//!					be read and supervise the process of invoking a byte-by-byte read of all 
//!					necessary data.
//!	
//!	@warning		Presumes caller has appropriately allocated space for copying read data into.
//!	
//!	@note			The address for the I2C is defined in the 'device' parameter provided to the 
//!					function and is transmitted prior to sending the provided 'len' of data (total 
//!					data transmitted == 'len + 1').
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		*dataTx				Buffer of data to send out over bus.
//!	@param[in]		lenTx				Amount of data to transfer [excluding address of target].
//!	@param[in]		*dataRx				Buffer of data to populate.
//!	@param[in]		lenRx				Amount of data to read (in bytes).
//!	@param[in]		ackRequired			Flag indicating if an acknowledgement of message should 
//!										occur during tx and be expected during rx.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
I2C_RC I2C_TxRx( I2C_Device *device, uint8_t *dataTx, uint32_t lenTx, uint8_t *dataRx, uint32_t lenRx, BOOL ack )
{
	I2C_RC			returnCode				= I2C_RC_SUCCESS;
	uint32_t		index					= 0;
	
	while( I2C_StartTransfer(device, FALSE) != I2C_SUCCESS );
	
	// Transmit provided data.
	I2C_SendAddr( device, FALSE );
	
	while( (returnCode == I2C_RC_SUCCESS) && (lenTx > index) )
	{
		I2C_SendByte( device, dataTx[index++] );
		
		if( ack && !I2CByteWasAcknowledged(device->module) )
			returnCode = I2C_RC_NO_ACK;
		
	}
	
	if( returnCode == I2C_RC_SUCCESS )
	{	
		// Restart transmission rather than stopping and starting it.
		while( I2C_StartTransfer(device, TRUE) != I2C_SUCCESS );
		uint32_t delay = 16666;while(--delay); // Roughly 10 ms delay
		
		// Read in the requested number of data bytes.
		I2C_SendAddr( device, TRUE );
		
		index = 0;
		while( (returnCode == I2C_RC_SUCCESS) && (lenRx > index) )
		{
			returnCode = I2C_ReadByte( device, (dataRx + index++), ack );
		}
		
		I2CReceiverEnable( device->module, FALSE );
	}	
	
	I2C_StopTransfer(device);
	
	return returnCode;
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
PRIVATE I2C_RC I2C_StartTransfer(I2C_Device *device, BOOL restart)
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
PRIVATE I2C_RC I2C_StopTransfer( I2C_Device *device )
{
	I2CStop( device->module ); 
	
	while( !(I2CGetStatus(device->module) & I2C_STOP) );
	
	return I2C_RC_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Send a single byte of data.
//!	
//!	@details		Waits until bus ready to transmit. Once the bus is ready, sends out the provided 
//!					byte of data and polls until tranmission is done.
//!	
//!	@note			This function does NOT handle bus start / stop. It is up to the caller to manage 
//!					the full bus arbitration proces.
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		data				Byte of data to send out over bus.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
PRIVATE I2C_RC I2C_SendByte(I2C_Device *device, uint8_t data)
{
	while( !I2CTransmitterIsReady( device->module ) );
	
	if( I2CSendByte( device->module, data ) != I2C_SUCCESS )
		return I2C_RC_SEND_BYTE_BUFFER_FAILED;
	
	while( !I2CTransmissionHasCompleted( device->module ) );
	
	if( !I2CByteWasAcknowledged( device->module ) )
		return I2C_RC_NO_ACK;
	
	return I2C_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Read a single byte of data and optionally send an acknowledgement signal.
//!	
//!	@details		Waits until recevied data is available and updates the provided data pointer 
//!					with the value read. If the acknowledgement flag is set, attempts to ack or nack
//!					 and poll until ack or nack is successful.
//!	
//!	@note			This function does NOT handle bus start / stop. It is up to the caller to manage 
//!					the full bus arbitration proces.
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[out]		*data				Where to save byte read in from bus.
//!	@param[out]		ackByte				Boolean flag indicating if a ACK/NACK bit should be 
//!										transmitted (acknowledging receipt of byte). This is a 
//!										boolean flag indicating if the process of acknowledging will 
//!										occur. The actual NACK or ACK value is defined in the 
//!										provided 'device' instance.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
PRIVATE I2C_RC I2C_ReadByte( I2C_Device *device, uint8_t *data, BOOL ackByte )
{
	if( I2CReceiverEnable( device->module, TRUE ) != I2C_SUCCESS )
		return I2C_RC_RECEIVE_OVERFLOW;
	
	while( !I2CReceivedDataIsAvailable(device->module) );
	
	if( ackByte )
		I2CAcknowledgeByte( device->module, (BOOL)device->ackMode );
	
	*data = I2CGetByte( device->module );
	
	if( ackByte )
		while (!I2CAcknowledgeHasCompleted( device->module ));
	
	return I2C_RC_SUCCESS;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Transmits the address to a target with the appropriate read/write bit set.
//!	
//!	@details		Serves as a wrapper to the private 'I2C_SendByte' method with the address 
//!					formatted using the Microchip plib library methods.
//!	
//!	@note			This function does NOT handle bus start / stop. It is up to the caller to manage 
//!					the full bus arbitration proces.
//!	
//!	@note			Supports only the address sizes defined in the 'I2C_ADDR_LEN{}' enum.
//!	
//!	@param[in]		*device				Instance of 'I2C_Device{}' struct.
//!	@param[in]		isReadRequest		Flag indicating if the R/W bit in the address should match 
//!										'I2C_READ' or 'I2C_WRITE'. A TRUE value == use 'I2C_READ'.
//!	
//!	@returns		Return code corresponding to an entry in the 'I2C_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
PRIVATE I2C_RC I2C_SendAddr( I2C_Device *device, BOOL isReadRequest)
{
	I2C_RC			returnCode				= I2C_RC_SUCCESS;
	BOOL			rwFlag					= (isReadRequest) ? I2C_READ : I2C_WRITE;
	
	if( device->addrLength == I2C_ADDR_LEN_7_BITS )
	{
		I2C_7_BIT_ADDRESS				addr;
		I2C_FORMAT_7_BIT_ADDRESS(		addr,	device->addr,	rwFlag	);
		returnCode = I2C_SendByte(		device,	addr.byte				);
	}
	else if( device->addrLength == I2C_ADDR_LEN_10_BITS )
	{
		I2C_10_BIT_ADDRESS				addr;
		I2C_FORMAT_10_BIT_ADDRESS(		addr,	device->addr,	rwFlag	);
		returnCode		= I2C_SendByte(	device,	addr.first_byte			);
		if(returnCode != I2C_RC_SEND_BYTE_BUFFER_FAILED)
			returnCode	= I2C_SendByte(	device,	addr.second_byte		);
	}
	
	return returnCode;
	
}

