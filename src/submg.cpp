#include "submg.h"
#include "timer.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <glib.h>
#include <assert.h>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include <string>
#include <pthread.h>
#include <map>
#include <unistd.h>
#include <vector>
using namespace std;

//static data member definition of SubMG
int SubMG::sockfd;
sockaddr_in SubMG::myAddr;
vector< std::pair<std::string, int> > SubMG::avaSubMG;
map<long, int> SubMG::curstate;//current state of a session
map<long, pair<string, unsigned> > SubMG::addr;
int SubMG::idCnt = 0;

long SubMG::getVacID(){
	while(1){
		if(++idCnt == 0x7fffffff){
			idCnt = 0;
			continue;
		}

		if(curstate[idCnt])continue;
		break;
	}
	return idCnt;
}

long SubMG::startFFmpeg(unsigned short myRecvPort, char codec, std::string &destIP, unsigned short &destPort){
	cout << "Enter startFFmpeg" << endl;

	long idx = getVacID();
	CtrlMsg msg(START, codec, htons(myRecvPort), htonl(idx));

	char buf[SIZE];
	memset(buf, 0, sizeof(buf));
	memcpy(buf, &msg, sizeof(msg));

	if(avaSubMG.size() == 0){
		cout << "No available Sub MG..." << endl;
		return -1;
	}

	int num = send(avaSubMG[rand()%avaSubMG.size()].second, buf, sizeof(msg), 0);

	if (num <= 0) {
		cout << "Fail to send CtrlMsg to SubMG.." << endl;
		return -2;
	} else {
		cout << "Send successfully, bytes: " << num << endl;
		curstate[idx] = 1;
	}

	while(curstate[idx] == 1){
		usleep(150000);
	}
	assert(curstate[idx] == 2);

	destIP = addr[idx].first;
	destPort = addr[idx].second;

	cout << "Leave startFFmpeg" << endl;
	return idx;
}

void* SubMG::run(void *para) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	int addr_len = sizeof(sockaddr_in);

	sockaddr_in clientAddr;
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(myPort);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&myAddr.sin_zero, 0, sizeof(myAddr.sin_zero));

	if (bind(sockfd, (struct sockaddr*) &myAddr, sizeof(myAddr)) < 0) {
		cout << "Fail on binding." << endl;
		return (void*)1;
	}

	if (listen(sockfd, MAX_CONNECTION_NUM) < 0) {
		cout << "Fail in listening." << endl;
		return (void*)2;
	}

	cout << "SubMG control module starts successfully!" << endl;

	fd_set fds;
	char buf[SIZE];

	while (1) {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);

		int maxsock = sockfd;
		for (int i = 0; i < avaSubMG.size(); i++) {
			FD_SET(avaSubMG[i].second, &fds);
			maxsock = max(maxsock, avaSubMG[i].second);
		}

		select(maxsock + 1, &fds, NULL, NULL, NULL);
		for (int i = 0; i < avaSubMG.size(); i++) {
			if (FD_ISSET(avaSubMG[i].second, &fds)) {
				memset(buf, 0, sizeof(CtrlMsg));

				int num = recv(avaSubMG[i].second, buf, sizeof(buf), 0);
				if (num <= 0) {
					cout << "Sub MG " << avaSubMG[i].first << " closed..." << endl;
					avaSubMG.erase(avaSubMG.begin() + i);
				} else {
					cout << "Recv answer from subMG." << endl;
//					cout << "msg: " << buf << endl;
					CtrlMsg msg(0, 0, 0, 0);
					memcpy(&msg, buf, sizeof(msg));
					assert(msg.msgType == OK_FOR_START);
					msg.ID = ntohl(msg.ID);
					msg.port = ntohs(msg.port);

//usage of while here is necessary
					while(!curstate[msg.ID]){
						usleep(150000);
					}
					curstate[msg.ID] = 2;
					addr[msg.ID].first = avaSubMG[i].first;
					addr[msg.ID].second = msg.port;
					cout << "msg.ID = " << msg.ID << endl;
				}
			}
		}

		if (FD_ISSET(sockfd, &fds)) {
			int clientfd = accept(sockfd, (struct sockaddr*) &clientAddr,
					(socklen_t*) &addr_len);

			if (clientfd < 0) {
				cout << "Not accepted!" << endl;
				continue;
			}
			string IP = (string) inet_ntoa(clientAddr.sin_addr);
			cout << "Client IP: " << IP << ", PORT: "
					<< htons(clientAddr.sin_port) << endl;

			bool flag = 0;
			for (int i = 0; i < avaSubMG.size(); i++) {
				if (avaSubMG[i].first == IP) {
					flag = 1;
					break;
				}
			}

			if (!flag)avaSubMG.push_back(make_pair(IP, clientfd));
			else{
				close(clientfd);
				cout << "Refuse client because of duplicate IP." << endl;
			}
		}
	}
	return NULL;
}
