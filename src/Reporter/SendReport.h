#ifndef SENDREPORT_H
#define SENDREPORT_H

#pragma warning(disable:4786)
#include <string>
#include <vector>
#include <queue>
using namespace std;
#include "singleton.h"
#include <afx.h>											// MFC
#include "ReporterInterface.h"


class CStatusReporter : public CSingleton<CStatusReporter> 
{
public:
	void Start(const char* ServerIP, const unsigned short ServerPort);
	void Send(const string& Msg);
	void Stop();
	void SetMaxMsgNum(const int Num);
	void SetSendSMS(const bool bSend){m_bSendSMS = bSend;}
	bool ChangeServer(const char* ServerIP, const unsigned short ServerPort);
	~CStatusReporter();
	string				m_Ip;
	unsigned short		m_Port;
private:
	HANDLE hReportThread;
	static int DoSendReport(void *param); 

	void _Init();
	int Run();

	bool				m_bBreak;
	int					m_MaxMsgNum;
	queue<string>		m_arrMsg;
	CRITICAL_SECTION	m_ghLock;
	HANDLE				m_ghSemaphore;
	bool				m_bInitOk;
	bool				m_bSendSMS;
	
	CStatusReporter();
	friend class CSingleton<CStatusReporter>;
};

#endif