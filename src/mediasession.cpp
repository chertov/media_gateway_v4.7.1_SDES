/*============================================================================
 *Name        : mediasession.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : This is the media session class, each session contains three agent
 *				the webAgent will establish p2p connection with browser and
 *				the imsAudioAgent and imsVideoAgent will establish audio/video p2p
 *				connection with xlite.
 *============================================================================
 */
#include "mediasession.h"
#include "candidate.h"
#include "commontype.h"
#include "messageparser.h"
#include "mediagateway.h"
#include "webagent.h"
#include "imsagent.h"
#include "timer.h"
#include <codecconverter.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>

using namespace webrtc;
using namespace std;

const int TIMEOUT = 30;

void MediaSession::deleteSession(){
	delete this->webAgent;
	delete this->imsAgent;
	delete this->imsToWebCC;
	delete this->webToImsCC;
	this->webAgent = NULL;
	this->imsAgent = NULL;
	this->imsToWebCC = NULL;
	this->webToImsCC = NULL;
}

MediaSession::MediaSession(string offerSessionID){
	this->webCallerFlag = true;//any value is ok
	this->webOfferOrAnswerArrived = false;
	this->offerSessionID = offerSessionID;
	this->sessionType = AUDIO;//any value is ok
	this->timerID = 0;

	this->webToImsCC = new CodecConverter(0);
	this->imsToWebCC = new CodecConverter(1);

	this->webAgent = new WebAgent(webToImsCC);
	this->imsAgent = new ImsAgent(imsToWebCC);

	webAgent->setPeerAgent(imsAgent);
	imsAgent->setPeerAgent(webAgent);
	webToImsCC->setAttachAgent(imsAgent);
	imsToWebCC->setAttachAgent(webAgent);

	webAgent->run();
	imsAgent->run();

	cout << "New Session finished!" << endl;
}

MediaSession::~MediaSession(){
	cout << "Enter deleting session." << endl;
	this->deleteSession();

	if(this->timerID != 0){
		Timer::stopTimer(this->timerID);
	}
	cout << "Session deleted successfully." << endl;
}

void* MediaSession::checkMediaFlow(void* para){
	MediaSession *ptr = (MediaSession*)para;
	if(!ptr->webAgent->gotPeriodData() || !ptr->imsAgent->gotPeriodData()){
		ptr->deleteSession();
	}else{
		ptr->timerID = Timer::setTimer(TIMEOUT, &checkMediaFlow, ptr);
	}
	return NULL;
}

void MediaSession::doTailingJobs() {
	this->webAgent->doTailingJobs();

	this->imsToWebCC->run();
	this->webToImsCC->run();
	this->timerID = Timer::setTimer(TIMEOUT, &checkMediaFlow, this);
}

void MediaSession::handleOfferFromWeb(SessionMessage &sMsg){
	//save the sdp, get parameters like ice-ufrag, ice-pwd and master key
	originalOffer = sMsg;
	cout << "Receive offer from web!" << endl;
	webCallerFlag = true;
	bool codecSupportFlag = webAgent->handleSdpFromWeb(sMsg.roap.sdp, webCallerFlag, this->sessionType);
	webOfferOrAnswerArrived = true;

	if(codecSupportFlag)
		sendOfferOrAnswerToIMS();
}

void MediaSession::sendCandidatesToWeb(const vector<string> &candidates, const SessionMessage &msg){

	vector<string>::const_iterator iter = candidates.begin();
	for( ; iter != candidates.end(); ++iter){
		SessionMessage candidateSMsg = msg;
		candidateSMsg.roap.sdp = *iter;
		candidateSMsg.roap.type = candidate;
		string candidateMsg = MessageParser::packageSessionMessage(candidateSMsg);

		MediaGateway::sendPacketToSG(candidateMsg.c_str());
	}
	cout << "send candidates to web successfully..." <<endl;
}

void MediaSession::handleAnswerFromIMS(SessionMessage &sMsg){
	originalAnswer = sMsg;
	cout << "Receive answer from ims!"<< endl;
	bool codecSupportFlag = imsAgent->handleSdpFromIMS(sMsg.roap.sdp, this->sessionType);

	if(!codecSupportFlag)return ;

	SessionMessage answerMsg = originalAnswer;

	string sdp = webAgent->createSdpToWeb(sessionType, webCallerFlag);

	//replace the sdp
	answerMsg.roap.sdp = sdp;

	string msg = MessageParser::packageSessionMessage(answerMsg);

	MediaGateway::sendPacketToSG(msg.c_str());


	for(int i = 0; i < 3; ++i)
		sendCandidatesToWeb(webAgent->getLocalCandidates(), originalAnswer);

	doTailingJobs();
}

void MediaSession::handleOfferFromIMS(SessionMessage &sMsg){
	//save the sdp
	originalOffer = sMsg;
	cout << "Receive offer from ims!"<< endl;

	webCallerFlag = false;


	bool codecSupportFlag = imsAgent->handleSdpFromIMS(sMsg.roap.sdp, this->sessionType);

	if(!codecSupportFlag)return ;

	//get the original answer from ims to construct the answer to browser
	SessionMessage offerSMsg = originalOffer;

	string sdp = webAgent->createSdpToWeb(sessionType, webCallerFlag);
	offerSMsg.roap.sdp = sdp;

	string offerMsg = MessageParser::packageSessionMessage(offerSMsg);
	MediaGateway::sendPacketToSG(offerMsg.c_str());

	for(int i=0; i<3; i++)
		sendCandidatesToWeb(webAgent->getLocalCandidates(), originalOffer);
}

void MediaSession::handleAnswerFromWeb(SessionMessage &sMsg){
	originalAnswer = sMsg;
	cout << "Receive answer from web!"<< endl;
	bool codecSupportFlag = webAgent->handleSdpFromWeb(sMsg.roap.sdp, webCallerFlag, this->sessionType);
	webOfferOrAnswerArrived = true;

	if(codecSupportFlag){
		sendOfferOrAnswerToIMS();
		doTailingJobs();
	}
}

void MediaSession::handleCandidateFromWeb(SessionMessage &sMsg){
//	judge whether is the last candidate
	int ret = webAgent->handleCandidateFromWeb(sMsg);

	if(ret == 1){
		cout << "Receive last candidate from web!" << endl;
		sendOfferOrAnswerToIMS();
	}
}

void MediaSession::sendOfferOrAnswerToIMS(){
	if(webOfferOrAnswerArrived && webAgent->recvCandidatesDone()){
		webAgent->setRemoteCandidates();
		SessionMessage sMsg;
		if(webCallerFlag == true){
			sMsg = originalOffer;
		} else{
			sMsg = originalAnswer;
		}

		string sdp = imsAgent->createSdpToIMS(sessionType);
		sMsg.roap.sdp = sdp;

		string msg = MessageParser::packageSessionMessage(sMsg);
		MediaGateway::sendPacketToSG(msg.c_str());
	}
}

