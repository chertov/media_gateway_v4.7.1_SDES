/*============================================================================
 *Name        : messageparser.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : this class can parse message string into SessionMessage class
 *				or package SessionMessage class into message string.
 *============================================================================
 */
#ifndef MESSAGEPARSER_H_
#define MESSAGEPARSER_H_

#include <string>
#include "json/json.h"
#include "commontype.h"

class MessageParser{
	public :
		static SessionMessage parserSessionMessage(const std::string message);
		static std::string packageSessionMessage(SessionMessage message);
};


#endif /*MESSAGEPARSER_H_*/
