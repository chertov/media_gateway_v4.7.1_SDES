/*
 * debug.h
 *
 *  Created on: Apr 8, 2014
 *      Author: guoxun
 */

#ifndef DEBUG_H_
#define DEBUG_H_


#include <glib.h>
#include <glib/gprintf.h>




#define dtls_print g_print
#define dtls_debug(...) \
	do { \
		g_print("[%s:%s:%d:] ", __FILE__, __FUNCTION__, __LINE__); \
		g_print(__VA_ARGS__); \
	} while(0);




#endif /* DEBUG_H_ */
