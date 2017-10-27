#pragma once

struct SMarketInfo
{
    string m_strIp;
    unsigned short m_shtPort; 
    int m_intFlag;
    SMarketInfo()
        :m_shtPort(0)
        ,m_intFlag(0)
    {
    }

    bool operator == (const SMarketInfo & info)const
    {
        return m_strIp == info.m_strIp
            && m_shtPort == info.m_shtPort
            && m_intFlag == info.m_intFlag;
    }
};

class CMarketData: public CTransCtrlCon, public CMarketDataBase
{
public:
    bool m_blnIsConnect;
    static CTimeSpan    s_startspan;
private:
    CTime m_starttime;
    SMarketInfo m_info;
    SMarketInfo m_initinfo;
public:
    CMarketData(const char * pMarketID)
        :CMarketDataBase(pMarketID)
        ,m_blnIsConnect(false)
    {
        m_starttime = CTime::GetCurrentTime();
    }

    bool IsActive(const SMarketTime & data);
    void InitConnect(const string & strIp, const unsigned short shtPort, const int intFlag, const bool blnBackup = false);
    virtual void NewPacketNotify(const char * pPacket, const int intLen);
    virtual void ConStatusNotify(const bool blnIsConnect); 
    virtual void SendUpPacket(const char * pData, const int intSize); 
    SMarketInfo GetInfo();
    void SetInitInfo(){m_initinfo = m_info;}
    void Resume();
};
