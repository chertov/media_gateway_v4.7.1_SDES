/*============================================================================
 *Name        : mediagateway.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : This is the media gateway class, the gateway will connect to
 *				signalling gateway and handle the message received.
 *============================================================================
 */
#include <iostream>
#include "mediagateway.h"
#include "messageparser.h"
#include <string>
#include <assert.h>
using namespace std;
using namespace webrtc;

SOCKET MediaGateway::sGatewaySock;
string MediaGateway::sGatewayHost;
string MediaGateway::sGatewayPort;
std::map<string, MediaSession *> MediaGateway::sessions;

void MediaGateway::connectToSignalingGateway(string host, string port, SOCKET sock){
	sGatewaySock = sock;
	sGatewayHost = host;
	sGatewayPort = port;

//	send the domain name of media gateway to signaling gateway
	const char *domainName = "MEDIA_GATEWAY";
	sendPacket(sGatewaySock, domainName);
	char message[BUFFER_SIZE];
	MediaSession *session;
	bool used = true;
	while(true){
		//receive message from signaling gateway
		if(used == true){
			session = new MediaSession();
			used = false;
		}

		memset(message, 0, BUFFER_SIZE);
		cout << "Waiting to receive data from SG..." << endl << endl;
		int recvlen = readPacket(sGatewaySock, message, BUFFER_SIZE);
		if(recvlen <= 0){
			cout << "SG shutdown!!!" << endl;
			break;
		}
		cout << "Receive message from SG: " << message << endl;
		used = handleSessionMessage(string(message), session);
	}
}

//update by guoxun
/*
 * @func:use crypto or setup to judge if the message comes from the browser
 */
bool MediaGateway::isFromBrowser(const string &message){
	const char flag[] = "crypto";
	const char flag2[] = "fingerprint";

	if((message.find(flag) != string::npos)||(message.find(flag2)!=string::npos)){
		return true;
	}else{
		return false;
	}
}

bool MediaGateway::handleSessionMessage(string message, MediaSession *session){
	bool used = false;

	//parse the message into struct
	SessionMessage sMsg = MessageParser::parserSessionMessage(message);

	//find the offerSessionID, if not exists, create a new MediaSession
	string offerSessionID = sMsg.roap.offerSessionID;

	if(!offerSessionID.empty()){//if offer string(not session[]) is not empty
		map<string, MediaSession*>::iterator iter;
		iter = sessions.find(offerSessionID);
		if(iter == sessions.end())
			used = true;

		//handle the message according to roap type
		int roapType = sMsg.roap.type;
		if(roapType == offer){
			assert(used == true);
			session->setOfferSessionId(offerSessionID);
			sessions.insert(make_pair(offerSessionID, session));

			if(isFromBrowser(message)){
				session->handleOfferFromWeb(sMsg);
			} else{
				session->handleOfferFromIMS(sMsg);
			}
		} else{
			if(used == true || sessions[offerSessionID]->isSessionStop()){
				if(used == false)sessions.erase(offerSessionID);
				cout << "Session has already deleted, something must be wrong or this is RTC_OK." << endl;
				return false;
			}
			session = sessions[offerSessionID];

			if(roapType == candidate){
				session->handleCandidateFromWeb(sMsg);
			}else if(roapType == answer){
				if(isFromBrowser(message)){
					session->handleAnswerFromWeb(sMsg);
				} else{
					session->handleAnswerFromIMS(sMsg);
				}
			}else if(roapType == ok){
				cout << "Recv ok from SG..." << endl;
			}else if(roapType == shutdown_r){
				cout << "Recv shutdown from SG..." << endl;
				delete session;
				sessions.erase(offerSessionID);
			}
		}
	}
	return used;
}

void MediaGateway::sendPacketToSG(string msg){
	sendPacket(sGatewaySock, msg.c_str());
}
