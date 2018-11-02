////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Implements functions used to interpret data read from a Wii classic controller 
//!					device.
////////////////////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include "i2c.h"
#include "wii_classic_controller.h"




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
//!	@param[in]		*device				Instance of 'WiiLib_Device{}'.
//!	
//!	@returns		Return code corresponding to an entry in the 'WII_LIB_RC' enum (zero == success; 
//!					non-zero == error code). Please see enum definition for details.
////////////////////////////////////////////////////////////////////////////////////////////////////
WII_LIB_RC WiiNunchuck_ProcessStatusParam(WiiLib_Device *device)
{
	switch( device->target )
	{
		case WII_LIB_TARGET_DEVICE_CLASSIC_CONTROLLER:
		{
			WiiClassic_StatusNormal	*bitfields			= (WiiClassic_StatusNormal*)&device->dataCurrent[0];
			
			// Discrete Buttons:
			device->interfaceCurrent.buttonA			= bitfields->buttonA;
			device->interfaceCurrent.buttonB			= bitfields->buttonB;
			device->interfaceCurrent.buttonX			= bitfields->buttonX;
			device->interfaceCurrent.buttonY			= bitfields->buttonY;
			device->interfaceCurrent.buttonZL			= bitfields->buttonZLeft;
			device->interfaceCurrent.buttonZR			= bitfields->buttonZRight;
			device->interfaceCurrent.buttonMinus		= bitfields->buttonMinus;
			device->interfaceCurrent.buttonHome			= bitfields->buttonHome;
			device->interfaceCurrent.buttonPlus			= bitfields->buttonPlus;
			// D-Pad Buttons:
			device->interfaceCurrent.dpadLeft			= bitfields->dpadLeft;
			device->interfaceCurrent.dpadUp				= bitfields->dpadUp;
			device->interfaceCurrent.dpadRight			= bitfields->dpadRight;
			device->interfaceCurrent.dpadDown			= bitfields->dpadDown;
			// Triggers:
			device->interfaceCurrent.buttonLeftTrigger	= bitfields->buttonTriggerLeft;
			device->interfaceCurrent.buttonRightTrigger	= bitfields->buttonTriggerRight;
			device->interfaceCurrent.triggerLeft		= ( ((bitfields->leftTriggerHigh & 0x03) << 3) | (bitfields->leftTriggerLow & 0x07) );
			device->interfaceCurrent.triggerRight		= bitfields->rightTrigger;
			// Analog Joysticks:
			device->interfaceCurrent.analogLeftX		= bitfields->analogLeftX;
			device->interfaceCurrent.analogLeftY		= bitfields->analogLeftY;
			device->interfaceCurrent.analogRightX		= ( ((bitfields->analogRightXHigh & 0x03) << 3) | ((bitfields->analogRightXMid & 0x03) << 1) | (bitfields->analogRightXLow & 0x01) );
			device->interfaceCurrent.analogRightY		= bitfields->analogRightY;
			break;
		}
		
		case WII_LIB_TARGET_DEVICE_MOTION_PLUS_PASS_CLASSIC:
		{
			WiiClassic_StatusPassThrough	*bitfields	= (WiiClassic_StatusPassThrough*)&device->dataCurrent[0];
			
			// Discrete Buttons:
			device->interfaceCurrent.buttonA			= bitfields->buttonA;
			device->interfaceCurrent.buttonB			= bitfields->buttonB;
			device->interfaceCurrent.buttonX			= bitfields->buttonX;
			device->interfaceCurrent.buttonY			= bitfields->buttonY;
			device->interfaceCurrent.buttonZL			= bitfields->buttonZLeft;
			device->interfaceCurrent.buttonZR			= bitfields->buttonZRight;
			device->interfaceCurrent.buttonMinus		= bitfields->buttonMinus;
			device->interfaceCurrent.buttonHome			= bitfields->buttonHome;
			device->interfaceCurrent.buttonPlus			= bitfields->buttonPlus;
			// D-Pad Buttons:
			device->interfaceCurrent.dpadLeft			= bitfields->dpadLeft;
			device->interfaceCurrent.dpadUp				= bitfields->dpadUp;
			device->interfaceCurrent.dpadRight			= bitfields->dpadRight;
			device->interfaceCurrent.dpadDown			= bitfields->dpadDown;
			// Triggers:
			device->interfaceCurrent.buttonLeftTrigger	= bitfields->buttonTriggerLeft;
			device->interfaceCurrent.buttonRightTrigger	= bitfields->buttonTriggerRight;
			device->interfaceCurrent.triggerLeft		= ( ((bitfields->leftTriggerHigh & 0x03) << 3) | (bitfields->leftTriggerLow & 0x07) );
			device->interfaceCurrent.triggerRight		= bitfields->rightTrigger;
			// Analog Joysticks:
			device->interfaceCurrent.analogLeftX		= (bitfields->analogLeftX & 0x3E);
			device->interfaceCurrent.analogLeftY		= (bitfields->analogLeftY & 0x3E);
			device->interfaceCurrent.analogRightX		= ( ((bitfields->analogRightXHigh & 0x03) << 3) | ((bitfields->analogRightXMid & 0x03) << 1) | (bitfields->analogRightXLow & 0x01) );
			device->interfaceCurrent.analogRightY		= bitfields->analogRightY;
			break;
		}
		
		default:
			return WII_LIB_RC_TARGET_ID_MISMATCH;
		
	}
	
	return WII_LIB_RC_SUCCESS;
	
}

