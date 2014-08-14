/*
 * imsagent.cpp
 *
 *  Created on: May 13, 2014
 *      Author: sy
 */

#include "imsagent.h"

#include <string>

#include "helper.h"
#include "codecconverter.h"
#include "commontype.h"
#include "webagent.h"
#include "mgdebug.h"
#include <map>

using namespace std;
using namespace webrtc;

extern map<string, int> SIPCodec;

ImsAgent::ImsAgent(CodecConverter* c){
	precnt = 0;
	curcnt = 0;

	videoHelper = new Helper();
	audioHelper = new Helper();
	audioSSRC = "777777777";
	videoSSRC = "888888888";
	this->aPayload = webrtc::PT_PCMA_DEFAULT;

	if(SIPCodec["H264"]){
		this->vPayload = webrtc::PT_H264_DEFAULT;
	}else if(SIPCodec["H263"]){
		this->vPayload = webrtc::PT_H263_DEFAULT;
	}
	converter = c;
	peerAgent = NULL;
}

ImsAgent::~ImsAgent(){
	cout << "In delete IMsagent.." << endl;
	delete videoHelper;
	delete audioHelper;
	cout << "IMSAgent deleted." << endl;
}

void ImsAgent::run(){
	this->videoHelper->run(cbFuncVideo, this);
	this->audioHelper->run(cbFuncAudio, this);
}

//type 0 for audio data, 1 for video data
void ImsAgent::sendData(unsigned char* mediaData, int len, bool type){

	RTP_HEADER *header = (RTP_HEADER *)&mediaData[0];

	if(type == 0){//audio data
		MG_DEBUG("Audio Data: Sending %d bytes to IMS.", len);
		header->ssrc = htonl(atoi(this->audioSSRC.c_str()));
		header->payload = this->aPayload;

		audioHelper->sendData(mediaData, len);
	}else{//video data
		MG_DEBUG("Video Data: Sending %d bytes to IMS.", len);
		header->ssrc = htonl(atoi(this->videoSSRC.c_str()));
		header->payload = this->vPayload;

		videoHelper->sendData(mediaData, len);
	}
}

string ImsAgent::createSdpToIMS(SessionType stype){
	string aStr = intToString(this->aPayload);
	string vStr = intToString(this->vPayload);
	string videoPort = intToString(videoHelper->getListenPort());
	string audioPort = intToString(audioHelper->getListenPort());


	string imsSDP = (string)"v=0\r\n" +
							"o=mg 1811991880 0 IN IP4 10.109.247.140\r\n" +
							"s=abc\r\n" +
							"c=IN IP4 10.109.247.140\r\n" +
							"t=0 0\r\n" +
							"m=audio " + audioPort + " RTP/AVP " + aStr + "\r\n" +
							"a=rtpmap:" + aStr + " PCMA/8000/1\r\n" +
							"a=sendrecv\r\n";

	if(stype == VIDEO){

		string videoSDP;
		if(SIPCodec["H264"]){
			videoSDP = 	(string)"m=video " + videoPort + " RTP/AVP " + vStr + "\r\n" +
								"a=rtpmap:" + vStr + " H264/90000\r\n" +
//								"a=fmtp:" + PT_H263_STRING + " profile-level-id=4DE01f; packetization-mode=1\r\n" +
								"a=fmtp:" + vStr + " profile-level-id=42E00D; max-br=320\r\n" +
//								"a=immageattr:" + PT_H263_STRING + " send * recv [x=[0-1364],y=[0-768]]\r\n" +
								"a=sendrecv\r\n";
		}else if(SIPCodec["H263"]){
			videoSDP = 	(string)"m=video " + videoPort + " RTP/AVP " + vStr + "\r\n" +
								"a=rtpmap:" + vStr + " H263-1998/90000\r\n" +
								"a=fmtp:" + vStr + " CIF=2;QCIF=2;SQCIF=2\r\n" +
								"a=sendrecv\r\n";
		}else if(SIPCodec["H263P"]){
			videoSDP  = (string)"m=video " + videoPort + " RTP/AVP " + vStr + "\r\n" +
								"a=rtpmap:" + vStr + " H263-1998/90000\r\n" +
								"a=fmtp:" + vStr + " QCIF=2;CIF=2;VGA=2;CIF4=2;I=1;J=1;T=1\r\n" +
								"a=sendrecv\r\n";
		}
		imsSDP += videoSDP;
	}

	return imsSDP;
}

#include <sstream>

