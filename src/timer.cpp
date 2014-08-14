#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <assert.h>
#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <pthread.h>
#include <map>
#include <vector>
#include "timer.h"
using namespace std;

map<int, Ts*> Timer::tpool;
map<int, pthread_t> Timer::tpool2;
int Timer::cnt = 0;

class Ts{
public:
	int timeout;
	void* (*ptr)(void*);
	void *para;
	int id;
	Ts(int t, void* (*p)(void*), void* arg, int idx){
		timeout = t;
		ptr = p;
		para = arg;
		id = idx;
	}

	~Ts(){
		cout << "Enter ~TS()..." << endl;
		cout << "Leave ~TS()..." << endl;
	}
};

int Timer::setTimer(int timeout, void* (*ptr)(void*), void* para){
	pthread_t tid;
	if(++cnt == 0x7fffffff)cnt = 1;
	Ts *t = new Ts(timeout, ptr, para, cnt);
	pthread_create(&tid, NULL, callback, t);
	tpool[cnt] = t;
	tpool2[cnt] = tid;
	return cnt;
}

int Timer::stopTimer(int id){
	if(tpool[id]){
		cout << "In stop timer..." << endl;
		delete tpool[id];
		cout << "CP1" << endl;
		tpool[id] = 0;

		pthread_cancel(tpool2[id]);

		pthread_join(tpool2[id], NULL);
		cout << "CP2" << endl;
		cout << "Leave stop timer." << endl;
		return 0;
	}else{
		cout << "No such Timer id!!!" << endl;
		return -1;
	}
}

void* Timer::callback(void *para){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	Ts *t = (Ts*)para;
	int timeout = t->timeout;
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	select(0, NULL, NULL, NULL, &tv);
	onTimeout(para);
	pthread_exit(0);
	return NULL;
}

void* Timer::onTimeout(void* para){
	cout << "TIMEOUT happened!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	Ts *t = (Ts*)para;
	void* (*ptr)(void*) = t->ptr;
	void *arg = t->para;
	int id = t->id;
	ptr(arg);

	if(tpool[id])
		delete tpool[id];

	puts("CP3");

	tpool[id] = 0;
	cout << "TIMEOUT finished..." << endl;
	return NULL;
}

