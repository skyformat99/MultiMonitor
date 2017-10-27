#include "stdafx.h"

template<typename Tp>
int OutFromBuf(Tp & data, const char * pBuf)
{
	memcpy(&data, pBuf, sizeof(data));
	return sizeof(data);
}

template<typename Tp>
void AddToBuf(const Tp & data, vector<char> & vectPack, const bool blnBegin = false)
{
	char * pBuf = (char *)&data;
	if(blnBegin)
	{
		vectPack.insert(vectPack.begin(), pBuf, pBuf + sizeof(Tp));
	}
	else
	{
		vectPack.insert(vectPack.end(), pBuf, pBuf + sizeof(Tp));
	}
}



CDSConn::~CDSConn(void)
{
}

CDSConn::CDSConn(void)
	:m_blnIsSubTrans(false)
	,m_blnIsConnect(false)
	,m_intPackNo(0)
	,m_intSubTrans(0)
{

}
void CDSConn::InitConnect(const string & strIp, const int shtPort, HWND hWnd,const vector<string>& vm,const string& fServer)
{
	Log(CLog::Info, "CDSConn", "InitConnect", "Begin initialize the connection.");
	Connect(strIp.c_str(), shtPort);
	m_blnIsConnect = true;
	m_strID =  strIp;
	m_repKey = strIp;
	m_fServer = fServer;
	m_port = shtPort;
	m_hWnd = hWnd;
	m_mapMarkets = vm;

	Log(CLog::Info, "CDSConn", "InitConnect", "End initialize the connection.");
}

const string CDSConn::GetServerName()
{
	return m_fServer;
}

void CDSConn::NewPacketNotify(const char * pPacket, const int intLen)
{
	Log(CLog::Info, "CDSConn", "NewPacketNotify", "Begin NewPacketNotify.");
	CAutoLock lock(m_lock);
	int tsize = 0;
	SPackHead head;
	CPack::UnpackHead(&head, pPacket, CPack::_const_pack_head_size);
	const char* pPos = pPacket;
	pPos += CPack::_const_pack_head_size;

	switch (head.m_shtType)
	{
	case PACK_VSS_MARKETDATA:
		{
			char market[16] = {0};
			char flag = 0;
			pPos += OutFromBuf(market,pPos);
			pPos += OutFromBuf(flag,pPos);
			unsigned int dwcount = 0;
			pPos += OutFromBuf(dwcount,pPos);
			mapMarkets::iterator it = m_data.find(market);
			map<string, SStdQt> mq;
			if(it != m_data.end())
			{
				mq = (*it).second;
				for( size_t i = 0 ; i < dwcount; i++)
				{
					SStdQt qt;
					pPos += OutFromBuf(qt,pPos);
					map<string, SStdQt>::iterator q = mq.find(string(qt.m_pchCode));
					mq[string(qt.m_pchCode)] = qt;
				}
			}
			else
			{
				for( size_t i = 0 ; i < dwcount; i++)
				{
					SStdQt qt;
					pPos += OutFromBuf(qt,pPos);
					map<string, SStdQt>::iterator q = mq.find(string(qt.m_pchCode));
					mq[string(qt.m_pchCode)] = qt;
				}
			}
			m_data[string(market)] = mq;
			
			PostMessage(m_hWnd, WM_NOTIFYDSLISTUPDATE,(WPARAM)&m_data, NULL);
		}

	}
	Log(CLog::Info, "CDSConn", "NewPacketNotify", "End NewPacketNotify.");
}

void CDSConn::ConStatusNotify(const bool blnIsConnect)
{
	Log(CLog::Info, "CDSConn", "ConStatusNotify", "Begin ConStatusNotify.");
	if (blnIsConnect)
	{
		int vmSize = m_mapMarkets.size();
		for(int i = 0; i < vmSize ; i++)
		{
			vector<char> buf;
			string market = m_mapMarkets[i];
			PackReq2(buf,market); 
		} 
	}
	Log(CLog::Info, "CDSConn", "ConStatusNotify", "End ConStatusNotify.");
}


void CDSConn::PackReq2(vector<char> & buf,const string& market)
{
	Log(CLog::Info, "CDSConn", "PackReq2", "Begin PackReq2.");
	CAutoLock lock(m_lock);
	//buf.resize(CPack::_const_pack_head_size+MARKET_CODE_LEN);
	char pMarket[MARKET_CODE_LEN];
	memset(pMarket, 0, sizeof(pMarket));
	strncpy(pMarket,market.c_str() , market.size());
	AddToBuf(pMarket,buf);
	char head[12] = {0};
	int intHeadSize = CPack::_const_pack_head_size;
	CPack::PackHead(head, intHeadSize, buf.size()+12, m_intPackNo++, 100, PACK_VSS_MARKETDATA);
	AddToBuf(head, buf, true);
	SendPacket(&*buf.begin(), buf.size());
	Log(CLog::Info, "CDSConn", "PackReq2", "End PackReq2.");
}
