/*============================================================================
 *Name        : webagent.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : this class packages the NiceAgent into WebAgent
 and each WebAgent object is a thread
 *============================================================================
 */
#ifndef WEBAGENT_H_
#define WEBAGENT_H_

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <glib.h>
#include <glib-object.h>
#include <nice/agent.h>
#include "commontype.h"
#include "rtpdefine.h"
#include "rtpconverter.h"

class CodecConverter;
class ImsAgent;

class WebAgent {
public:
	WebAgent(CodecConverter*);

	~WebAgent();

	void run();

	void setPeerAgent(ImsAgent *peerAgent) {this->peerAgent = peerAgent;}

//	return 1 if this candidate is the last candidate, 0 otherwise
	int handleCandidateFromWeb(SessionMessage &sMsg);

	void sendData(unsigned char *mediaData,  int len, bool type); //0 for audio, 1 for video)

	void setRemoteCandidates();

	bool recvCandidatesDone(){return lastCandidateArrived;}

//	return true on support codecs, false otherwise
	bool handleSdpFromWeb(std::string, bool, webrtc::SessionType&);

	std::string createSdpToWeb(webrtc::SessionType, bool);

	void doTailingJobs();

	const std::vector<std::string> &getLocalCandidates();

	bool gotPeriodData(){
		bool ret = curcnt != precnt;
		if(ret){
			precnt = curcnt;
		}
		return ret;
	}

private:

	static void sendrtcp(WebAgent*);

	void setCredentials(const std::string &ufrag, const std::string &pwd);

	void getCredentials(std::string &ufrag, std::string &pwd);
	std::string getCryptoInfo(const std::string &);
	void getIceInfo(const std::string &, std::string &, std::string &);

	void replaceCrypto(std::string &sdp, const std::string &);
	void replaceUfragPwd(std::string &sdp);

	static void *startICE(void *args);

	static void receiveDataCallback(NiceAgent *agent, guint streamID,
			guint componentID, guint len, gchar *buf, gpointer userData);

	static void newCandidateCallback(NiceAgent *agent, guint streamID,
			guint componentID, gchar *foundation, gpointer data);

	static void candidateGatheringDoneCallback(NiceAgent *agent, guint streamID,
			gpointer data);

	/*
	 * @Func:  This signal is fired when we received our first binding request from the peer.
	 */
	static void initialBindingRequestReceivedCallback(NiceAgent *agent,
			guint streamID, gpointer data);

	/*
	 * @Func:  This signal is fired once a candidate pair is selected for data transfer for a stream's component
	 */
	static void newSelectedPairCallback(NiceAgent *agent, guint streamID,
			guint componentID, gchar *lfoundation, gchar *rfoundation,
			gpointer data);

	static void componentStateChangedCallback(NiceAgent *agent, guint streamID,
			guint componentID, guint state, gpointer data);

	pthread_t tid;
	NiceAgent *agent;
	guint streamID;
	GMainLoop *mainLoop;
	GMainContext *mainContext;

	std::vector<std::string> remoteRTPCandidates;
	std::vector<std::string> remoteRTCPCandidates;
	std::vector<std::string> localCandidates;
	std::string iceUfrag;
	std::string icePwd;

	NiceComponentState agentState[2];

//	indicate whether all local candidates have been gathered
	bool isCandidateGatheringDone;

//	indicate whether the agent is ready to send and received data
	bool isReady;

//	indicate whether last candidate from web has arrived
	bool lastCandidateArrived;

	std::string cryptoFromWeb;
	std::string cryptoToWeb;

//	SSRC means SSRC of local media flow, not SSRC of web media flow
	std::string audioSSRC;
	std::string videoSSRC;

	ImsAgent *peerAgent;
	CodecConverter *codecConverter;
	RTPConverter *rtpConverter;

//	used for sending RTCP to web
	std::string videoSSRCFromWeb;

	int vPayload;
	int aPayload;

	int precnt;
	int curcnt;
};

#endif /* WEBAGENT_H_ */
