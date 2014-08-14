/*============================================================================
 *Name        : candidate.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : package the NiceCandidate into candidate class
 *============================================================================
 */

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "candidate.h"

Candidate::Candidate() : candidate(NULL)
{

}

/*
 * parse string into candidate
 */
Candidate::Candidate(const char str[])
{
	NiceCandidateType type;
	if(strstr(str, "host")){
		type = NICE_CANDIDATE_TYPE_HOST;
	} else if(strstr(str, "srflx")){
		type = NICE_CANDIDATE_TYPE_SERVER_REFLEXIVE;
	} else if(strstr(str, "prflx")){
		type = NICE_CANDIDATE_TYPE_PEER_REFLEXIVE;
	} else if(strstr(str, "relayed")){
		type = NICE_CANDIDATE_TYPE_RELAYED;
	} else{
		type = NICE_CANDIDATE_TYPE_HOST;
	}

	this->candidate = nice_candidate_new(type);

	char transport[32];
	char ip[16];
	int port;
	char others[128];
	sscanf(str, "a=candidate:%s %d %s %d %s %d %s", this->candidate->foundation, &(this->candidate->component_id), transport, &(this->candidate->priority), ip, &port, others);

	if(strstr(transport, "udp")){
		this->candidate->transport = NICE_CANDIDATE_TRANSPORT_UDP;
	} else {
		this->candidate->transport = NICE_CANDIDATE_TRANSPORT_UDP;
	}

	NiceAddress *addr = &(this->candidate->addr);
	nice_address_init(addr);
	if(nice_address_set_from_string(addr, ip)){
		printf("set address success!\n");
	} else{
		printf("set address failed!\n");
	}

	nice_address_set_port(addr, port);
	printf("ip = %s, port = %d\n", ip, port);
}

Candidate::Candidate(NiceCandidateType type)
{
	this->candidate = nice_candidate_new(type);
}

Candidate::Candidate(NiceCandidateType type, NiceCandidateTransport transport, NiceAddress addr, guint32 priority, guint streamID, guint componentID, gchar foundation[])
{
	this->candidate = nice_candidate_new(type);
	this->setCandidateProperties(transport, addr, priority, streamID, componentID, foundation);
}

Candidate::~Candidate()
{
	nice_candidate_free(this->candidate);
}

Candidate & Candidate::operator=(Candidate &other)
{
	if(this == &other){
		return *this;
	}
	if(this->candidate != NULL){
		nice_candidate_free(this->candidate);
	}
	this->candidate = nice_candidate_copy(other.getNiceCandidate());
	return *this;
}

void Candidate::setCandidateProperties(NiceCandidateTransport transport, NiceAddress addr, guint32 priority, guint streamID, guint componentID, gchar foundation[])
{
	this->candidate->transport = transport;
	this->candidate->addr = addr;
	this->candidate->priority = priority;
	this->candidate->stream_id = streamID;
	this->candidate->component_id = componentID;
	strcpy(this->candidate->foundation, foundation);
}


