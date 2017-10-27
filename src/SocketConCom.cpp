#include "StdAfx.h"
#include "socketconcom.h"
#include "pack.h"
#include "comfun.h"

int CSocketConCom::_const_max_send_buf_size = 10*1024*1024;
int CSocketConCom::_const_shake_hand_interval = 2;

CSocketConCom::CSocketConCom(const int intConID, const SOCKET & so, const char * pIp, const unsigned short shtClientPort, const unsigned short shtMod)
:m_socket(so)
,m_blnIsClose(false)
,m_intConID(intConID)
,m_blnPause(false)
,m_strIp(pIp)
,m_shtPort(shtClientPort)
,m_blnRecvShakeHand(true)
,_const_shake_hand_pack_type(76)
,_const_shake_hand_mod(shtMod)
{
    m_refreshTime = CTime::GetCurrentTime();
	m_bIsInit = false;
}

CSocketConCom::~CSocketConCom()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
		Log(CLog::Info, "CSocketConCom", "~CSocketConCom", "Close the socket");
    }
}

int CSocketConCom::GetSendBufSize()
{
    int intSize = 0;
    for (list<vector<char> >::iterator it = m_sendbuf.begin(); it != m_sendbuf.end(); it++)
    {
        intSize += it->size();
    }

    return intSize;
}

void CSocketConCom::GetShakeHandPacket(vector<char> & vectData)
{
    vectData.resize(CPack::_const_pack_head_size + sizeof(int) * 2);
    int intLen = vectData.size();
    CPack::Pack2(&*vectData.begin(), intLen, 0, 100, _const_shake_hand_pack_type, 0, 0);
}

