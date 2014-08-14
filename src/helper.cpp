
/*
 * helper.cpp
 *
 *  Created on: May 12, 2014
 *      Author: sy
 */


#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <glib.h>
#include <glib-object.h>
#include <time.h>
#include <cstring>
#include <iostream>
using namespace std;

Helper::Helper(){
	this->tid = 0;
	this->listenPort = 0;
	setDestination("10.109.247.140", 12345);

	do{
		sendfd = socket(AF_INET, SOCK_DGRAM, 0);
	}while(sendfd == -1);

	do{
		listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	}while(listenfd == -1);

	sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&myaddr.sin_zero, 0, sizeof(myaddr.sin_zero));

	int failedtimes = -1;
	do{
		failedtimes++;
		this->listenPort = 10001 + rand()%55000;
		myaddr.sin_port = htons(this->listenPort);
		printf("On port = %d\n", this->listenPort);
	}while(bind(listenfd, (struct sockaddr*)&myaddr, sizeof(struct sockaddr)) == -1);
	printf("Bind successfully on port %u after trying %d times.\n", ntohs(myaddr.sin_port), failedtimes);
}

Helper::~Helper(){

	cout << "In delete helper." << endl;
	pthread_cancel(this->tid);
	g_assert(close(this->sendfd) == 0);

	g_assert(close(this->listenfd) == 0);
	cout << "helper deleted." << endl;
}

void Helper::run(void* (*cbFunc)(void* args), void* para){
	cout << "in helper::run, listen on port: " << this->listenPort << ", send to port" << ntohs(toAddr.sin_port) << endl;
	int ret = pthread_create(&tid, NULL, cbFunc, para);
	g_assert(ret == 0);
}

void Helper::sendData(unsigned char* data, int len){
	int sz = sizeof(struct sockaddr);
	int sendlen = sendto(this->sendfd, data, len, 0, (const struct sockaddr*)&this->toAddr, sz);
}

void Helper::setDestination(const std::string &dest, unsigned short port){
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons(port);
	toAddr.sin_addr.s_addr = inet_addr(dest.c_str());
	memset(&toAddr.sin_zero, 0, sizeof(toAddr.sin_zero));
}




