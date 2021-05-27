/**
 * \file
 *
 * \brief Board configuration.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/** Enable Com Port. */
#define CONF_BOARD_UART_CONSOLE

/** Enable SD MMC interface pins through SPI */
#define CONF_BOARD_SD_MMC_SPI

/** SPI slave select MACRO definition */
#define CONF_BOARD_SPI_NPCS0

/** FLEXCOM base address for SPI mode*/
#define BOARD_FLEXCOM_SPI    FLEXCOM5

//#define CONF_BOARD_USB_PORT
//#define CONF_BOARD_USB_VBUS_DETECT

#define DEBUG_CONSOLE

#ifdef DEBUG_CONSOLE
//0: USB CDC; 1: UART
#define INTERFACE_USB_CDC		1
#define INTERFACE_UART			2
#define DEBUG_CONSOLE_INTERFACE		INTERFACE_UART//INTERFACE_UART//INTERFACE_USB_CDC
#if DEBUG_CONSOLE_INTERFACE == INTERFACE_USB_CDC
//#define WAIT_CDC_OPEN
#endif
#endif

#define CONSOLE_UART_TXD_PIN			PIO_PA28_IDX//PIO_PA10_IDX//
#define CONSOLE_UART_TXD_FLAG			(PIO_PERIPH_B | PIO_DEFAULT)//(PIO_PERIPH_A | PIO_DEFAULT)
#define CONSOLE_UART_RXD_PIN			PIO_PA27_IDX//PIO_PA9_IDX//
#define CONSOLE_UART_RXD_FLAG			(PIO_PERIPH_B | PIO_DEFAULT)//(PIO_PERIPH_A | PIO_DEFAULT)//
#define CONSOLE_UART_PIO				PIOA

/** UART Interface */
#define CONSOLE_UART					USART7//USART0//
#define CONSOLE_UART_ID					ID_FLEXCOM7//ID_FLEXCOM0//ID_USART0//

#define CONSOLE_UART_IRQN				FLEXCOM7_IRQn//USART0_IRQn
#define CONSOLE_UART_HANDLER			FLEXCOM7_Handler

/** Baudrate setting */
#define CONSOLE_UART_BAUDRATE			921600//(115200UL)
/** Character length setting */
#define CONSOLE_UART_CHAR_LENGTH		US_MR_CHRL_8_BIT
/** Parity setting */
#define CONSOLE_UART_PARITY				US_MR_PAR_NO
/** Stop bits setting */
#define CONSOLE_UART_STOP_BITS			US_MR_NBSTOP_1_BIT

//If defined this, it will change the function 'udp_set_csr(ep, bits)' to Daniel's version
#define DANIEL_MODIFY_USB_ASF

#endif /* CONF_BOARD_H_INCLUDED */
