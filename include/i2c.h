////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@file
//!	
//!	@brief			Defines public constants, macros, and constant functions used to interact with 
//!					an external I2C bus.
//!	
//!	@note			For references to functions, macros, and constants provided by Microchip, please 
//!					see the library file:
//!						-	[mc32-install]\pic32-libs\include\peripheral\i2c.h
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __I2C__
#define	__I2C__


//==================================================================================================
// INCLUDES
//--------------------------------------------------------------------------------------------------
#include <plib.h>
#include <stdint.h>




//==================================================================================================
// CONSTANTS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of return code values.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_RC
{
	I2C_RC_SUCCESS					= 0,										//!< Successfully completed task(s).
	I2C_RC_START_FAILED				= 1,										//!< Failed to set start condition (as I2C master).
	I2C_RC_RESTART_FAILED			= 2											//!< Failed to send restart message (as I2C master).

} I2C_RC;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of available clock rates supported by Wii devices.
//!	
//!	@note			This is not a list of all I2C supported rates. Rather, this is a list of known 
//!					clock speeds used by this library.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_CLOCK_RATE
{
	I2C_CLOCK_RATE_STANDARD				= 1000000,								//!< Standard clock rate used for (low speed) I2C communication over a I2C bus.
	I2C_CLOCK_RATE_FAST					= 4000000								//!< Fastest [supported] clock rate used for I2C communication over a I2C bus.
} I2C_CLOCK_RATE;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of I2C bus modes.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_MODE
{
	I2C_MODE_MASTER						= 1,									//!< I2C device is a master controlling the communication.
	I2C_MODE_SLAVE						= 2										//!< I2C device is a slave controlling the communication.
} I2C_MODE;




//==================================================================================================
// TYPEDEFS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Defines how this device will interact with an I2C port.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _I2C_Port
{
	I2C_CONFIGURATION		config;												//!< Configuration flags for port (e.g. stop in idle).
	I2C_MODULE				module;												//!< I2C module as defined by core MCP library (I2C1, I2C2, ...).
	uint32_t				clkFreq;											//!< Clock frequency to use when communicating with the bus (as a master).
	I2C_MODE				mode;												//!< Mode for I2C device (master, slave, etc.).
} I2C_Port;




//==================================================================================================
// PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
I2C_RC		I2C_InitPort(I2C_Port *port, uint32_t pbClk);


#endif	// __I2C__
