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
//!	@param[out]		*device				Instance of 'WiiLib_Device{}' to populate/utilize.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiLib_Init(I2C_MODULE module, uint32_t pbClk, WII_LIB_TARGET_DEVICE target, WiiLib_Device *device)
{
	WII_LIB_RC		returnCode;
	
	// Prepare I2C port for communication as a master device.
	device->i2c.port.config		= I2C_ENABLE_SLAVE_CLOCK_STRETCHING | I2C_STOP_IN_IDLE;
	device->i2c.port.module		= module;
	device->i2c.port.clkFreq	= I2C_CLOCK_RATE_STANDARD;
	device->i2c.port.ackMode	= I2C_ACK_MODE_ACK;
	
	I2C_InitPort(&device->i2c.port, pbClk);
	
	// Define common I2C device characteristics (common for communicating with all supported Wii devices).
	device->i2c.mode			= I2C_MODE_MASTER;
	device->i2c.addrLength		= I2C_ADDR_LEN_7_BITS;
	
	// Push out initialization settings to target.
	switch(target)
	{
		case WII_LIB_TARGET_DEVICE_NUNCHUK_ENCRYPTED:
			device->i2c.addr	= WII_LIB_I2C_ADDR_NUNCHUCK;
			device->data[0]		= 0x40;
			device->data[1]		= 0x00;
			if( I2C_Transmit( &device->i2c, &device->data[0], 2, TRUE ) != I2C_RC_SUCCESS )
				returnCode = WII_LIB_RC_I2C_ERROR; 
			break;
		
		case WII_LIB_TARGET_DEVICE_NUNCHUK_DECRYPTED:
			device->i2c.addr	= WII_LIB_I2C_ADDR_NUNCHUCK;
			device->data[0]		= 0xF0;
			device->data[1]		= 0x55;
			if( I2C_Transmit( &device->i2c, &device->data[0], 2, TRUE ) == I2C_RC_SUCCESS )
			{
				uint32_t delay = 16666;while(--delay); // Roughly 10 ms delay
				
				device->data[0] = 0xFB;
				device->data[1] = 0x00;
				if( I2C_Transmit( &device->i2c, &device->data[0], 2, TRUE ) != I2C_RC_SUCCESS )
					returnCode = WII_LIB_RC_I2C_ERROR;
			}
			else
			{
				returnCode = WII_LIB_RC_I2C_ERROR;
			}
			break;
		
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER:
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER_PRO:
		default:
			returnCode = WII_LIB_RC_UNSUPPORTED_DEVICE;
			break;
		
	}
	
	return returnCode;
	
}

/*

			{	// Quick hack to confirm able to query data with changes to I2C structs.
				uint32_t x;
				
				x = 100000;while(--x);
				
				buff[0] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
				x = 100000;while(--x);
				
				I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
				x = 100000;while(--x);
				
				buff[0] = 0x40;
				buff[1] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 2, TRUE );
				x = 100000;while(--x);
				
				buff[0] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
				x = 100000;while(--x);
				
				I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
				x = 100000;while(--x);
			}
// */


/*
			buff[0] = 0xF0;
			buff[1] = 0x55;
			I2C_Transmit( &i2c_device, &buff[0], 2, TRUE );
			x = 100000;while(--x);
			
			buff[0] = 0xFB;
			buff[1] = 0x00;
			I2C_Transmit( &i2c_device, &buff[0], 2, TRUE );
			x = 100000;while(--x);
// */

			// */
			
			/*/ 
			buff[0] = 0xFA;
			I2C_TxRx( &i2c_device, &buff[0], 1, &buff[0], 6, TRUE );
			// */
			
			/*/ 
			
			buff[0] = 0x00;
			I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
			x = 100000;while(--x);
			
			
			I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
			x = 1000000;while(--x);
			
			buff[0] = 0x00;
			I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
			x = 100000;while(--x);
			
			
			I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
			x = 1000000;while(--x);
			// */
			
			/*/
			x = 100000;while(--x);
			x = 100000;while(--x);
			x = 100000;while(--x);
			x = 100000;while(--x);
			x = 100000;while(--x);
			x = 100000;while(--x);
			
			buff[0] = 0xFA;
			I2C_Transmit( &i2c_device, &buff[0], 1, FALSE );
			x = 100000;while(--x);
			
			
			I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
			x = 100000;while(--x);
			// */


/*

			{
				buff[0] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
				x = 100000;while(--x);
				
				I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
				x = 100000;while(--x);
				
				buff[0] = 0x40;
				buff[1] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 2, TRUE );
				x = 100000;while(--x);
				
				buff[0] = 0xFA;
				I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
				x = 100000;while(--x);
				
				
				I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
				x = 100000;while(--x);
				
				buff[0] = 0x40;
				buff[1] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 2, TRUE );
				x = 100000;while(--x);
				
				buff[0] = 0x00;
				I2C_Transmit( &i2c_device, &buff[0], 1, TRUE );
				x = 100000;while(--x);
				
				I2C_Receive( &i2c_device, &buff[0], 6, TRUE );
				x = 100000;while(--x);
				
*/