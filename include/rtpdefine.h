/*============================================================================
 *Name        : rtpdefine.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : this file define the struct related with rtp
 *============================================================================
 */
#ifndef RTPDEFINE_H_
#define RTPDEFINE_H_

typedef struct
{
	/** byte 0 */
	unsigned char csrc_len:4;        /** expect 0 */
	unsigned char extension:1;       /** expect 1, see RTP_OP below */
	unsigned char padding:1;         /** expect 0 */
	unsigned char version:2;         /** expect 2 */
	/** byte 1 */
	unsigned char payload:7;         /** stream type */
	unsigned char marker:1;          /** when send the first framer,set it */
	/** bytes 2, 3 */
	unsigned short seq_no;
	/** bytes 4-7 */
	unsigned  long timestamp;
	/** bytes 8-11 */
	unsigned long ssrc;              /** stream number is used here. */
} RTP_HEADER;

typedef struct
{
	/** byte 0 */
	unsigned char id;        /** dtmf */

	/** byte 1 */
	unsigned char volume:6;
	unsigned char reserved:1;          /** reserved */
	unsigned char endflag:1;         /** end of event */

	unsigned short duration;

} RTP_EVENT_S;

//stun
typedef struct
{
	/** bytes 1, 2*/
	unsigned short MessageType;//0x00 0x01
	/** bytes 3, 4*/
	unsigned short MessageLength;
	/** bytes 5-8*/
	unsigned  long MessageCookie;
	/** bytes 9-20 */
	unsigned  char MessageTID[12];

} STUN_HEADER;

typedef struct
{
	/** bytes 1, 2*/
	unsigned short ArrType;//0x00 0x06
	/** bytes 3, 4*/
	unsigned short MessageLength;
	unsigned  char Message[32];   //Adler Editor 64 -> 32

} STUN_ARR;

typedef struct
{
	/** bytes 1, 2*/
	unsigned short ArrType;//0x00 0x001
	/** bytes 3, 4*/
	unsigned short MessageLength;
	unsigned char reserved;
	unsigned char ipv4;
	unsigned short port;
	unsigned char ip[4];
} STUN_ADDR_MAP;

#define SRTP_LEN 182
#define RTP_LEN 172
//#define RTP_HEADER_LEN = 12;

#endif /*RTPDEFINE_H_*/
