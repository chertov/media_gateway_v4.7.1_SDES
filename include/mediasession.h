/*============================================================================
 *Name        : mediagateway.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : This is the media session class, each session contains three agent
 *				the webAgent will establish p2p connection with browser and
 *				the imsAudioAgent and imsVideoAgent will establish audio/video p2p
 *				connection with xlite.
 *============================================================================
 */
#ifndef MEDIASESSION_H_
#define MEDIASESSION_H_

#include <string>
#include "commontype.h"
#include <vector>

class CodecConverter;
class ImsAgent;
class WebAgent;

class MediaSession{
public:
	MediaSession(std::string offerSessionID = "");
	~MediaSession();

	void setOfferSessionId(const std::string& offerSessionId) {offerSessionID = offerSessionId;}

	void handleOfferFromWeb(SessionMessage &sMsg);
	void handleAnswerFromIMS(SessionMessage &sMsg);
	void handleOfferFromIMS(SessionMessage &sMsg);
	void handleAnswerFromWeb(SessionMessage &sMsg);
	void handleCandidateFromWeb(SessionMessage &sMsg);

	bool isSessionStop(){
		return this->imsAgent==NULL && this->webAgent==NULL;
	}

private:

	void deleteSession();

	static void* checkMediaFlow(void*);

	void sendCandidatesToWeb(const std::vector<std::string> &, const SessionMessage &);

	void sendOfferOrAnswerToIMS();

	void doTailingJobs();

//	true if web is caller, false if web is callee
	bool webCallerFlag;

//	true if offer of answer from web is arrived
	bool webOfferOrAnswerArrived;

	std::string offerSessionID;
	SessionMessage originalOffer;
	SessionMessage originalAnswer;
	webrtc::SessionType sessionType;

	WebAgent *webAgent;
	ImsAgent *imsAgent;

//	web to ims codec converter
	CodecConverter *webToImsCC;

//	ims to web codec converter
	CodecConverter *imsToWebCC;

	int timerID;
};

#endif /*MEDIASESSION*/
