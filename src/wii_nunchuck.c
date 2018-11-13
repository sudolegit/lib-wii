////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to interpret data read from a Wii nunchuck device.
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include "i2c.h"
#include "wii_nunchuck.h"




//==================================================================================================
// PUBLIC METHODS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Process current data for device as the response field from querying the device 
//!					status register.
//!	
//!	@details		Populates the relevant 'device->interfaceCurrent' values by applying the 
//!					appropriate bitfield mapping and merging values.
//!	
//!	@note			The nunchuck does not have multiple (left and right) fields. For situations 
//!					where the nunchuck uses a 'instance' member that is tracked across multipe 
//!					fields, all values receive the current status values (e.g. ZL and ZR both 
//!					populated with the current status value).
//!	
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiNunchuck_ProcessStatusParam(WiiLib_Device *device)
{
	switch( device->target )
	{
		case WII_LIB_TARGET_DEVICE_NUNCHUCK:
		{
			WiiNunchuck_StatusNormal	*bitfields	= (WiiNunchuck_StatusNormal*)&device->dataCurrent[0];
			
			device->interfaceCurrent.buttonC			= !(bitfields->buttonC & 0x01);
			device->interfaceCurrent.buttonZL			= !(bitfields->buttonZ & 0x01);
			device->interfaceCurrent.analogLeftX		= bitfields->analogX;
			device->interfaceCurrent.analogLeftY		= bitfields->analogY;
			device->interfaceCurrent.accelX				= ( (bitfields->accelXHigh << 2) | (bitfields->accelXLow & 0x03) );
			device->interfaceCurrent.accelY				= ( (bitfields->accelYHigh << 2) | (bitfields->accelYLow & 0x03) );
			device->interfaceCurrent.accelZ				= ( (bitfields->accelZHigh << 2) | (bitfields->accelZLow & 0x03) );
			break;
		}
		
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_NUNCHUCK:
		{
			WiiNunchuck_StatusPassThrough	*bitfields	= (WiiNunchuck_StatusPassThrough*)&device->dataCurrent[0];
			
			device->interfaceCurrent.buttonC			= !(bitfields->buttonC & 0x01);
			device->interfaceCurrent.buttonZL			= !(bitfields->buttonZ & 0x01);
			device->interfaceCurrent.analogLeftX		= bitfields->analogX;
			device->interfaceCurrent.analogLeftY		= bitfields->analogY;
			device->interfaceCurrent.accelX				= ( (bitfields->accelXHigh << 2)			| ((bitfields->accelXLow & 0x01) << 1)	);
			device->interfaceCurrent.accelY				= ( (bitfields->accelYHigh << 2)			| ((bitfields->accelYLow & 0x01) << 1)	);
			device->interfaceCurrent.accelZ				= ( ((bitfields->accelZHigh & 0xFE) << 2)	| ((bitfields->accelZLow & 0x03) << 1)	);
			break;
		}
		
		default:
			return WII_LIB_RC_TARGET_ID_MISMATCH;
		
	}
	
	// Keep things consistent by duplicating left and right values for [relevant] nunchuck features.
	device->interfaceCurrent.buttonZR				= device->interfaceCurrent.buttonZL;
	device->interfaceCurrent.analogRightX			= device->interfaceCurrent.analogLeftX;
	device->interfaceCurrent.analogRightY			= device->interfaceCurrent.analogLeftY;
	
	return WII_LIB_RC_SUCCESS;
	
}

