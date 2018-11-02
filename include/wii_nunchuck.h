////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants, macros, and constant functions available for the "wii 
//!					nunchuck" support.
//!	
//!	@note			This file is pulled into "wii_lib.h" automatically.
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WII_NUNCHUCK__
#define	__WII_NUNCHUCK__


//==================================================================================================
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include <stdint.h>
#include "wii_lib.h"




//==================================================================================================
//	TYPEDEFS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines bitfield interaction for status field queries from nunchuck controllers.
//!	
//!	@note			The definitions below are for use when directly connected to a nunchuck.
//!	
//!	@warning		The following is in little-endian format.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiNunchuck_StatusNormal
{
	// Byte-01:
	uint8_t								analogX:			8;					//!< Bits <7:0> for the analog joystic along the x-axis.
	// Byte-02:
	uint8_t								analogY:			8;					//!< Bits <7:0> for the analog joystic along the y-axis.
	// Byte-03:
	uint8_t								accelXHigh:			8;					//!< Bits <9:2> for the accelerometer along the x-axis.
	// Byte-04:
	uint8_t								accelYHigh:			8;					//!< Bits <9:2> for the accelerometer along the y-axis.
	// Byte-05:
	uint8_t								accelZHigh:			8;					//!< Bits <9:2> for the accelerometer along the z-axis.
	// Byte-06:
	uint8_t								buttonZ:			1;					//!< Bit indicating status of z button (pressed == low).
	uint8_t								buttonC:			1;					//!< Bit indicating status of c button (pressed == low).
	uint8_t								accelXLow:			2;					//!< Bits <1:0> for the accelerometer along the z-axis.
	uint8_t								accelYLow:			2;					//!< Bits <1:0> for the accelerometer along the y-axis.
	uint8_t								accelZLow:			2;					//!< Bits <1:0> for the accelerometer along the x-axis.
} WiiNunchuck_StatusNormal;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines bitfield interaction for status field queries from nunchuck controllers.
//!	
//!	@note			The definitions below are for use when connected to a nunchuck in pass-through 
//!					mode (e.g. when using Wii Motion Plus + nunchuck).
//!	
//!	@note			To accomidate the pass-through flag, the least significant bit of all 
//!					accelerometer values is dropped.
//!	
//!	@warning		The following is in little-endian format.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _WiiNunchuck_StatusPassThrough
{
	// Byte-01:
	uint8_t								analogX:			8;					//!< Bits <7:0> for the analog joystic along the x-axis.
	// Byte-02:
	uint8_t								analogY:			8;					//!< Bits <7:0> for the analog joystic along the y-axis.
	// Byte-03:
	uint8_t								accelXHigh:			8;					//!< Bits <9:2> for the accelerometer along the x-axis.
	// Byte-04:
	uint8_t								accelYHigh:			8;					//!< Bits <9:2> for the accelerometer along the y-axis.
	// Byte-05:
	uint8_t								extensionConnected:	1;					//!< Bit indicating if extension is connected (1 == active).
	uint8_t								accelZHigh:			7;					//!< Bits <9:3> for the accelerometer along the z-axis.
	// Byte-06:
	uint8_t								RESERVED:			2;					//!< Rserved bits (should be == 0b00).
	uint8_t								buttonZ:			1;					//!< Bit indicating status of z button (pressed == low).
	uint8_t								buttonC:			1;					//!< Bit indicating status of c button (pressed == low).
	uint8_t								accelXLow:			1;					//!< Bit <1> for the accelerometer along the z-axis.
	uint8_t								accelYLow:			1;					//!< Bit <1> for the accelerometer along the y-axis.
	uint8_t								accelZLow:			2;					//!< Bits <2:1> for the accelerometer along the x-axis.
} WiiNunchuck_StatusPassThrough;




//==================================================================================================
//	PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
WII_LIB_RC		WiiNunchuck_ProcessStatusParam(	WiiLib_Device *device	);


#endif	// __WII_NUNCHUCK__
