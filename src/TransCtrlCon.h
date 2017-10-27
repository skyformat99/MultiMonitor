#pragma once

enum MultiMonitorType {
    PACK_PUSH_TRANSDEFINE	= 0,
    PACK_PUSH_TRANSSTATUS	= 1,
    PACK_PUSH_PC_SHAKEHAND	= 2,
	PACK_PUSH_PC_MDSVERSION = 4,
	PACK_PUSH_GQSBASICINFO	= 5,
	PACK_PUSH_COMPRESSEDSTATUS = 6
};

class CTransCtrlCon : public CThread
{
public:
	struct _Task
	{
		_Task()
			:m_intConID(0)
		{

		}
		int			m_intConID;
		vector<char> m_packet;
	};

private:
	class CSocketCon:public CSocketConCom
	{
	private:
		CTransCtrlCon & m_mag;
	public:
		CSocketCon(const SOCKET & so, const char * pIp, const unsigned short shtClientPort, CTransCtrlCon & mag)
			:CSocketConCom(0, so, pIp, shtClientPort)
			,m_mag(mag)
		{
			
		}

		virtual void Pause(const int intConID);
		virtual void Resume(const int intConID);
		virtual void NewPacketNotify(const int intConID, const char * pPacket, const int intLen);
		virtual void SocketCloseNotify(const int intConID);		
	};

	class CPacketQueue:public CTaskQueue<int, _Task >
	{
	private:
		CTransCtrlCon & m_mag;
	public:
		CPacketQueue(CTransCtrlCon & mag)
			:m_mag(mag)
		{}
		virtual void DoSglTask(const int & id, const _Task & task)
		{
			if(task.m_packet.size() > 0)
			{
				m_mag.NewPacketNotify(&*task.m_packet.begin(), task.m_packet.size());
			}
		}
	};

	CPacketQueue			m_packetqueue;
	CMutex					m_Mutex;
	string 					m_strIp;
	unsigned short 			m_shtPort;	
	CSocketCon*				m_pCon;
	void Run();
public:
	CTransCtrlCon()
		:m_packetqueue(*this) 
		,m_pCon(NULL)
		,m_shtPort(0)
	{}
	
    virtual ~CTransCtrlCon();
	virtual void NewPacketNotify(const char * pPacket, const int intLen) = 0;
	virtual void ConStatusNotify(const bool blnIsConnect) = 0;
	void SendPacket(const char * pPacket, const int intLen);
	void CloseCon();
	void StartThread();
	void StopThread();
	bool Connect(const char * pIp, const unsigned short shtClientPort);
private:
	void _DoConnet();
};

