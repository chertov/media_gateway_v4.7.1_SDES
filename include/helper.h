/*
 * helper.h
 *
 *  Created on: May 12, 2014
 *      Author: sy
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

class CodecConverter;

class Helper{
public:
	Helper();

	~Helper();

	void run(void* (*cbFunc)(void *args), void* para);

	void sendData(unsigned char* data, int len);

	unsigned short getListenPort(){
		return this->listenPort;
	}

	int getListenFd(){
		return listenfd;
	}

	void setDestination(const std::string &dest, unsigned short port);

private:
	pthread_t tid;

	unsigned short listenPort;//receive data port, aka listen port

	sockaddr_in toAddr;

	int sendfd;//send socket file descriptor

	int listenfd;//listen socket file descriptor
};

#endif /* HELPER_H_ */
