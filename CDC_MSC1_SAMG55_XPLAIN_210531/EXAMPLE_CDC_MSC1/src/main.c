/**
 * \file
 *
 * \brief Main functions for USB composite example
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
#include <string.h>
#include <asf.h>
#include "debug_print.h"
#include "conf_debug_print.h"
#include "conf_usb.h"
#include "ui.h"
#include "uart.h"

static volatile bool main_b_msc_enable = false;
static volatile bool main_b_cdc_enable = false;

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	bool MSCEnableStatus = false;
	uint32_t MSCEnableCount = 0;
	
	char test_file_name[] = "0:sd_mmc_test.txt";
	Ctrl_status status;
	FRESULT res;
	FATFS fs;
	FIL file_object;
	
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();
#if !SAM0
	sysclk_init();
	board_init();
#else
	system_init();
#endif

	//Set PA17 as PCK1 output pin, peripheral B
	pio_configure_pin(PIO_PA17_IDX, (PIO_PERIPH_B | PIO_DEFAULT));
	
	pmc_enable_pck(1);
	pmc_pck_set_source(1, PMC_PCK_CSS_PLLB_CLK);
	
	configure_console();
	
	DBG_MSG_PRINT(STRING_HEADER);
	
	ui_init();
	ui_powerdown();
	
	memories_initialization();
	/* Initialize SD MMC stack */
	sd_mmc_init();
	MATRIX->CCFG_USBMR |= (1 << 4);
	
	/* Insert application code here, after the board has been initialized. */
	DBG_MSG_PRINT("Mount disk (f_mount)...\r\n");
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_VIRTUAL_MEM, &fs);
	if (FR_INVALID_DRIVE == res)
	{
		DBG_MSG_PRINT("[FAIL] res %d\r\n", res);
		goto main_end_of_test;
	}
	DBG_MSG_PRINT("[OK]\r\n");
	
	DBG_MSG_PRINT("Format the virtual disk\r\n");
	res = f_mkfs(LUN_ID_VIRTUAL_MEM, /* Drv */
					0, /* FDISK partition */
					512); /* AllocSize */
	if (res != FR_OK)
	{
		DBG_MSG_PRINT("[FAIL] res %d\r\n", res);
		goto main_end_of_test;
	}
	
	#if 1
	DBG_MSG_PRINT("Please plug an SD, MMC or SDIO card in slot.\n\r");
	/* Wait card present and ready */
	do
	{
		status = sd_mmc_test_unit_ready(0);
		if (CTRL_FAIL == status)
		{
			DBG_MSG_PRINT("Card install FAIL\n\r");
			DBG_MSG_PRINT("Please unplug and re-plug the card.\n\r");
			while (CTRL_NO_PRESENT != sd_mmc_check(0))
			{
			}
		}
	}while (CTRL_GOOD != status);
	DBG_MSG_PRINT("SD, MMC or SDIO card is in the slot\n\r");
	#endif
	
	DBG_MSG_PRINT("Mount disk (f_mount)...\r\n");
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
	if (FR_INVALID_DRIVE == res)
	{
		DBG_MSG_PRINT("[FAIL] res %d\r\n", res);
		goto main_end_of_test;
	}
	DBG_MSG_PRINT("[OK]\r\n");

	DBG_MSG_PRINT("Create a file (f_open)...\r\n");
	test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res = f_open(&file_object, (char const *)test_file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK)
	{
		DBG_MSG_PRINT("[FAIL] res %d\r\n", res);
		if(res == FR_NO_FILESYSTEM)
		{
			DBG_MSG_PRINT("No file system, format the disk\r\n");
			res = f_mkfs(LUN_ID_SD_MMC_0_MEM, /* Drv */
							0, /* FDISK partition */
							512); /* AllocSize */
			if (res != FR_OK)
			{
				DBG_MSG_PRINT("[FAIL] res %d\r\n", res);
				goto main_end_of_test;
			}
		}
		else
		{
			goto main_end_of_test;
		}
		
	}
	//DBG_MSG_PRINT("[OK]\r\n");

	//DBG_MSG_PRINT("Write to test file (f_puts)...\r\n");
	if (0 == f_puts("Test SD/MMC stack\n", &file_object))
	{
		f_close(&file_object);
		DBG_MSG_PRINT("[FAIL]\r\n");
		goto main_end_of_test;
	}
	DBG_MSG_PRINT("[OK]\r\n");
	f_close(&file_object);
	DBG_MSG_PRINT("Test is successful.\n\r");

	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	
	// Start USB stack to authorize VBus monitoring
	udc_start();
	
	while (true) {

		if (main_b_msc_enable) {
			if(MSCEnableStatus == false)
			{
				MSCEnableStatus = true;
				MSCEnableCount ++;
				DBG_MSG_PRINT("\r\nMSC Enabled! Count %d\r\n", MSCEnableCount);
			}
			if (!udi_msc_process_trans()) {
				//sleepmgr_enter_sleep();
			}
		}else{
			//sleepmgr_enter_sleep();
			if(MSCEnableStatus == true)
			{
				MSCEnableStatus = false;
				DBG_MSG_PRINT("\r\nMSC Disabled!\r\n");
			}
		}
	}
	
