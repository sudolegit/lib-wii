////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants, macros, and constant functions available for the "wii 
//!					classic controller" support.
//!	
//!	@note			This file is pulled into "wii_lib.h" automatically.
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WII_CLASSIC_CONTROLLER__
#define	__WII_CLASSIC_CONTROLLER__


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include <stdint.h>
#include "wii_lib.h"




//==================================================================================================
//	CONSTANTS => THRESHOLDS
//--------------------------------------------------------------------------------------------------
// NOTE:	The following threshold values are not used in the library but are good reference values 
//			for the amount of noise to ignore off of a relative zero position in an application.
#define	WII_CLASSIC_CONTROLLER_THRESHOLD_ANALOG_LEFT		15					//!< Threshold off of relative position to consider accelerometer data to be active. Has a range of ~60 (15 ~= 25% of range of movement).
#define	WII_CLASSIC_CONTROLLER_THRESHOLD_ANALOG_RIGHT		8					//!< Threshold off of relative position to consider left analog joystick to be active. Has a range of ~30 (8 ~= 25% of range of movement).
#define	WII_CLASSIC_CONTROLLER_THRESHOLD_TRIGGERS			8					//!< Threshold off of relative position to conider and left or right trigger button to be active. Has a range of ~30 (8 ~25% of range of movement).




//==================================================================================================
//	TYPEDEFS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines bitfield interaction for status field queries from classic controllers.
//!	
//!	@note			The definitions below are for use when directly connected to a classic 
//!					controller.
//!	
//!	@warning		The following is in little-endian format.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiClassic_StatusNormal
{
	// Byte-01:
	uint8_t								analogLeftX:		6;					//!< Bits <5:0> for the left analog joystic along the x-axis.
	uint8_t								analogRightXHigh:	2;					//!< Bits <4:3> for the right analog joystic along the x-axis.
	// Byte-02:
	uint8_t								analogLeftY:		6;					//!< Bits <5:0> for the left analog joystic along the y-axis.
	uint8_t								analogRightXMid:	2;					//!< Bits <2:1> for the right analog joystic along the x-axis.
	// Byte-03:
	uint8_t								analogRightY:		5;					//!< Bits <4:0> for the right analog joystic along the y-axis.
	uint8_t								leftTriggerHigh:	2;					//!< Bits <4:3> for the left trigger.
	uint8_t								analogRightXLow:	1;					//!< Bit  <0>   for the right analog joystick along the x-axis.
	// Byte-04:
	uint8_t								rightTrigger:		5;					//!< Bits <4:0> for the right trigger.
	uint8_t								leftTriggerLow:		3;					//!< Bits <2:0> for the left trigger.
	// Byte-05:
	uint8_t								RESERVED:			1;					//!< Reserved bit used when passing through the Wii Motion Plus (should be high == 1).
	uint8_t								buttonTriggerRight:	1;					//!< Bit indicating status of button->-trigger-right.
	uint8_t								buttonPlus:			1;					//!< Bit indicating status of button->+.
	uint8_t								buttonHome:			1;					//!< Bit indicating status of button->home.
	uint8_t								buttonMinus:		1;					//!< Bit indicating status of button->-.
	uint8_t								buttonTriggerLeft:	1;					//!< Bit indicating status of button->-trigger-left.
	uint8_t								dpadDown:			1;					//!< Bit indicating status of d-pad->down.
	uint8_t								dpadRight:			1;					//!< Bit indicating status of d-pad->right.
	// Byte-06:
	uint8_t								dpadUp:				1;					//!< Bit indicating status of d-pad->up.
	uint8_t								dpadLeft:			1;					//!< Bit indicating status of d-pad->left.
	uint8_t								buttonZRight:		1;					//!< Bit indicating status of button->z-right.
	uint8_t								buttonX:			1;					//!< Bit indicating status of button->x.
	uint8_t								buttonA:			1;					//!< Bit indicating status of button->a.
	uint8_t								buttonY:			1;					//!< Bit indicating status of button->y.
	uint8_t								buttonB:			1;					//!< Bit indicating status of button->b.
	uint8_t								buttonZLeft:		1;					//!< Bit indicating status of button->z-left.
} WiiClassic_StatusNormal;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines bitfield interaction for status field queries from classic controllers.
//!	
//!	@note			The definitions below are for use when connected to a classic controller in 
//!					pass-through mode (e.g. when using Wii Motion Plus + classic controller).
//!	
//!	@warning		The following is in little-endian format.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiClassic_StatusPassThrough
{
	// Byte-01:
	uint8_t								dpadUp:				1;					//!< Bit indicating status of d-pad->up.
	uint8_t								analogLeftX:		5;					//!< Bits <5:1> for the left analog joystic along the x-axis.
	uint8_t								analogRightXHigh:	2;					//!< Bits <4:3> for the right analog joystic along the x-axis.
	// Byte-02:
	uint8_t								dpadLeft:			1;					//!< Bit indicating status of d-pad->left.
	uint8_t								analogLeftY:		5;					//!< Bits <5:1> for the left analog joystic along the y-axis.
	uint8_t								analogRightXMid:	2;					//!< Bits <2:1> for the right analog joystic along the x-axis.
	// Byte-03:
	uint8_t								analogRightY:		5;					//!< Bits <4:0> for the right analog joystic along the y-axis.
	uint8_t								leftTriggerHigh:	2;					//!< Bits <4:3> for the left trigger.
	uint8_t								analogRightXLow:	1;					//!< Bit  <0>   for the right analog joystick along the x-axis.
	// Byte-04:
	uint8_t								rightTrigger:		5;					//!< Bits <4:0> for the right trigger.
	uint8_t								leftTriggerLow:		3;					//!< Bits <2:0> for the left trigger.
	// Byte-05:
	uint8_t								extensionConnected:	1;					//!< Pass through bit used when passing through the Wii Motion Plus (should be high == 1).
	uint8_t								buttonTriggerRight:	1;					//!< Bit indicating status of button->-trigger-right.
	uint8_t								buttonPlus:			1;					//!< Bit indicating status of button->+.
	uint8_t								buttonHome:			1;					//!< Bit indicating status of button->home.
	uint8_t								buttonMinus:		1;					//!< Bit indicating status of button->-.
	uint8_t								buttonTriggerLeft:	1;					//!< Bit indicating status of button->-trigger-left.
	uint8_t								dpadDown:			1;					//!< Bit indicating status of d-pad->down.
	uint8_t								dpadRight:			1;					//!< Bit indicating status of d-pad->right.
	// Byte-06:
	uint8_t								RESERVED:			2;					//!< Rserved bits (should be == 0b00).
	uint8_t								buttonZRight:		1;					//!< Bit indicating status of button->z-right.
	uint8_t								buttonX:			1;					//!< Bit indicating status of button->x.
	uint8_t								buttonA:			1;					//!< Bit indicating status of button->a.
	uint8_t								buttonY:			1;					//!< Bit indicating status of button->y.
	uint8_t								buttonB:			1;					//!< Bit indicating status of button->b.
	uint8_t								buttonZLeft:		1;					//!< Bit indicating status of button->z-left.
} WiiClassic_StatusPassThrough;




//==================================================================================================
//	PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
WII_LIB_RC		WiiClassic_ProcessStatusParam(	WiiLib_Device *device	);


#endif	// __WII_CLASSIC_CONTROLLER__
