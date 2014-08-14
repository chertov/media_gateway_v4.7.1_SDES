/*
 * imsagent.h
 *
 *  Created on: May 12, 2014
 *      Author: sy
 */

#ifndef IMSAGENT_H_
#define IMSAGENT_H_

#include <string>
#include "helper.h"
#include "commontype.h"

class CodecConverter;
class WebAgent;
class MediaSession;

class ImsAgent{
public:
	ImsAgent(CodecConverter* c);

	~ImsAgent();

	void run();

//	type 0 for audio data, 1 for video data
	void sendData(unsigned char* data, int len, bool type);

	void setPeerAgent(WebAgent *_agent){this->peerAgent = _agent;}

	bool handleSdpFromIMS(const std::string &, webrtc::SessionType &);
	std::string createSdpToIMS(webrtc::SessionType);
	bool gotPeriodData(){
		bool ret = curcnt != precnt;
		if(ret){
			precnt = curcnt;
		}
		return ret;
	}

private:
	static void* cbFuncVideo(void*);//call back function for video
	static void* cbFuncAudio(void*);//call back function for audio

	void getRemoteIpAndPort(const std::string&, std::string &, unsigned short &, unsigned short &);

	Helper *videoHelper;
	Helper *audioHelper;

//	SSRC may be useless, here SSRC means SSRC to be sent to IMS, not SSRC from IMS
	std::string audioSSRC;
	std::string videoSSRC;

//	converter is the associate CodecConverter of this ImsAgent
	CodecConverter *converter;

//	peer agent of ImsAgent
	WebAgent *peerAgent;

	int aPayload;
	int vPayload;

	int curcnt;
	int precnt;
};


#endif /* IMSAGENT_H_ */