void CSocketConCom::Run()
{
	int index = 0;
    for (;GetStopFlag() == false;)
    {
        fd_set fsRead, fsWrite, fsExcept;
        timeval tv;

        FD_ZERO(&fsRead);
        FD_ZERO(&fsWrite);
        FD_ZERO(&fsExcept);

        FD_SET(m_socket, &fsRead);
        {
            CAutoLock lock(m_MutexSend);
            if (m_sendbuf.size() > 0)
            {
                FD_SET(m_socket, &fsWrite);
            }
        }
        //FD_SET(m_socket, &fsExcept);

        tv.tv_sec = 0;
        tv.tv_usec = 1000;
        int nRet = select(0, &fsRead, &fsWrite, &fsExcept, &tv);
        bool blnRecvData = false;
        if (nRet>0)
        {
            Log(CLog::Info, "CSocketConCom", "Run", "Socket is active.{id=%d, socket=%d, ip=%s, port=%d}"
            , m_intConID, m_socket, m_strIp.c_str(), m_shtPort);
            if (FD_ISSET(m_socket, &fsRead)) 
            {
				index++;
                CAutoLock lock(m_MutexRecv);
                int intOldSize = m_recvbuf.size();
                const int _const_len = 1024*1024;
                m_recvbuf.resize(intOldSize+_const_len);
                char * pPos = &*(m_recvbuf.begin() + intOldSize);
                int intRecvLen = recv(m_socket, pPos, _const_len, 0);
                if (intRecvLen <= 0)
                {
                    Log(CLog::Error, "CSocketConCom", "Run", "intRecvLen <= 0{id=%d, socket=%d, ip=%s, port=%d}"
                        , m_intConID, m_socket, m_strIp.c_str(), m_shtPort);
                    break;
                }
                else if (intRecvLen > 0)
                {
                    //Log(CLog::Info, "CSocketConCom", "Run", "Recv socket data{id=%d, socket=%d, ip=%s, port=%d, size=%d}"
                    // , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, intRecvLen);
                    blnRecvData = true;
                    m_recvbuf.resize(intOldSize+intRecvLen);
                }
                
            }

            if (FD_ISSET(m_socket, &fsWrite))
            {
                try
                {
                    CAutoLock lock(m_MutexSend);
                    if (m_sendbuf.size() > 0)
                    {
                        while(m_sendbuf.size())
                        {
                            if (m_sendbuf.front().size() == 0)
                            {
                                m_sendbuf.erase(m_sendbuf.begin());
                                Log(CLog::Error, "CSocketConCom", "Run", "Remove empty packet");
                            }
                            else
                            {
								Log(CLog::Error, "CSocketConCom", "Run", "break empty packet.");
                                break;
                            }
                        }

                        vector<char> & tmp = m_sendbuf.front();
                        int intBufLen = tmp.size();
                        int intSendLen = send(m_socket, &*tmp.begin(), intBufLen, 0);
                        if (intSendLen > 0)
                        { 
                            Log(CLog::Info, "CSocketConCom", "Run", "Send socket data{id=%d, socket=%d, ip=%s, port=%d, size=%d}"
                             , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, intSendLen); 
                            if (intBufLen == intSendLen)
                            {
                                m_sendbuf.erase(m_sendbuf.begin());
							    Log(CLog::Error, "CSocketConCom", "Run", "buf len is equal to send len,erase buf");
                            }
                            else
                            {
                                tmp.erase(tmp.begin(), tmp.begin() + intSendLen);
								Log(CLog::Error, "CSocketConCom", "Run", "erase buf");
                            }
                        }
                    }
                }
                catch(...)
                {
                    Log(CLog::Error, "CSocketConCom", "Run", "Unknown exception, connection break!");
                    break;
                }
                
            }
        }

        vector<char> vectRecvShakeHand;
        if (blnRecvData)
        {
            CAutoLock lock(m_MutexRecv);
            bool blnIsOk = true;
            while(m_recvbuf.size() >= CPack::_const_pack_head_size)
            {
                char * pPos = &*m_recvbuf.begin();
                unsigned int intPackLen = 0;
                CPack::Input(pPos, intPackLen);
                unsigned short shtPackType = 0;
                CPack::Input(pPos + 10,shtPackType);
                if (intPackLen < CPack::_const_pack_head_size)
                {
                    Log(CLog::Error, "CSocketConCom", "Run", "intPackLen < CPack::_const_pack_head_size{intPackLen=%d}", intPackLen);
                    blnIsOk = false;
                    break;
                }
                else if (intPackLen <= m_recvbuf.size())
                {
                    if (_const_shake_hand_pack_type == shtPackType)
                    {
                        vectRecvShakeHand.insert(vectRecvShakeHand.end(), m_recvbuf.begin(), m_recvbuf.begin() + intPackLen);
                        m_recvbuf.erase(m_recvbuf.begin(), m_recvbuf.begin() + intPackLen);
                        m_blnRecvShakeHand = true;
                        Log(CLog::Status, "CSocketConCom", "Run", "Recv shake hand{id=%d, socket=%d, ip=%s, port=%d, interval=%d}"
                            , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, _const_shake_hand_interval); 
                        break;
                    }
                    else
                    {
						Log(CLog::Info, "CSocketConCom", "Run", "new packet notify,packet len=%d,m_intConID=%d,index=%d.",intPackLen,m_intConID,index); 
                        NewPacketNotify(m_intConID, &*m_recvbuf.begin(), intPackLen);
                        m_recvbuf.erase(m_recvbuf.begin(), m_recvbuf.begin() + intPackLen);
                    }
                }
                else
                {
					Log(CLog::Info, "CSocketConCom", "Run", "break"); 
                    break;
                }
            }

            if (blnIsOk == false)
            {
				Log(CLog::Info, "CSocketConCom", "Run", "blnIsOk break."); 
                break;
            }
        }

		if(m_bIsInit)
		{
			m_refreshTime = CTime::GetCurrentTime();
			m_bIsInit = true;
		}
        if (vectRecvShakeHand.size() && SHAKEHAND_RECV == _const_shake_hand_mod)
        {
            Log(CLog::Status, "CSocketConCom", "Run", "Send shake hand{id=%d, socket=%d, ip=%s, port=%d, interval=%d}"
                , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, _const_shake_hand_interval);   
            SendPacket(&*vectRecvShakeHand.begin(), vectRecvShakeHand.size()); 
            m_refreshTime = CTime::GetCurrentTime();
        }

        if (SHAKEHAND_RECV == _const_shake_hand_mod && _const_shake_hand_interval > 0)
        {
            CTimeSpan shakeHandSpan(0,0,0,_const_shake_hand_interval * 100);
            if (CTime::GetCurrentTime() - m_refreshTime > shakeHandSpan)
            {
                Log(CLog::Error, "CSocketConCom", "Run", "Shake hand recv timeout{id=%d, socket=%d, ip=%s, port=%d, interval=%d}"
                    , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, _const_shake_hand_interval);    
               
				closesocket(m_socket);
				m_blnIsClose = true;
				return;
            }
        }
        
        if (SHAKEHAND_SEND == _const_shake_hand_mod && _const_shake_hand_interval > 0)
        {
            CTimeSpan shakeHandSpan(0,0,0,_const_shake_hand_interval);
            if (CTime::GetCurrentTime() - m_refreshTime > shakeHandSpan && !m_blnRecvShakeHand)
            {
                Log(CLog::Error, "CSocketConCom", "Run", "Shake hand send timeout{id=%d, socket=%d, ip=%s, port=%d, interval=%d}"
                    , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, _const_shake_hand_interval);    
                
				break;
            }
            
            if (CTime::GetCurrentTime() - m_refreshTime > shakeHandSpan)
            {
                vector<char> vectBuf;
                GetShakeHandPacket(vectBuf);
                SendPacket(&*vectBuf.begin(), vectBuf.size());
                m_refreshTime = CTime::GetCurrentTime();
                m_blnRecvShakeHand = false;
                Log(CLog::Status, "CSocketConCom", "Run", "Send shake hand{id=%d, socket=%d, ip=%s, port=%d, interval=%d}"
                    , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, _const_shake_hand_interval);     
            }
        }
        
        {
            CAutoLock lock(m_MutexSend);
            if (m_blnPause)
            {
                if (GetSendBufSize() <= _const_max_send_buf_size)
                {
                    Log(CLog::Warn, "CSocketConCom", "Run", "Send buffer is not full{id=%d, socket=%d, ip=%s, port=%d, size=%d}"
                        , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, GetSendBufSize());  
                    Resume(m_intConID);
                    m_blnPause = false;
                }
            }
            else
            {
                if (GetSendBufSize() > _const_max_send_buf_size)
                {
                    Log(CLog::Warn, "CSocketConCom", "Run", "Send buffer full{id=%d, socket=%d, ip=%s, port=%d, size=%d}"
                        , m_intConID, m_socket, m_strIp.c_str(), m_shtPort, GetSendBufSize());
                    Pause(m_intConID);
                    m_blnPause = true;
                }
            }
        }
    }

    Log(CLog::Warn, "CSocketConCom", "Run", "Thread will be close");
    closesocket(m_socket);
    m_blnIsClose = true;
    Log(CLog::Warn, "CSocketConCom", "Run", "Thread end");
}

void CSocketConCom::SendPacket(const char * pPacket, const int intLen)
{
	Log(CLog::Info, "CSocketConCom", "SendPacket", "Begin Sending packet.");
    if (intLen == 0 || pPacket == NULL)
    {
        Log(CLog::Error, "CSocketConCom", "SendPacket", "Send null packet");
        return;
    }
  
    CAutoLock lock(m_MutexSend);
    const int _const_buf_size = 1024 * 1024;
    if (m_sendbuf.size() > 0)
    {
        vector<char> & tmp = m_sendbuf.back();
        if (tmp.size() + intLen > _const_buf_size)
        {
            vector<char> tmp2(pPacket, pPacket + intLen);
            m_sendbuf.push_back(tmp2);
        }
        else
        {
            tmp.insert(tmp.end(), pPacket, pPacket + intLen);
        }
    }
    else
    {
        vector<char> tmp2(pPacket, pPacket + intLen);
        m_sendbuf.push_back(tmp2);
    }

	Log(CLog::Info, "CSocketConCom", "SendPacket", "End Sending packet.");
}
