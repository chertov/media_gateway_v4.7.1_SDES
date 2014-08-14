#ifndef SUBMG_H_
#define SUBMG_H_

#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>

namespace{

enum CTRLMSGTYPE{START, OK_FOR_START, STOP};

const int myPort = 12345;
const std::string myIP = "10.109.247.140";
const int MAX_CONNECTION_NUM = 20;
const int SIZE = 1005;

}

class CtrlMsg{
public:
    char msgType;
    char codec;
    unsigned short port;
    long ID;
    CtrlMsg(char typ, char cc, unsigned short p, long id):msgType(typ), codec(cc), port(p), ID(id){}
};

class SubMG{
public:
//  return id on success, -1 on no avaiable subMG, -2 on send failure
    static long startFFmpeg(unsigned short myRecvPort, char codec, std::string &destIP, unsigned short &destPort);

//  return true on success, false on failure
    static bool deleteFFmpeg(long ID);

    static void* run(void*);
private:
    static long getVacID();

    static int sockfd;
    static sockaddr_in myAddr;
    static std::vector< std::pair<std::string, int> > avaSubMG;
    static int idCnt;

//	current state of a session, 0 is the init state, 1 is waiting for OK_FOR_START, 2 for already got OK_FOR_START
    static std::map<long, int> curstate;
    static std::map<long, std::pair<std::string, unsigned> > addr;
};

#endif
