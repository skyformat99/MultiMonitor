#include "stdafx.h"
#include "transctrlcon.h"

void CTransCtrlCon::CSocketCon::Pause(const int intConID)
{
	Log(CLog::Info, "CTransCtrlCon", "Pause", "Pause the connection.");
	m_mag.m_packetqueue.Pause(intConID);
}
void CTransCtrlCon::CSocketCon::Resume(const int intConID)
{
	Log(CLog::Info, "CTransCtrlCon", "Resume", "Resume the connection.");
	m_mag.m_packetqueue.Resume(intConID);
}

void CTransCtrlCon::CSocketCon::NewPacketNotify(const int intConID, const char * pPacket, const int intLen)
{
	Log(CLog::Info, "CTransCtrlCon", "NewPacketNotify", "Begin New packet is received.");
	CTransCtrlCon::_Task task;
	task.m_intConID = intConID;
	task.m_packet.insert(task.m_packet.end(), pPacket, pPacket + intLen);
	m_mag.m_packetqueue.AddTask(intConID, task);
	Log(CLog::Info, "CTransCtrlCon", "NewPacketNotify", "End New packet is received.");
}

void CTransCtrlCon::CSocketCon::SocketCloseNotify(const int intConID)
{
	Log(CLog::Info, "CTransCtrlCon", "SocketCloseNotify", "Begin Set socket status to close.");
	m_mag.ConStatusNotify(false);
}

CTransCtrlCon::~CTransCtrlCon()
{
    CloseCon();
    if (m_pCon)
    {
        delete m_pCon;
    }
}

bool CTransCtrlCon::Connect(const char * pIp, const unsigned short shtClientPort)
{
	CAutoLock lock(m_Mutex);
	Log(CLog::Info, "CTransCtrlCon", "Connect", "Begin Connect to server.");
	m_strIp = pIp;
	m_shtPort = shtClientPort;	
	if(m_pCon)
	{
		m_pCon->StopThread();
	}
	
	Log(CLog::Info, "CTransCtrlCon", "Connect", "End Connect to server.");
	return true;
}

void CTransCtrlCon::CloseCon()
{
	Log(CLog::Info, "CTransCtrlCon", "CloseCon", "Begin Close the connection.");
	CAutoLock lock(m_Mutex);
	if(m_pCon)
	{
		m_pCon->StopThread();
	}

	Log(CLog::Info, "CTransCtrlCon", "CloseCon", "End Close the connection.");
}

void CTransCtrlCon::Run()
{
	for(;GetStopFlag() == false;)
	{
		Sleep(50);					// Please wake up as quickly as possible.
		CAutoLock lock(m_Mutex);
		if(m_pCon && m_pCon->IsClosed())
		{
			delete m_pCon;
			m_pCon = NULL;
		}
		
		if(!m_pCon)
		{
			_DoConnet();
		}
	}

	if(m_pCon)
	{
	   m_pCon->StopThread(); 
	}
}

void CTransCtrlCon::SendPacket(const char * pPacket, const int intLen)
{
	Log(CLog::Info, "CTransCtrlCon", "SendPacket", "Begin Sending packet.");
	CAutoLock lock(m_Mutex);
	if(m_pCon)
	{
		m_pCon->SendPacket(pPacket, intLen);
	}
	
	
	Log(CLog::Info, "CTransCtrlCon", "SendPacket", "End Sending packet.");
}

void CTransCtrlCon::StartThread()
{
	Log(CLog::Info, "CTransCtrlCon", "StartThread", "Starting the thread.");
	m_packetqueue.Init(0);
	CThread::StartThread();
}
void CTransCtrlCon::StopThread()
{
	Log(CLog::Info, "CTransCtrlCon", "StopThread", "Stopping the thread.");
	CThread::StopThread();
	m_packetqueue.StopThread();
}

void CTransCtrlCon::_DoConnet()
{
	Log(CLog::Info, "CTransCtrlCon", "DoConnect", "Begin DoConnect the socket function.");
	CAutoLock lock(m_Mutex);
	if(m_strIp.size() == 0)
	{
		return;
	}
	
	SOCKET so;
    so = socket(PF_INET, SOCK_STREAM, 0);
	if (so==INVALID_SOCKET)
	{
		return;
	}

	UINT32 lAddress = inet_addr(m_strIp.c_str());
	SOCKADDR_IN rSockAddr;
	memset(&rSockAddr, 0, sizeof(rSockAddr));
	rSockAddr.sin_family = AF_INET;
	rSockAddr.sin_addr.s_addr = lAddress;
	rSockAddr.sin_port = htons((u_short)m_shtPort);
	if(connect(so, (struct sockaddr *)&rSockAddr, sizeof(rSockAddr)) == 0)
	{
		if(m_pCon)
		{
			delete m_pCon;
		}
		
		unsigned long ulBlockMode = 1;
		//int nResult = ioctlsocket(so, FIONBIO, &ulBlockMode);
		m_pCon = new CSocketCon(so, m_strIp.c_str(), m_shtPort, *this);
		m_pCon->StartThread();
		ConStatusNotify(true);
		Log(CLog::Info, "CTransCtrlCon", "DoConnect", "A new connection has been established.");
	}
	else
	{
		Log(CLog::Info, "CTransCtrlCon", "DoConnect", "close the socket");
		closesocket(so);
	}

	Log(CLog::Info, "CTransCtrlCon", "DoConnect", "End DoConnect the socket function.");
}
