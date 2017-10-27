#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
using namespace std;
#include "thread.h"
#include "autolock.h"

class CSocketConCom:public CThread
{
private:
 SOCKET m_socket;
 string m_strIp;
 unsigned short m_shtPort;

 CMutex   m_MutexSend;
 list<vector<char> > m_sendbuf;
 CMutex   m_MutexRecv;
 vector<char> m_recvbuf;
 CTime   m_refreshTime;

 bool m_blnPause;
 bool m_blnIsClose;
 bool m_blnRecvShakeHand;
 bool m_bIsInit;
 
 
 int  m_intConID;
 
 const unsigned short _const_shake_hand_mod;
 const unsigned short _const_shake_hand_pack_type;
public:
 static int _const_shake_hand_interval;
 static int _const_max_send_buf_size;
 enum{SHAKEHAND_RECV = 0, SHAKEHAND_SEND};
 CSocketConCom(const int intConID, const SOCKET & so, const char * pIp, const unsigned short shtClientPort, const unsigned short shtMod = SHAKEHAND_RECV);
 ~CSocketConCom();
 virtual void Run();
 void SendPacket(const char * pPacket, const int intLen);
 bool IsClosed(){return m_blnIsClose;}
 int GetSendBufSize();
 virtual void Pause(const int intConID) = 0;
 virtual void Resume(const int intConID) = 0;
 virtual void NewPacketNotify(const int intConID, const char * pPacket, const int intLen) = 0;
private:
 void GetShakeHandPacket(vector<char> & vectData);
};
