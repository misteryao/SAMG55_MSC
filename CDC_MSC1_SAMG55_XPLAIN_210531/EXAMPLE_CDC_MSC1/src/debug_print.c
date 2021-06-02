/*
 * debug_print.c
 *
 * Created: 2018/7/26 下午 05:44:33
 *  Author: Daniel
 */ 
#include <asf.h>
#include "debug_print.h"
#include "conf_debug_print.h"

static uint8_t cdcRxBuffer[CDC_READ_BUFFER_SIZE];
static uint16_t cdcRxBufferIndex = 0;
static uint16_t cdcRxReciveIndex = 0;
static uint16_t cdcRxData;

//static struct usart_module cdc_uart_module;

//static void cdc_read_callback(struct usart_module *const usart_module);
static void cdc_read_callback(void);

void configure_console(void)
{
	#if (DEBUG_CONSOLE_INTERFACE == INTERFACE_USB_CDC)
	stdio_usb_init();
	//stdio_usb_enable();
	#elif (DEBUG_CONSOLE_INTERFACE == INTERFACE_UART)
	const usart_serial_options_t uart_serial_options = {
		.baudrate =		CONSOLE_UART_BAUDRATE,
		.charlength =	CONSOLE_UART_CHAR_LENGTH,
		.paritytype =	CONSOLE_UART_PARITY,
		.stopbits =		CONSOLE_UART_STOP_BITS,
	};
	
	pio_configure_pin(CONSOLE_UART_TXD_PIN, CONSOLE_UART_TXD_FLAG);
	pio_configure_pin(CONSOLE_UART_RXD_PIN, CONSOLE_UART_RXD_FLAG);

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);
	
		#if (USE_SCANF != true)
	usart_enable_interrupt(CONSOLE_UART, US_IER_RXRDY);
	NVIC_EnableIRQ(CONSOLE_UART_IRQN);
		#endif
	#endif
}

void debug_buffer_print(uint8_t *p_buffer, uint16_t length, uint8_t line_bytes, enum print_separate prt_sp, bool prt_0x)
{
	uint16_t i = 0;
	for(i = 0; i < length; i ++)
	{
		if(i % line_bytes == 0)
		{
			DBG_MSG_PRINT("\r\n");
		}
		switch(prt_sp)
		{
			default:
			case PRT_SEPARATE_SPACE:
				if(prt_0x == true)
				{
					DBG_MSG_PRINT("0x%02X ", p_buffer[i]);
				}
				else
				{
					DBG_MSG_PRINT("%02X ", p_buffer[i]);
				}
			break;
			
			case PRT_SEPARATE_COMMA:
				if(prt_0x == true)
				{
					DBG_MSG_PRINT("0x%02X, ", p_buffer[i]);
				}
				else
				{
					DBG_MSG_PRINT("%02X, ", p_buffer[i]);
				}
			break;
		}	
	}
}

void debug_write_raw_data(uint16_t data)
{
	usart_write(CONSOLE_UART, data);
}

bool cdc_rxdata_receive(uint8_t *p_data)
{
	if(cdcRxBufferIndex != cdcRxReciveIndex)
	{
		*p_data = cdcRxBuffer[cdcRxReciveIndex];
		if((++ cdcRxReciveIndex) >= sizeof(cdcRxBuffer))
		{
			cdcRxReciveIndex = 0;
		}
		return true;
	}
	return false;
}

static void cdc_read_callback(void)
{
	cdcRxBuffer[cdcRxBufferIndex] = cdcRxData;
	if((++ cdcRxBufferIndex) >= sizeof(cdcRxBuffer))
	{
		cdcRxBufferIndex = 0;
	}
}

void CONSOLE_UART_HANDLER(void)
{
	/* Introducing critical section to avoid buffer corruption. */
	cpu_irq_disable();
	usart_serial_read_packet(CONSOLE_UART, &cdcRxData, 1);
	cdc_read_callback();
	cpu_irq_enable();
}