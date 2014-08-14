/*============================================================================
 *Name        : etcp.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : this is a socket library, packaging all the operation related with socket
 *============================================================================
 */
#ifndef ETCP_H_
#define ETCP_H_

#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "skel.h"

#define BUFFER_SIZE 10240

typedef struct Packet{
	u_int32_t len;
	char buf[BUFFER_SIZE];
} Packet, *PacketPtr;

void error(int exitCode, int errCode, const char *fmt, ...);

/*
 * set the host and port into sockaddr
 */
void setAddress(const char *host, const char *port, struct sockaddr_in *sockaddr, const char *protocol);

void client(SOCKET s);

/*
 * create a client
 */
SOCKET tcpClient(const char* host, const char* port);

/*
 * create a server
 */
SOCKET tcpServer(const char* host, const char* port);

/*
 * read len bytes from socket s
 */
int readn(SOCKET s, char *buf, size_t len);

/*
 * read a whole packet
 */
int readPacket(SOCKET s, char *buf, size_t len);

/*
 * send a packet
 */
int sendPacket(SOCKET s, const char *buf);

std::string getAddress(int socket_handler);

#endif /* ETCP_H_ */