main_end_of_test:
	DBG_MSG_PRINT("Please unplug the card.\n\r");
	while (CTRL_NO_PRESENT != sd_mmc_check(0))
	{
	
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if ((!main_b_msc_enable) ||
		(!main_b_cdc_enable))
		return;
	ui_process(udd_get_frame_number());
}

/*! \brief Example of extra USB string management
 * This feature is available for single or composite device
 * which want implement additional USB string than
 * Manufacture, Product and serial number ID.
 *
 * return true, if the string ID requested is know and managed by this functions
 */
bool main_extra_string(void)
{
	static uint8_t udi_cdc_name[] = "CDC interface";
	static uint8_t udi_msc_name[] = "MSC interface";

	struct extra_strings_desc_t{
		usb_str_desc_t header;
		le16_t string[Max(sizeof(udi_cdc_name)-1, sizeof(udi_msc_name)-1)];
	};
	static UDC_DESC_STORAGE struct extra_strings_desc_t extra_strings_desc = {
		.header.bDescriptorType = USB_DT_STRING
	};

	uint8_t i;
	uint8_t *str;
	uint8_t str_lgt=0;

	// Link payload pointer to the string corresponding at request
	switch (udd_g_ctrlreq.req.wValue & 0xff) {
	case UDI_CDC_IAD_STRING_ID:
		str_lgt = sizeof(udi_cdc_name)-1;
		str = udi_cdc_name;
		break;
	case UDI_MSC_STRING_ID:
		str_lgt = sizeof(udi_msc_name)-1;
		str = udi_msc_name;
		break;
	default:
		return false;
	}

	if (str_lgt!=0) {
		for( i=0; i<str_lgt; i++) {
			extra_strings_desc.string[i] = cpu_to_le16((le16_t)str[i]);
		}
		extra_strings_desc.header.bLength = 2+ (str_lgt)*2;
		udd_g_ctrlreq.payload_size = extra_strings_desc.header.bLength;
		udd_g_ctrlreq.payload = (uint8_t *) &extra_strings_desc;
	}

	// if the string is larger than request length, then cut it
	if (udd_g_ctrlreq.payload_size > udd_g_ctrlreq.req.wLength) {
		udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
	}
	return true;
}

bool main_msc_enable(void)
{
	main_b_msc_enable = true;
	return true;
}

void main_msc_disable(void)
{
	main_b_msc_enable = false;
}

bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	// Open communication
	//uart_open(port);
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
	// Close communication
	//uart_close(port);
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
	if (b_enable) {
		// Host terminal has open COM
		ui_com_open(port);
	}else{
		// Host terminal has close COM
		ui_com_close(port);
	}
}

/**
 * \mainpage ASF USB Composite Device Example CDC and MSC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Composite Device with CDC and
 * Mass Storage interfaces on Atmel MCU with USB module.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as
 * digital telephones or analog modems, as well as networking devices
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example uses all memories available on the board and connects these to
 * USB Device Mass Storage stack.
 * Also, the example is a bridge between a USART from the main MCU
 * and the USB CDC interface.
 * After loading firmware, connect the board (EVKxx,Xplain,...) to the USB Host.
 * When connected to a USB host system this application allows to display
 * all available memories as a removable disks and provides a mouse, keyboard and CDC in
 * the Unix/Mac/Windows operating systems.
 *
 * In this example, we will use a PC as a USB host:
 * it connects to the USB and to the USART board connector.
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC)
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Device Manager).
 * - Open a HyperTerminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one HyperTerminal and it will echo in the other.
 *
 * \note
 * This example uses the native MSC driver on Unix/Mac/Windows OS, except for Win98.
 * About CDC, on the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - When requested by Windows for a driver INF file, select the
 *   atmel_devices_cdc.inf file in the directory indicated in the Atmel Studio
 *   "Solution Explorer" window.
 * - click "Next"
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC & MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/msc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - udi_composite_desc.c,udi_composite_conf.h,
 *      <br>USB Composite Device definition
 *    - uart_uc3.c,
 *      <br>implementation of RS232 bridge for AVR32 parts
 *    - uart_xmega.c,
 *      <br>implementation of RS232 bridge for XMEGA parts
 *    - uart_sam.c,
 *      <br>implementation of RS232 bridge for SAM parts
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 *
 * <SUP>1</SUP> The memory data transfers are done outside USB interrupt routine.
 * This is done in the MSC process ("udi_msc_process_trans()") called by main loop.
 */
