/**
 * \file
 *
 * \brief Declaration of main function used by Composite example 4
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include "usb_protocol_cdc.h"

#define APP_FW_VERSION			"CDC_MSC1_SAMG55_XPLAIN_210527"

#define STRING_EOL				"\r\n"
#define STRING_HEADER			"-- SAMG55 USB Mass Storage Test --"STRING_EOL \
								"-- "BOARD_NAME " --"STRING_EOL	\
								"-- "APP_FW_VERSION " --"STRING_EOL	\
								"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL


/*! \brief Called by MSC interface
 * Callback running when USB Host enable MSC interface
 *
 * \retval true if MSC startup is ok
 */
bool main_msc_enable(void);

/*! \brief Called by MSC interface
 * Callback running when USB Host disable MSC interface
 */
void main_msc_disable(void);

/*! \brief Opens the communication port
 * This is called by CDC interface when USB Host enable it.
 *
 * \retval true if cdc startup is successfully done
 */
bool main_cdc_enable(uint8_t port);

/*! \brief Closes the communication port
 * This is called by CDC interface when USB Host disable it.
 */
void main_cdc_disable(uint8_t port);

/*! \brief Save new DTR state to change led behavior.
 * The DTR notify that the terminal have open or close the communication port.
 */
void main_cdc_set_dtr(uint8_t port, bool b_enable);

/*! \brief Manages the leds behaviors
 * Called when a start of frame is received on USB line each 1ms.
 */
void main_sof_action(void);

/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void main_suspend_action(void);

/*! \brief Turn on a led to notify active mode
 * Called when the USB line is resumed from the suspend state
 */
void main_resume_action(void);

/*! \brief Called by UDC when USB Host request a extra string different
 * of this specified in USB device descriptor
 */
bool main_extra_string(void);

/*! \brief Initialize the memories used by examples
 */
void memories_initialization(void);

#endif // _MAIN_H_
