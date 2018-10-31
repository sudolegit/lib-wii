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
//	INCLUDES
//--------------------------------------------------------------------------------------------------
#include <plib.h>
#include <stdint.h>




//==================================================================================================
//	CONSTANTS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of return code values.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_RC
{
	I2C_RC_SUCCESS						= 0,									//!< Successfully completed task(s).
	I2C_RC_START_FAILED					= 1,									//!< Failed to set start condition (as I2C master).
	I2C_RC_RESTART_FAILED				= 2,									//!< Failed to send restart message (as I2C master).
	I2C_RC_SEND_BYTE_BUFFER_FAILED		= 3,									//!< Failed to buffer a byte for transmission over I2C bus.
	I2C_RC_NO_ACK						= 4,									//!< Data transmitted over bus but no 1.
	I2C_RC_RECEIVE_OVERFLOW				= 5										//!< Unable to start receiving data due to buffer overflow.
} I2C_RC;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of available clock rates supported by Wii devices.
//!	
//!	@note			This is not a list of all I2C supported rates. Rather, this is a list of known 
//!					clock speeds used by this library.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_CLOCK_RATE
{
	I2C_CLOCK_RATE_STANDARD				= 100000,								//!< Standard clock rate used for (low speed) I2C communication over a I2C bus.
	I2C_CLOCK_RATE_FAST					= 400000								//!< Fastest [supported] clock rate used for I2C communication over a I2C bus.
} I2C_CLOCK_RATE;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Enum of I2C bus modes.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_MODE
{
	I2C_MODE_MASTER						= 1,									//!< I2C device is a master controlling the communication.
	I2C_MODE_SLAVE						= 2										//!< I2C device is a slave controlling the communication.
} I2C_MODE;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Acknowledgement methods used when receiving data from devices over I2C.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_ACK_MODE
{
	I2C_ACK_MODE_NACK					= 0,									//!< Acknowledge data received with a low-bit [0].
	I2C_ACK_MODE_ACK					= 1										//!< Acknowledge data received with a high-bit [1].
} I2C_ACK_MODE;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Number of bits (length) of address format for target device.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _I2C_ADDR_LEN
{
	I2C_ADDR_LEN_7_BITS					= 7,									//!< Target device address is 7-bits.
	I2C_ADDR_LEN_10_BITS				= 10									//!< Target device address is 10-bits.
} I2C_ADDR_LEN;


// The following delays are executed prior to raising the stop conditoin on the bus.
#define	I2C_DELAY_POST_SEND_MS			0										//!< Delay in milliseconds after trasnmitting a payload across the I2C bus.
#define	I2C_DELAY_POST_READ_MS			10										//!< Delay in milliseconds after reading a payload from the I2C bus.
#define	I2C_DELAY_BETWEEN_TX_RX_MS		1										//!< Delay in milliseconds between sending a TX request and starting the following RX reqeust to read the reply.




//==================================================================================================
//	TYPEDEFS
//--------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Tracks I2C port settings (e.g. which module and clock speed for communication).
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _I2C_Port
{
	I2C_CONFIGURATION					config;									//!< Configuration flags for port (e.g. stop in idle).
	I2C_MODULE							module;									//!< I2C module as defined by core MCP library (I2C1, I2C2, ...).
	uint32_t							clkFreq;								//!< Clock frequency to use when communicating with the bus (as a master).
	I2C_ACK_MODE						ackMode;								//!< Mode to use when acknowledging received data (high vs low ack).
} I2C_Port;


////////////////////////////////////////////////////////////////////////////////////////////////////
//!	@brief			Tracks the device-specific I2C information.
//!	
//!	@note			Also encapsulates the 'I2C_Port{}' information for easier tracking.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _I2C_Device
{
	I2C_Port							port;									//!< Structure tracking I2C port specific details.
	I2C_MODE							mode;									//!< Mode for I2C device (master, slave, etc.).
	uint16_t							addr;									//!< Identifier for device (e.g. 0x50).
	I2C_ADDR_LEN						addrLength;								//!< Length of I2C address format (e.g. 7-bits).
} I2C_Device;




//==================================================================================================
//	PUBLIC FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------------------------
I2C_RC		I2C_InitPort(		I2C_Port *port,		uint32_t pbClk																								);
I2C_RC		I2C_Transmit(		I2C_Device *device,	uint8_t *data,		uint32_t len,	BOOL ackRequired														);
I2C_RC		I2C_Receive(		I2C_Device *device,	uint8_t *data,		uint32_t len,	BOOL ackMessages														);
I2C_RC		I2C_TxRx(			I2C_Device *device,	uint8_t *dataTx,	uint32_t lenTx,	uint8_t *dataRx,	uint32_t lenRx,	BOOL ack,	BOOL useRepeatedStart 	);


#endif	// __I2C__
