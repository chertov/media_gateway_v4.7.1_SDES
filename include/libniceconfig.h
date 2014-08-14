/*============================================================================
 *Name        : libniceconfig.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : define the configuration of libnice library
 *============================================================================
 */
#ifndef COMMON_H_
#define COMMON_H_
//#define NICE_COMPATIBILITY NICE_COMPATIBILITY_GOOGLE
#define NICE_COMPATIBILITY NICE_COMPATIBILITY_RFC5245

#define USE_RELIABLE 0

#define USE_TURN 0
#if USE_TURN
	#define TURN_IP "xxx"
	#define TURN_PORT 443
	#define TURN_USER "xxx"
	#define TURN_PASS ""
#endif

#define BUFFER_SIZE 1024

#define USE_STUN 1
#if USE_STUN
const char *STUN_SERVER = "stun.l.google.com";
const char *STUN_SERVER_PORT = "19302";
#endif

#endif /*COMMON_H_*/
