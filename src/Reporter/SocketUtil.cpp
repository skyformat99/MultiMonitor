/*
   �ļ�����     : SocketUtil.cpp
   �ļ�����	    : TcpͨѶ���ߺ���ģ��
   ��Ʒ��ʶ     : L2AppServer
*/
#pragma warning(disable:4786)
#include <afx.h>
#include "SocketUtil.h"
BOOL InitSocket(SOCKET &sockListen,unsigned short port)
{
	SOCKADDR_IN sockLocal;

	sockListen = socket(AF_INET,SOCK_STREAM,0);
	if (sockListen == INVALID_SOCKET) 
		return FALSE;
	
	sockLocal.sin_family = AF_INET;
	sockLocal.sin_addr.s_addr = INADDR_ANY;
    sockLocal.sin_port = htons(port);
	if (bind(sockListen,(struct sockaddr FAR *)&sockLocal,sizeof(sockLocal)) == SOCKET_ERROR)
	{
		closesocket(sockListen);
		return FALSE;
	}

	int t = SOMAXCONN;
	if (listen(sockListen,32) < 0)
	{
		closesocket(sockListen);
		return FALSE;
	}

	return TRUE;
}
BOOL ConnectUseName(SOCKET &sock,char *host, unsigned short port)
{
	PHOSTENT phe;
	SOCKADDR_IN dest_sin;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock == INVALID_SOCKET) return FALSE;
	dest_sin.sin_family = AF_INET;
	phe = gethostbyname(host);
	if (phe == NULL) 
	{
		closesocket(sock);
		return FALSE;
	}
	memcpy((char FAR *)&(dest_sin.sin_addr),phe->h_addr,phe->h_length);
	dest_sin.sin_port = htons(port);
	if (connect(sock,(PSOCKADDR)&dest_sin,sizeof(dest_sin)) < 0) 
	{
		closesocket(sock);
		return FALSE;
	}

	return TRUE;
}

BOOL ConnectUseIP(SOCKET &sock,char *ip, unsigned short port)
{
	SOCKADDR_IN dest_sin;
	int n1,n2,n3,n4;
	int nRet;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock == INVALID_SOCKET) return FALSE;
	dest_sin.sin_family = AF_INET;

	// ������ջ�����
	int nRecvBuf=256*1024;//����Ϊ256K
	nRet = setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	if (sscanf(ip,"%d.%d.%d.%d",&n1,&n2,&n3,&n4) != 4) return FALSE;
	
	dest_sin.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)n1;
	dest_sin.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)n2;
	dest_sin.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)n3;
	dest_sin.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)n4;
	dest_sin.sin_port = htons(port);
	if (connect(sock,(PSOCKADDR)&dest_sin,sizeof(dest_sin)) < 0) 
	{
		closesocket(sock);
		return FALSE;
	}

	return TRUE;
}

BOOL Connect(SOCKET &sock,char *server, unsigned short port)
{
	int n1,n2,n3,n4;

	if (sscanf(server,"%d.%d.%d.%d",&n1,&n2,&n3,&n4) != 4)
		return ConnectUseName(sock,server,port);
	else
		return ConnectUseIP(sock,server,port);
}
//
//BOOL Connect(SOCKET &sock,CString &server,unsigned short port)
//{
//	int n1,n2,n3,n4;
//	char strServer[64];
//
//	strncpy(strServer,server,63);
//	strServer[63] = '\0';
//
//	if (sscanf(strServer,"%d.%d.%d.%d",&n1,&n2,&n3,&n4) != 4)
//		return ConnectUseName(sock,strServer,port);
//	else
//		return ConnectUseIP(sock,strServer,port);
//}

void Disconnect(SOCKET sock)
{
	if (sock != INVALID_SOCKET)
		closesocket(sock);
}

BOOL SendBuffer(SOCKET sock,const char *buffer,long length)
{
	/* check buffer */
	/*for( int i = 0 ; i < length ; i ++){
		char c = buffer[i];
		if( c == ';' || c == '&' || c == '.' || c == '=' ||
			(c >= 97 && c <= 122) ||
			(c >= 48 && c <= 57 ) ||
			(c >= 65 && c <= 90)
			){

		}
		else{
			int asdf = 0;
			asdf ++;
		}
	}*/
	int status;					// ͨ�Ų����Ľ��
	short nSent;				// �ѷ��͵��ֽ���
	fd_set sockSet;				// ����Socket����
	int nWritingSock;			// �Ƿ���Է�����Ϣ
	struct timeval interval;	// �ȴ���ʱ����

	nSent = 0;
	while (TRUE)
	{
		// ����Ƿ���Է�������
		FD_ZERO(&sockSet);
		FD_SET(sock,&sockSet);
		interval.tv_sec = 8;
		interval.tv_usec = 888;
		nWritingSock = select(0,NULL,&sockSet,NULL,&interval);

		// ��ʱ����
		if (nWritingSock != 1) 
			return FALSE;

		// ���ջ������е�����
		status = send(sock,buffer+nSent,length-nSent,0);

		// �������ݳ���
		if (status <= 0) return FALSE;

		// ���ӽ��յ����ֽ���
		nSent += status;

		// ���������ȫ���������򷵻�
		if (nSent >= length)
			return TRUE;
	}
}

