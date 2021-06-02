/*
 * debug_print.h
 *
 * Created: 2018/7/26 下午 05:45:18
 *  Author: Daniel
 */ 


#ifndef DEBUG_PRINT_H_
#define DEBUG_PRINT_H_

#define CDC_READ_BUFFER_SIZE		1024

enum print_separate
{
	PRT_SEPARATE_SPACE = 0,
	PRT_SEPARATE_COMMA	
};

void configure_console(void);
void debug_buffer_print(uint8_t *p_buffer, uint16_t length, uint8_t line_bytes, enum print_separate pr_sp, bool prt_0x);
void debug_write_raw_data(uint16_t data);
bool cdc_rxdata_receive(uint8_t *p_data);

#endif /* DEBUG_PRINT_H_ */