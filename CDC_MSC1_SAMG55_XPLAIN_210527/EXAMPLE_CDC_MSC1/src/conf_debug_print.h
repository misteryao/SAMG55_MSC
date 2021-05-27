/*
 * conf_debug_print.h
 *
 * Created: 2018/7/26 下午 05:46:47
 *  Author: Daniel
 */ 


#ifndef CONF_DEBUG_PRINT_H_
#define CONF_DEBUG_PRINT_H_

/** Define to \c true to enable rich formatted debug output (colors, etc.) */
#define DEBUG_ENABLE_FORMATTING        true

/* for debug text */
#ifdef DEBUG_CONSOLE
#define DBG_MSG_PRINT	printf
#define USE_SCANF		false
	#if (USE_SCANF == true)
#define DBG_SCANF		scanf
	#endif
#else
#define DBG_MSG_PRINT	//NULL(...)
#endif

#define DBG_BUFFER_PRINT(ptr, length, line_bytes, prt_sp, prt_0x)	debug_buffer_print(ptr, length, line_bytes, prt_sp, prt_0x)

#if DEBUG_ENABLE_FORMATTING == true
#  define DEBUG_ANSI(format)           ANSI_ESC_##format
#else
/** Wrapper for the standard ANSI formatting codes, if enabled via the
 *  \c DEBUG_ENABLE_FORMATTING configuration token. When enabled, use
 *  the \c ANSI_ESC_* defines in \c <ansi.h> with the \c ANSI_ESC_ prefix
 *  removed (e.g. \c DEBUG_ANSI(FG_RED)).
 */
#  define DEBUG_ANSI(format)
#endif

#endif /* CONF_DEBUG_PRINT_H_ */