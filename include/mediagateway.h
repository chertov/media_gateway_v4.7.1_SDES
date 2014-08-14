/*============================================================================
 *Name        : mediagateway.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : This is the media gateway class, the gateway will connect to
 *				signalling gateway and handle the message received.
 *============================================================================
 */
#ifndef MEDIAGATEWAY_H_
#define MEDIAGATEWAY_H_

#include <map>
#include <string>
#include "json/json.h"
#include "etcp.h"
#include "mediasession.h"
#include "commontype.h"



class MediaGateway{
	public :
		static void connectToSignalingGateway(std::string, std::string, SOCKET);
		static void sendPacketToSG(std::string);

	private:
		static bool handleSessionMessage(std::string message, MediaSession *session);
		static bool isFromBrowser(const std::string &message);

		static SOCKET sGatewaySock;
		static std::string sGatewayHost;
		static std::string sGatewayPort;
		static std::map<std::string, MediaSession *> sessions;
};

#endif
