/*============================================================================
 *Name        : media_gateway.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : the main function
 *============================================================================
 */
#include <iostream>
#include <cstdlib>
#include <string>
#include <pthread.h>
#include "mediagateway.h"
#include "messageparser.h"
#include "commontype.h"
#include "rtpconverter.h"
#include "codecconverter.h"
#include <glib.h>
#include <glib-object.h>
#include "webagent.h"
#include "imsagent.h"
#include <map>
#include "submg.h"
using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

map<string, int> SIPCodec;

int main(){
	SIPCodec["H264"] = 1;
//	SIPCodec["H263"] = 1;

	srand(time(NULL));
//	Initialize glib library
	g_type_init();
	#if !GLIB_CHECK_VERSION(2, 31, 8)
		g_thread_init(NULL);
	#endif

//	initialize srtp library
	srtp_init();

	string host = "10.109.247.134";
	string port = "9870";
	SOCKET sock = tcpClient(host.c_str(), port.c_str());

	pthread_t tid;
	pthread_create(&tid, NULL, &SubMG::run, NULL);

	MediaGateway::connectToSignalingGateway(host, port, sock);

	return 0;
}



//int main(){
//
//	pthread_t tid;
//
//	pthread_create(&tid, NULL, &SubMG::run, NULL);
//
//	fd_set fds;
//
//
//	int maxfd = 1;
//
//	while(1){
//		FD_ZERO(&fds);
//		FD_SET(0, &fds);
//
//		select(maxfd, &fds, NULL, NULL, NULL);
//
//		string str;
//		cin >> str;
//		cout << "hello: " << str << endl;
//
//		string IP;
//		unsigned short pt;
//		long id = 0;
//
//		id = SubMG::startFFmpeg(7788, 1, IP, pt);
//
//		if(id > 0){
//			cout << "IP = " << IP << ", port = " << pt << endl;
//		}else{
//			cout << "startFFmpeg Returns : " << id << endl;
//		}
//	}
//	return 0;
//}