BOOL ReceiveBuffer(SOCKET sock,char *buffer,long length)
{
	int status;					// ͨ�Ų����Ľ��
	//short nReceived;			// ���յ����ֽ���
	long nReceived;
	fd_set sockSet;				// ����Socket����
	int nReadingSock;			// ������Ϣ�Ŀͻ���
	struct timeval interval;	// �ȴ���ʱ����

	nReceived = 0;
	while (TRUE)
	{
		// �����ջ��������Ƿ�������
		FD_ZERO(&sockSet);
		FD_SET(sock,&sockSet);
		interval.tv_sec = 8;
		interval.tv_usec = 888;
		nReadingSock = select(0,&sockSet,NULL,NULL,&interval);

		// ��ʱ����
		if (nReadingSock != 1)
		{
			int rc = WSAGetLastError();
			return FALSE;
		}

		// ���ջ������е�����
		status = recv(sock,buffer+nReceived,length-nReceived,0);

		// �������ݳ���
		if (status <= 0)
		{
			int rc = WSAGetLastError();
			return FALSE;
		}

		// ���ӽ��յ����ֽ���
		nReceived += status;

		// ������յ�ȫ���������򷵻�
		if (nReceived >= length)
			return TRUE;
	}
}


int ReceiveBuffer2(SOCKET sock,char *buffer,long length, int nSecond, int nMiliSec, int& nRecvBytes)
{
	int status;					// ͨ�Ų����Ľ��
	long nReceived;				// ���յ����ֽ���
	fd_set sockSet;				// ����Socket����
	int nReadingSock;			// ������Ϣ�Ŀͻ���
	struct timeval interval;	// �ȴ���ʱ����
	
	// �ϴν��ܵ�һ�������
	nReceived = nRecvBytes;
	if (nReceived >= length)
		return 1;

	while (TRUE)
	{
		// �����ջ��������Ƿ�������
		FD_ZERO(&sockSet);
		FD_SET(sock,&sockSet);
		interval.tv_sec = nSecond;
		interval.tv_usec = nMiliSec*1000;
		nReadingSock = select(0, &sockSet, NULL, NULL, &interval);
		
		// ��ʱ����
		if (nReadingSock != 1)
		{	
			nRecvBytes = nReceived;
			return 0;
		}
		
		// ���ջ������е�����
		status = recv(sock, buffer+nReceived, length-nReceived, 0);
		if (status <= 0)
		{
			// �������ݳ���
			int rc = WSAGetLastError();
			return -1;
		}
		
		// ���ӽ��յ����ֽ���
		nReceived += status;

		// ������յ�ȫ���������򷵻�
		if (nReceived >= length)
		{
			nRecvBytes = 0;	// �������
			return 1;
		}
	}

}

// ReceiveBuffer3
int ReceiveBuffer3(SOCKET sock, char *buffer, long length, int nSecond, int nMiliSec, int& nRecvBytes)
{
	int status;					// ͨ�Ų����Ľ��
	long nReceived;				// ���յ����ֽ���
	fd_set sockSet;				// ����Socket����
	int nReadingSock;			// ������Ϣ�Ŀͻ���
	struct timeval interval;	// �ȴ���ʱ����
	
	DWORD iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);
	
	if (length<=0)
	{
		return 2;
	}

	// �ϴν��ܵ�һ�������
	nReceived = nRecvBytes;
	if (nReceived >= length)
		return 1;
	
	while (TRUE)
	{
		// �����ջ��������Ƿ�������
		FD_ZERO(&sockSet);
		FD_SET(sock,&sockSet);
		interval.tv_sec = nSecond;
		interval.tv_usec = nMiliSec*1000;
		nReadingSock = select(0, &sockSet, NULL, NULL, &interval);
		if (nReadingSock != 1)
		{	
			nRecvBytes = nReceived;
			return 0;
		}
		
		status = recv(sock, buffer+nReceived, length-nReceived, 0);
		if (status <= 0)
		{
			// �������ݳ���
			int rc = WSAGetLastError();
			return -1;
		}
		
		// ���ӽ��յ����ֽ���
		nReceived += status;
		
		// ������յ�ȫ���������򷵻�
		if (nReceived >= length)
		{
			nRecvBytes = nReceived;
			return 1;
		}
	}

}

int ReceiveHttpHead(SOCKET sock,char *buffer,long length, int nSecond)
{
	int status;					// ͨ�Ų����Ľ��
	short nReceived;			// ���յ����ֽ���
	fd_set sockSet;				// ����Socket����
	int nReadingSock;			// ������Ϣ�Ŀͻ���
	struct timeval interval;	// �ȴ���ʱ����
	int nWaitTime = 0;
	DWORD dwTick = GetTickCount();
	
	nReceived = 0;
	while (TRUE)
	{
		// �����ջ��������Ƿ�������
		FD_ZERO(&sockSet);
		FD_SET(sock,&sockSet);
		interval.tv_sec = 1;
		interval.tv_usec = 0;
		nReadingSock = select(0,&sockSet,NULL,NULL,&interval);
		
		// ��ʱ����
		if (nReadingSock != 1)
		{	
			if (GetTickCount()-dwTick > nSecond*1000)
				return 0;
			continue;
		}
		
		// ���ջ������е�����
		status = recv(sock,buffer+nReceived,1,0);
		
		// �������ݳ���
		if (status <= 0)
		{	
			int rc = WSAGetLastError();
			return -1;
		}	
		
		// ���ӽ��յ����ֽ���
		nReceived += status;
		
		// ������յ�ȫ���������򷵻�
		if (nReceived >= length)
			return nReceived;
		if (strstr(buffer, "\r\n\r\n") != 0)
			return nReceived;
	}
}

