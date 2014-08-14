#ifndef TIMER_H_
#define TIMER_H_

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
#include <unistd.h>
#include <string>
#include <pthread.h>
#include <map>
#include <vector>

//struct which serves class Timer
class Ts;

class Timer{
public:
//	start a timer
//	para: timeout(in seconds), ptr is the callback function pointer, para is a pointer to the arguments pass to ptr.
//	return: Timer id
	static int setTimer(int timeout, void* (*ptr)(void*), void* para);

//	stop a Timer by id
//	return 0 on success, -1 if no Timer with such id
	static int stopTimer(int id);
private:
	static void* callback(void *para);
	static void* onTimeout(void* para);

//	data member
	static std::map<int, Ts*> tpool;
	static std::map<int, pthread_t> tpool2;
	static int cnt;
};

#endif
