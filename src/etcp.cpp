/*============================================================================
 *Name        : etcp.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : this is a socket library, packaging all the operation related with socket
 *============================================================================
 */

#include <cerrno>
#include <arpa/inet.h>
#include <string>
#include "etcp.h"

void error(int exitCode, int errCode, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if(errCode){
		fprintf(stderr, ", %s (%d)\n", strerror(errCode), errCode);
	}
	if(exitCode){
		EXIT(exitCode);
	}
}

void setAddress(const char *host, const char *port, struct sockaddr_in *sockaddr, const char *protocol)
{
	memset(sockaddr, 0, (size_t)sizeof(sockaddr_in));
	sockaddr->sin_family = AF_INET;
	if(NULL != host){
		unsigned long lhost = inet_addr(host);
		if(-1 == lhost){
			struct hostent *ht = gethostbyname(host);
			if(NULL == ht){
				error(1, 0, "unknown host: %s", host);
			}
			sockaddr->sin_addr = *(struct in_addr*)ht->h_addr_list;
		} else{
			sockaddr->sin_addr.s_addr = lhost;
			//printf("%ld", lhost);
		}
	} else{
		sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	}

	char *endptr;
	short sport = strtol(port, &endptr, 0);
	if(*endptr == '\0'){
		sockaddr->sin_port = htons(sport);
	} else{
		struct servent *st;
		st = getservbyname(port, protocol);
		if(NULL == st){
			error(1, 0, "unknown service(port): %s", port);
		}
		sockaddr->sin_port = st->s_port;
	}
}

SOCKET tcpClient(const char* host, const char* port)
{
	struct sockaddr_in peer;
	SOCKET s;
	int ret = 0;

	setAddress(host, port, &peer, "tcp");
	s = socket(AF_INET, SOCK_STREAM, 0);

	if(!isvalidsock(s)){
		error(1, errno, "new socket failed!");
	}
	//printf("%s\n", getAddress(s).c_str());
	ret = connect(s, (struct sockaddr*) &peer, sizeof(peer));
	if(ret){
		error(1, errno, "connect failed!");
	} else{
		printf("Have established connection with Signal Gateway %s:%s\n", host, port);
	}

	return s;
}

void client(SOCKET s)
{
	char buffer[BUFFER_SIZE];
	int n = 0;

	while(true){
		n = recv(s, buffer, sizeof(buffer), 0);
		if(n < 0){
			error(1, errno, "receive failed!");
			break;
		}
		write(1, buffer, n);
	}
}

SOCKET tcpServer(const char* host, const char* port)
{
	struct sockaddr_in local;
	SOCKET s;
	const int on = 1;
	setAddress(host, port, &local, "tcp");
	s = socket(AF_INET, SOCK_STREAM, 0);

	if(isvalidsock(s)){
		error(1, errno, "new socket failed!");
	}

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))){
		error(1, errno, "setsockopt failed!");
	}

	if(bind(s, (struct sockaddr *)&local, sizeof(local))){
		error(1, errno, "bind failed!");
	}
	if(listen(s, 5)){
		error(1, errno, "listen failed!");
	}
	return s;
}

void server(SOCKET s, struct sockaddr_in *peer)
{

}

/*
 * read n bytes from socket
 *
 * return :
 * 		-1 -> readerror
 * 		n  -> the number of character received
 */
int readn(SOCKET s, char *buf, size_t len)
{
	int recvlen;
	int needToReceive = len;

	while(needToReceive > 0){
		recvlen = recv(s, buf, needToReceive, 0);
		if(recvlen < 0){
			if(errno == EINTR){
				//interrupt by system call
				continue;
			}
			return -1;
		}
		if(recvlen == 0){
			//return the fact num of character received
			return len - needToReceive;
		}
		buf += recvlen;
		needToReceive -= recvlen;
	}
	return len;
}

/*
 * read a packet, the struct of packet is
 * struct{
 * 	u_int32_t len;
 * 	char data[size];
 * }packet;
 *
 * return :
 * 		-1 -> readerror or buf isn't enough
 * 		0  -> EOF disconnection
 */
int readPacket(SOCKET s, char *buf, size_t len)
{
	u_int32_t packetlen = 0;
	int recvlen;

	//retrieve the length of the packet
	recvlen = readn(s, (char *)&packetlen, sizeof(u_int32_t));
	if(recvlen != sizeof(u_int32_t)){
		return recvlen < 0 ? recvlen : 0;
	}

	packetlen = ntohl(packetlen);

	if(packetlen > len){
		//the length of buffer isn't enough
		while(packetlen > 0){
			recvlen = readn(s, buf, len);
			if(recvlen != len){
				return recvlen < 0 ? recvlen : 0;
			}
			packetlen -= recvlen;
			if(len > packetlen){
				len = packetlen;
			}
		}
		return -1; //read error
	}

	recvlen = readn(s, buf, packetlen);
	if(recvlen != packetlen){
		return recvlen < 0 ? recvlen : 0;
	}
	return recvlen;
}

/*
 * send a packet, the struct of packet is
 * struct{
 * 	u_int32_t len;
 * 	char data[size];
 * }packet;
 *
 * return :
 * 		-1 -> readerror or buf isn't enough
 * 		0  -> EOF disconnection
 */
int sendPacket(SOCKET s, const char *buf)
{
	int sendlen = 0;
	int n = strlen(buf);
	if(n > 0){
		Packet packet;
		strcpy(packet.buf, buf);
		packet.len = htonl(n);
		sendlen = send(s, (char *)&packet, n + sizeof(packet.len), 0);
		if(sendlen < 0){
			error(1, 0, "send faild!");
		}
	}
	return sendlen;
}

std::string getAddress(int socket_handler)
{
	struct sockaddr_in socket_address;
	socklen_t addr_len;
	char buffer[50];
	std::string ip_address;
	memset(buffer, 0, 50);
	if (socket_handler <= 0)
		return ip_address;
	if (getsockname(socket_handler, (struct sockaddr*)&socket_address, &addr_len) == -1) {
			return ip_address;
	}
	snprintf(buffer, 50, "%s", inet_ntoa(socket_address.sin_addr));
	ip_address = (std::string)buffer;
	return ip_address;
}

