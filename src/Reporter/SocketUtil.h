#ifndef _SOCKUTILHEADER_
#define _SOCKUTILHEADER_

#pragma warning(disable:4786)
#include <winsock2.h>

// ��ʼ������
BOOL InitSocket(SOCKET &sockListen,unsigned short port);

// ���ӷ�����/�Ͽ�����
BOOL ConnectUseName(SOCKET &sock,char *host,unsigned short port);
BOOL ConnectUseIP(SOCKET &sock,char *ip,unsigned short port);
BOOL Connect(SOCKET &sock,char *server,unsigned short port);
BOOL Connect(SOCKET &sock,CString &server,unsigned short port);
void Disconnect(SOCKET sock);

// �շ�����
BOOL SendBuffer(SOCKET sock,const char *buffer,long length);
BOOL ReceiveBuffer(SOCKET sock,char *buffer,long length);
int ReceiveBuffer2(SOCKET sock,char *buffer,long length, int nSecond, int nMiliSecond, int& nRecvBytes);
int ReceiveBuffer3(SOCKET sock,char *buffer,long length, int nSecond, int nMiliSec, int& nRecvBytes);
int ReceiveHttpHead(SOCKET sock,char *buffer,long length, int nSecond);
#endif
