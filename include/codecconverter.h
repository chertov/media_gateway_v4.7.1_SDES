/*
 * codecconverter.h
 *
 *  Created on: May 12, 2014
 *      Author: sy
 */

#ifndef CODECCONVERTER_H_
#define CODECCONVERTER_H_

#include <string>
class MediaSession;
class Helper;

class CodecConverter{
public:
	CodecConverter(bool _type);//type 0 for webtoims, 1 for imstoweb
	~CodecConverter();
	void run();
	void convertAndSend(unsigned char* data, int len);
	void setAttachAgent(void *_agent){this->agent = _agent;}

private:
	static void* cbImsToWeb(void*);
	static void* cbWebToIms(void*);

	bool type;//type 0 for webtoims, 1 for imstoweb

//	for webtoims codec converter, this agent is a ImsAgent,
//	for imstoweb codec converter, this agent is a WebAgent
	void *agent;

	Helper *helper;
};

#endif /* CODECCONVERTER_H_ */
