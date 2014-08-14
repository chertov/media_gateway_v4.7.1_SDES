/*
 * mgdebug.h
 *
 *  Created on: May 14, 2014
 *      Author: guoxun
 */

#ifndef MGDEBUG_H_
#define MGDEBUG_H_

//#define __WEBRTC__SK__

#ifdef __WEBRTC__SK__
#define MG_DEBUG(format,...) printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define MG_DEBUG(format,...)
#endif


#endif /* MGDEBUG_H_ */
