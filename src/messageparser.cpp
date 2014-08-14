#include "messageparser.h"
#include "commontype.h"
#include <string>
using std::string;

SessionMessage MessageParser::parserSessionMessage(string message)
{
	SessionMessage sessionMsg;
	Json::Reader reader;
	Json::Value session;
	if(reader.parse(message, session)){
		if(!session["type"].isNull()){
			sessionMsg.type = session["type"].asInt();
		}
		if(!session["from"].isNull()){
			sessionMsg.from = session["from"].asString();
		}
		if(!session["to"].isNull()){
			sessionMsg.to = session["to"].asString();
		}
		if(!session["roap"].isNull()){
			Json::Value roap(session["roap"]);
			if(!roap["type"].isNull()){
				sessionMsg.roap.type = (RoapType)roap["type"].asInt();
			}
			if(!roap["offerSessionId"].isNull()){
				sessionMsg.roap.offerSessionID = roap["offerSessionId"].asString();
			}
			if(!roap["answerSessionId"].isNull()){
				sessionMsg.roap.answerSessionID = roap["answerSessionId"].asString();
			}
			if(!roap["seq"].isNull()){
				sessionMsg.roap.seq = roap["seq"].asInt();
			}
			if(!roap["sdp"].isNull()){
				if(sessionMsg.roap.type == offer || sessionMsg.roap.type == answer){
					Json::Value sdp(roap["sdp"]);
					if(!sdp["sdp"].isNull()){
						sessionMsg.roap.sdp = sdp["sdp"].asString();
					}
				}else{
					sessionMsg.roap.sdp = roap["sdp"].asString();
				}
			}else{
				sessionMsg.roap.sdp = "";
			}
			if(!roap["label"].isNull()){
				sessionMsg.roap.label = roap["label"].asInt();
			}
			if(!roap["error"].isNull()){
				sessionMsg.roap.error = (ErrorType)roap["error"].asInt();
			}
			if(!roap["moreComingFlag"].isNull()){
				sessionMsg.roap.moreComingFlag = roap["moreComingFlag"].asBool();
			}
			if(!roap["tiebreaker"].isNull()){
				sessionMsg.roap.tiebreaker = roap["tiebreaker"].asString();
			}
			if(!roap["msgSize"].isNull()){
				sessionMsg.roap.msgSize = roap["msgSize"].asInt();
			}
			if(!roap["msgContent"].isNull()){
				sessionMsg.roap.msgContent = roap["msgContent"].asString();
			}
		}
	}
	return sessionMsg;
}
string MessageParser::packageSessionMessage(SessionMessage sessionMsg)
{
	Json::Value roap;
	roap["type"] = Json::Value(sessionMsg.roap.type);
	roap["offerSessionId"] = Json::Value(sessionMsg.roap.offerSessionID);
	roap["answerSessionId"] = Json::Value(sessionMsg.roap.answerSessionID);
	roap["seq"] = Json::Value(sessionMsg.roap.seq);
	if(sessionMsg.roap.type == offer || sessionMsg.roap.type == answer){
		Json::Value sdp;
		sdp["sdp"] = Json::Value(sessionMsg.roap.sdp);
		if(sessionMsg.roap.type == offer){
			sdp["type"] = Json::Value("offer");
		} else{
			sdp["type"] = Json::Value("answer");
		}
		roap["sdp"] = sdp;
	} else{
		roap["sdp"] = Json::Value(sessionMsg.roap.sdp);
	}
	if(sessionMsg.roap.label == -1){
		roap["label"] = Json::Value(NULL);
	}else{
		roap["label"] = Json::Value(sessionMsg.roap.label);
	}

	if(sessionMsg.roap.error == -1){
		roap["error"] = Json::Value(NULL);
	}else{
		roap["error"] = Json::Value(sessionMsg.roap.error);
	}

	if(sessionMsg.roap.msgSize == -1){
		roap["msgSize"] = Json::Value(NULL);
	}else{
		roap["msgSize"] = Json::Value(sessionMsg.roap.msgSize);
	}


	roap["moreComingFlag"] = Json::Value(sessionMsg.roap.moreComingFlag);
	roap["tiebreaker"] = Json::Value(sessionMsg.roap.tiebreaker);
	roap["msgContent"] = Json::Value(sessionMsg.roap.msgContent);

	Json::Value message;
	message["type"] = Json::Value(sessionMsg.type);
	message["from"] = Json::Value(sessionMsg.from);
	message["to"] = Json::Value(sessionMsg.to);
	message["roap"] = roap;

	Json::FastWriter fast_writer;
	return fast_writer.write(message);
}

//void MessageParser::parseMessage(string message)
//{
//	Json::Reader reader;
//	Json::Value session;
//	if(reader.parse(message, session)){
//		if(!session["type"].isNull()){
//			this->msgType = session["type"].asInt();
//		}
//		if(!session["from"].isNull()){
//			this->from = session["from"].asString();
//		}
//		if(!session["to"].isNull()){
//			this->to = session["to"].asString();
//		}
//		if(!session["roap"].isNull()){
//			Json::Value roap(session["roap"]);
//			if(!roap["type"].isNull()){
//				this->roapType = roap["type"].asInt();
//			}
//			if(!roap["offerSessionId"].isNull()){
//				this->offerSessionID = roap["offerSessionId"].asString();
//			}
//			if(!roap["answerSessionId"].isNull()){
//				this->answerSessionID = roap["answerSessionId"].asString();
//			}
//			if(!roap["seq"].isNull()){
//				this->seq = roap["seq"].asInt();
//			}
//			if(!roap["sdp"].isNull()){
//				Json::Value sdp(roap["sdp"]);
//				if(!sdp["sdp"].isNull()){
//					this->sdp = sdp["sdp"].asString();
//				}
//			}
//			if(!roap["label"].isNull()){
//				this->label = roap["label"].asInt();
//			}
//			if(!roap["error"].isNull()){
//				this->errorType = roap["error"].asInt();
//			}
//			if(!roap["moreComingFlag"].isNull()){
//				this->moreComingFlag = roap["moreComingFlag"].asBool();
//			}
//			if(!roap["tiebreaker"].isNull()){
//				this->tiebreaker = roap["tiebreaker"].asString();
//			}
//			if(!roap["msgSize"].isNull()){
//				this->msgSize = roap["msgSize"].asInt();
//			}
//			if(!roap["msgContent"].isNull()){
//				this->msgContent = roap["msgContent"].asString();
//			}
//		}
//	}
//}
