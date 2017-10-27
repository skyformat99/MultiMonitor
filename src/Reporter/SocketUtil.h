#ifndef _SOCKUTILHEADER_
#define _SOCKUTILHEADER_

#pragma warning(disable:4786)
#include <winsock2.h>

// 初始化连接
BOOL InitSocket(SOCKET &sockListen,unsigned short port);

// 连接服务器/断开连接
BOOL ConnectUseName(SOCKET &sock,char *host,unsigned short port);
BOOL ConnectUseIP(SOCKET &sock,char *ip,unsigned short port);
BOOL Connect(SOCKET &sock,char *server,unsigned short port);
BOOL Connect(SOCKET &sock,CString &server,unsigned short port);
void Disconnect(SOCKET sock);

// 收发数据
BOOL SendBuffer(SOCKET sock,const char *buffer,long length);
BOOL ReceiveBuffer(SOCKET sock,char *buffer,long length);
int ReceiveBuffer2(SOCKET sock,char *buffer,long length, int nSecond, int nMiliSecond, int& nRecvBytes);
int ReceiveBuffer3(SOCKET sock,char *buffer,long length, int nSecond, int nMiliSec, int& nRecvBytes);
int ReceiveHttpHead(SOCKET sock,char *buffer,long length, int nSecond);
#endif
