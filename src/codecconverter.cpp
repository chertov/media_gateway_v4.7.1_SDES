/*
 * codecconverter.cpp
 *
 *  Created on: May 12, 2014
 *      Author: sy
 */

#include "codecconverter.h"
#include "helper.h"
#include "mediasession.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "commontype.h"
#include <iostream>
#include "webagent.h"
#include <sstream>
#include "imsagent.h"
#include <string>
#include "mgdebug.h"
#include "submg.h"
#include "rtpdefine.h"

using namespace std;

extern map<string, int> SIPCodec;

CodecConverter::CodecConverter(bool _type){
	this->agent = NULL;
	this->helper = new Helper();
	this->type = _type;
}

CodecConverter::~CodecConverter(){
	cout << "In delete codecconverter." << endl;
	delete(this->helper);
	cout << "CodecConverter deleted." << endl;
}

void CodecConverter::run(){
	unsigned short myRecvPort = helper->getListenPort();
	char codec;
	string destIP;
	unsigned short destPort;

	if(this->type == 0){
		if(SIPCodec["H264"]){
			long ab = SubMG::startFFmpeg(myRecvPort, 1, destIP, destPort);
		}else if(SIPCodec["H263"]){
			long ab = SubMG::startFFmpeg(myRecvPort, 0, destIP, destPort);
		}
		this->helper->setDestination(destIP, destPort);
		this->helper->run(cbWebToIms, this);
	}else{
		if(SIPCodec["H264"]){
			long ab = SubMG::startFFmpeg(myRecvPort, 3, destIP, destPort);
		}else if(SIPCodec["H263"]){
			long ab = SubMG::startFFmpeg(myRecvPort, 2, destIP, destPort);
		}

		this->helper->setDestination(destIP, destPort);
		this->helper->run(cbImsToWeb, this);
	}
}

void CodecConverter::convertAndSend(unsigned char* data, int len){
	RTP_HEADER *header = (RTP_HEADER *)&data[0];
	header->payload = 96;
	this->helper->sendData(data, len);
}

//call back function from ims to web
void* CodecConverter::cbImsToWeb(void *args){
	struct sockaddr from;
	unsigned char buf[3005];
	socklen_t size = sizeof(struct sockaddr);
	CodecConverter *ptr = (CodecConverter*)args;
	Helper *helper = ptr->helper;
	WebAgent* web = (WebAgent*)ptr->agent;

	while(1){
		int recvlen = recvfrom(helper->getListenFd(), buf, 3005, 0, &from, &size);

		if(recvlen == -1){
			printf("Recv data failed in CodecConverter::cbImsToWeb!\n");
			continue;
		}

		web->sendData(buf, recvlen, 1);
	}
	return NULL;
}

//call back function from web to ims
void* CodecConverter::cbWebToIms(void *args){
	struct sockaddr from;
	unsigned char buf[3005];
	socklen_t size = sizeof(struct sockaddr);
	CodecConverter *ptr = (CodecConverter*)args;
	Helper *helper = ptr->helper;
	ImsAgent* ims = (ImsAgent*)ptr->agent;

	while(1){
		int recvlen = recvfrom(helper->getListenFd(), buf, 3005, 0, &from, &size);

		if(recvlen == -1){
			printf("Recv data failed in CodecConverter::cbWebToIms!\n");
			continue;
		}

		ims->sendData(buf, recvlen, 1);
	}
	return NULL;
}
