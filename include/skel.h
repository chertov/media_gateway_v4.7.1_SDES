/*============================================================================
 *Name        : skel.h
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : define the macro related with socket
 *============================================================================
 */
#ifndef _SKEL_H_
#define _SKEL_H_

#define EXIT(s) exit(s)
#define CLOSE(s) if(close(s)) error(1, errno "close failed!")
#define set_errno(e) errno = e
#define isvalidsock(s) ((s) >= 0)

typedef int SOCKET;

#endif /* _SKEL_H_*/