bool ImsAgent::handleSdpFromIMS(const std::string & sdp, webrtc::SessionType &stype){
	string ip;
	unsigned short aport, vport;

	getRemoteIpAndPort(sdp, ip, aport, vport);
	videoHelper->setDestination(ip, vport);
	audioHelper->setDestination(ip, aport);

	stype = AUDIO;


	if(sdp.find("video") != string::npos){
		stype = VIDEO;
	}

	string::size_type beginPos = 0, endPos, p2;


	bool supportFlag = 1;

	beginPos = sdp.find("m=audio ", beginPos);
	if(beginPos == string::npos){
		cout << "MY god!" << endl;
		supportFlag = 0;
	}else{
		beginPos += strlen("m=audio ");
		//find recv port
		endPos = sdp.find(" ", beginPos);

		beginPos = endPos + 1;

		//find RTP/AVP
		endPos = sdp.find(" ", beginPos);
		beginPos = endPos + 1;

		endPos = sdp.find("\n", beginPos);
		cout << "sy1" << endl;
		while(beginPos < endPos){
			p2 = sdp.find(" ", beginPos);

			if(p2 == string::npos || p2 > endPos){
				p2 = sdp.find("\r\n", beginPos);
				if(sdp.substr(beginPos, p2-beginPos) != "8")
					supportFlag = 0;
				cout << "sy2" << endl;
				break;
			}

			cout << "sy3: " << sdp.substr(beginPos, p2-beginPos) << endl;
			if(sdp.substr(beginPos, p2-beginPos) == "8"){
				break;
			}
			beginPos = p2 + 1;
		}

	}

	cout << "CP1: flag = " << supportFlag << endl;


	if(SIPCodec["H264"]){
		if(stype == VIDEO){
			beginPos = 0;
			do{
				beginPos = sdp.find("a=rtpmap:", beginPos);
				if(beginPos == string::npos)break;
				beginPos += strlen("a=rtpmap:");
				endPos = sdp.find(" ", beginPos);
				string vStr = sdp.substr(beginPos, endPos-beginPos);
				if(sdp.substr(endPos+1, 4) == "H264"){
					this->vPayload = atoi(vStr.c_str());
					cout << "vPayload(SIP) = " << this->vPayload << endl;
					break;
				}
			}while(true);
		}
	}else if(SIPCodec["H263"]){
		if(stype == VIDEO){
			beginPos = 0;
			do{
				beginPos = sdp.find("a=rtpmap:", beginPos);
				if(beginPos == string::npos)break;
				beginPos += strlen("a=rtpmap:");
				endPos = sdp.find(" ", beginPos);
				string vStr = sdp.substr(beginPos, endPos-beginPos);
				if(sdp.substr(endPos+1, 4) == "H263"){
					this->vPayload = atoi(vStr.c_str());
					cout << "vPayload(SIP) = " << this->vPayload << endl;
					break;
				}
			}while(true);
			cout << "YEP: " << this->vPayload << endl;
		}
	}

	return supportFlag;
}

//helper function: get IP and port(video and audio) numbers from sdp, store them in ip, aport and vport
void ImsAgent::getRemoteIpAndPort(const std::string &sdp, std::string &ip, unsigned short &aport, unsigned short &vport){
	string::size_type beginPos, endPos;
	const char *connection = "c=IN IP4 ";
	beginPos = sdp.find(connection);

	if(beginPos != string::npos){
		beginPos += strlen(connection);
		endPos = sdp.find(webrtc::PARAM_END, beginPos);
		if(endPos != string::npos){
			ip = sdp.substr(beginPos, endPos - beginPos);
		}
	}

	const char *audio = "m=audio ";
	beginPos = sdp.find(audio);
	if(beginPos != string::npos){
		beginPos += strlen(audio);
		endPos = sdp.find(" ", beginPos);
		if(endPos != string::npos){
			string tmp = sdp.substr(beginPos, endPos - beginPos);
			aport = atoi(tmp.c_str());
		}
	}

	const char *video = "m=video ";
	beginPos = sdp.find(video);
	if(beginPos != string::npos){
		beginPos += strlen(video);
		endPos = sdp.find(" ", beginPos);
		if(endPos != string::npos){
			string tmp = sdp.substr(beginPos, endPos - beginPos);
			vport = atoi(tmp.c_str());
		}
	}
}

//call back function for video
void* ImsAgent::cbFuncVideo(void *args){
	struct sockaddr from;
	unsigned char buf[3005];
	socklen_t size = sizeof(struct sockaddr);
	ImsAgent *ptr = (ImsAgent*)args;
	Helper *helper = ptr->videoHelper;
	CodecConverter* con = ptr->converter;

	while(1){
		int recvlen = recvfrom(helper->getListenFd(), buf, 3005, 0, &from, &size);
		ptr->curcnt++;

		if(recvlen == -1){
			printf("Recv data failed in ImsAgent::cbFuncVideo!\n");
			continue;
		}
		con->convertAndSend(buf, recvlen);
	}
	return NULL;
}

//call back function for audio
void* ImsAgent::cbFuncAudio(void *args){
	struct sockaddr from;
	unsigned char buf[3005];
	socklen_t size = sizeof(struct sockaddr);
	ImsAgent *ptr = (ImsAgent*)args;
	Helper *helper = ptr->audioHelper;
	WebAgent *web = ptr->peerAgent;

	while(1){
		int recvlen = recvfrom(helper->getListenFd(), buf, 3005, 0, &from, &size);
		ptr->curcnt++;

		if(recvlen == -1){
			printf("Recv data failed in ImsAgent::cbFuncVideo!\n");
			continue;
		}
		web->sendData(buf, recvlen, 0);
	}
	return NULL;
}


