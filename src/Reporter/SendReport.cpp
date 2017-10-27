#include "SendReport.h"
#include <afx.h>
#include <process.h>
#include "SocketUtil.h"
#include <string>
#include <iostream>
#include <algorithm>

void CStatusReporter::SetMaxMsgNum(const int Num){
	m_MaxMsgNum = Num;
}
void CStatusReporter::Send(const string& Msg){
	EnterCriticalSection(&m_ghLock);
	if( m_arrMsg.size() >= m_MaxMsgNum )// 如果大于m_MaxMsgNum，则退出第一条，放入最后一条
		m_arrMsg.pop();
	m_arrMsg.push(Msg);
	LeaveCriticalSection(&m_ghLock);
	if (!ReleaseSemaphore(
		m_ghSemaphore, // handle to semaphore - hSemaphore是要增加的信号量句柄
		1, // increase count by one - lReleaseCount是增加的计数
		NULL) ) // not interested in previous count - lpPreviousCount是增加前的数值返回
	{
		printf("ReleaseSemaphore error: %d\n", GetLastError());
	}
	//printf("add Msg OK");
};
void CStatusReporter::Start(const char* ServerIP, const unsigned short ServerPort){
	if( !m_bInitOk ){
		printf("Init failed!\n");
		return;
	}
	m_Ip = string(ServerIP);
	m_Port = ServerPort;
	
	hReportThread = (HANDLE)_beginthread((void (__cdecl *) (void *))DoSendReport, 1, (void*)this);
};

CStatusReporter::~CStatusReporter(){
	m_bBreak = true;
	DeleteCriticalSection(&m_ghLock);
	if(m_ghSemaphore)	
		CloseHandle(m_ghSemaphore);
};
CStatusReporter::CStatusReporter(){	
	_Init();
};

void CStatusReporter::Stop(){
	m_bBreak = true;
	EnterCriticalSection(&m_ghLock);
	while( m_arrMsg.size() > 0)
		m_arrMsg.pop();
	LeaveCriticalSection(&m_ghLock);
};

void CStatusReporter::_Init()
{	
	m_MaxMsgNum = 100;
	m_bBreak = false;
	m_bInitOk = false;
	m_bSendSMS = true;

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if( ret != 0 ){
		printf("WSAStartup error: %d\n", GetLastError());
		return;
	}
	
	InitializeCriticalSection(&m_ghLock);
	
	m_ghSemaphore = CreateSemaphore(
		NULL, // default security attributes - lpSemaphoreAttributes是信号量的安全属性
		0, // initial count - lInitialCount是初始化的信号量
		m_MaxMsgNum, // maximum count - lMaximumCount是允许信号量增加到最大值
		NULL); // unnamed semaphore - lpName是信号量的名称
	if (m_ghSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		return;
	}

	m_bInitOk = true;
};

int CStatusReporter::DoSendReport(void *param)
{
	CStatusReporter *sr = (CStatusReporter*)param;
	return sr->Run();
};

bool CStatusReporter::ChangeServer(const char* ServerIP, const unsigned short ServerPort)
{
	m_bBreak = true;
	WaitForSingleObject(hReportThread, INFINITE);
	m_bBreak = false;
	Start(ServerIP, ServerPort);
	return true;
}

int CStatusReporter::Run()
{
	SOCKET hSock		= NULL;

	while(1){
		if( m_bBreak ){
			shutdown(hSock, SD_BOTH);
			closesocket(hSock);
			hSock = NULL;
			return 0;
		}
		if( hSock == NULL){
			if (Connect(hSock, (char*)m_Ip.c_str(), m_Port) == FALSE)
			{	
				hSock = NULL;
				Sleep(10*1000);// 重连中止10秒
			}
		}
		else{
			WaitForSingleObject(m_ghSemaphore, 10000);	
			EnterCriticalSection(&m_ghLock);
			int l = m_arrMsg.size();
			if( l <= 0 ){
				LeaveCriticalSection(&m_ghLock);
				//printf("CStatusReporter :: No Data\n");
				continue;
			}
			else {
				string sMsg = "";
				while(m_arrMsg.size() > 0 ){// 获取所有发送
					sMsg += m_arrMsg.front();
					m_arrMsg.pop();
				} 
				LeaveCriticalSection(&m_ghLock);

				if (SendBuffer(hSock, sMsg.c_str(), sMsg.length()) == FALSE)
				{
					//printf("send error\n");
					// 重连
					shutdown(hSock, SD_BOTH);
					closesocket(hSock);
					hSock = NULL;
					continue;
					//SysLogCache.Put("Send Msg to ReportServer Failed");
				}
				//printf("Send OK: %s\n", Msg.c_str());
			}
		}
	}
	return true;
};


