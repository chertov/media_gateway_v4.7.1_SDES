/*============================================================================
 *Name        : rtpconverter.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : the RTPConverter class can convert media data between srtp and rtp
 *============================================================================
 */
#ifndef CONVERT_H_
#define CONVERT_H_

#include <signal.h>
#include <iostream>
#include <string>
#include "libsrtp/rtp.h"
#include "libsrtp/srtp.h"
#include "libsrtp/rtp_priv.h"
#include "libsrtp/datatypes.h"
#include "libsrtp/getopt_s.h"
#include "base64.h"

#define MASTER_KEY_LEN 30
#define SRTP_LEN 182
#define RTP_LEN 172
#define SRTP_VP8_LEN 2048

class RTPConverter{
	public :
		RTPConverter();

		~RTPConverter();

		/*
		 * @Func:  initialize the srtp session
		 */
		int init(std::string enkey, std::string dekey);

		/*
		 * @Func:  encrypt RTP stream from IMS into SRTP stream
		 */
		int encryptData(unsigned char *buffer, int &len);

		/*
		 * @Func:  decrypt SRTP stream from WEB into RTP stream
		 */
		int decryptData(unsigned char *buffer, int &len);

		bool isReady(){return ready;}

		int encryptRTCP(unsigned char *, int&);

	private:
		srtp_t sessionDecrypt;
		srtp_t sessionEncrypt;


		srtp_policy_t policyDecrypt;
		srtp_policy_t policyEncrypt;
		unsigned char masterKey[MASTER_KEY_LEN];
		unsigned char masen[MASTER_KEY_LEN];
		bool ready;
};

#endif /*CONVERT_H_*/
