/*============================================================================
 *Name        : commontype.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : this file define the common type used in project
 *============================================================================
 */
#ifndef COMMONTYPE_H_
#define COMMONTYPE_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>


enum RoapType{offer = 1, answer, ok, candidate, shutdown_r, error_r, message};
enum ErrorType{none = -1, offline = 1, timeout, nomatch, refused, conflict, doubleconflict, mediafailed, failed};

typedef struct RoapMessage{
	RoapMessage():seq(-1), label(-1), error(none), msgSize(-1){}
	RoapType type;
	int seq;
	int label;
	ErrorType error;
	std::string offerSessionID;
	std::string answerSessionID;
	std::string sdp;
	std::string tiebreaker;
	bool moreComingFlag;
	int msgSize;
	std::string msgContent;
}RoapMessage;

typedef struct SessionMessage{
	int type;
	std::string from;
	std::string to;
	RoapMessage roap;
}SessionMessage;

namespace webrtc{
	const int PT_H263_DEFAULT = 102;
	const int PT_H264_DEFAULT = 98;
	const int PT_VP8_DEFAULT = 112;
	const int PT_PCMA_DEFAULT = 8;

	//sdp associated field from gateway to web
	const static std::string WEB_SDP_BEGIN = "v=0\r\no=- 3407814663 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\n";
	const static std::string WEB_AUDIO_BUNDLE = "a=group:BUNDLE audio\r\n";
	const static std::string WEB_VIDEO_BUNDLE = "a=group:BUNDLE audio video\r\n";

	const static char *ICE_UFRAG = "a=ice-ufrag:";
	const static char *ICE_PWD = "a=ice-pwd:";
	const static char *ICE_OPTIONS = "a=ice-options";

	const static char *PARAM_END = "\r\n";

	const static char *CRYPTO_80 = "a=crypto:1 AES_CM_128_HMAC_SHA1_80 inline:";

	//add by guoxun
	//in dtls
	const static char *FINGERPRINT = "a=fingerprint:";
	const static char *SETUP = "a=setup:";

	const static char *CANDIDATE = "a=candidate:";
	const static char *SSRC = "a=ssrc:";
	const static char *SSRC_END = " cname:";

	enum SessionType{AUDIO, VIDEO};
	enum ActRole{WEBDTLSCLIENT,WEBDTLSSERVER,WEBCORS};

	static std::string intToString(int n){
		char buf[20];
		sprintf(buf, "%d", n);
		return (std::string)buf;
	}
}

#endif /*COMMONTYPE_H_*/
