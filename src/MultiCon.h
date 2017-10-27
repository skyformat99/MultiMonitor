#pragma once

enum TransPackType{PACK_GET_DATAID = 1000
    ,PACK_PUSH_MARKETTIME
    ,PACK_PUSH_DICT
    ,PACK_PUSH_READYEVENT

    ,PACK_PUSH_MX = 1100
    ,PACK_PUSH_RTMIN

    ,PACK_PUSH_QT = 1200
    ,PACK_STAT_SUBTRANS = 2000

    ,PACK_PUSH_BLOCKMAP = 8000
    ,PACK_PUSH_MRAWDATA = 8001
};
#pragma pack(push, 1)
class CStdMarketTime
{
public:
    unsigned int m_dwTradeSequence;
    unsigned int m_dwTradeDate;
    unsigned int m_dwTradeTime;
    char   m_cTradeFlag;
    CStdMarketTime()
    {
        memset(this,0,sizeof(CStdMarketTime));
    }
};

class CStdTradePeriod
{
public:
    unsigned int m_dwBegDate;
    unsigned int m_dwBegTime;
    unsigned int m_dwEndDate;
    unsigned int m_dwEndTime;
    CStdTradePeriod()
    {
        memset(this,0,sizeof(CStdMarketTime));
    }
};

typedef struct SConInfo {
	char m_strIp[32];
	unsigned short m_shtPort;
    int m_intFlag;
	SConInfo() : m_shtPort(0), m_intFlag(-1) {
		memset(m_strIp,0,32);
	}

	bool operator == (const SConInfo & info)const {
		return string(m_strIp) == string(info.m_strIp)
				&& m_shtPort == info.m_shtPort;
	}
}SConInfo;

#pragma pack(pop)

struct comp {
    bool operator()(const int& _left, const int& _right) const {
        int* _pleft = NULL;
        int* _pright = NULL;
        int prior[] = {116, 347, 105, 106, 107, 104, 113, 114, 115, 125};
        if ((_pleft = std::find(prior, prior + 8, _left)) != prior + 8)
        {
            if ((_pright = std::find(prior, prior + 8, _right)) != prior + 8)
            {
                return (_pleft < _pright);
            }
            else
            {
                return true;
            }
        }
        else
        {
            if ((_pright = std::find(prior, prior + 8, _right)) != prior + 8)
            {
                return false;
            }
            else
            {
                return (_left < _right);
            }
        }
    }
};

struct SMarketInfo
{
    unsigned short m_wMarketID;
    CStdMarketTime m_time;
    vector<CStdTradePeriod> m_current;
};

typedef map<unsigned short, SMarketInfo, comp> PriorMarketInfoMap;


struct STransDefine
{
    char szName[32];
    unsigned int intIP;
    unsigned short shtTransID;
	SConInfo  qtSvrInfo;
    STransDefine()
    {
        memset(this, 0, sizeof(STransDefine));
    }
	bool operator == (const STransDefine& info) const
	{
		return string(szName) == string(info.szName) && 
			   intIP == info.intIP &&
			   shtTransID == info.shtTransID &&
			   qtSvrInfo == info.qtSvrInfo;
	}
};

struct SMarketItem {
    unsigned short	shtTransID;
    unsigned short	shtMarketID;
    unsigned int	intTradeSeqs;
    unsigned int	intTradeDate;
    unsigned int	intTradeTime;
    SMarketItem() {
        memset(this, 0, sizeof (SMarketItem));
    }
	SMarketItem(unsigned short trans_id, unsigned short market_id, 
		unsigned int trade_sequence, unsigned int trade_date, unsigned int trade_time) {
		shtTransID = trans_id;
		shtMarketID = market_id;
		intTradeSeqs = trade_sequence;
		intTradeDate = trade_date;
		intTradeTime = trade_time;
	}
	bool operator<(const SMarketItem& a) {
		return (((intTradeSeqs < a.intTradeSeqs)
			|| (intTradeSeqs == a.intTradeSeqs && intTradeDate < a.intTradeDate)
			|| (intTradeSeqs == a.intTradeSeqs && intTradeDate == a.intTradeDate && intTradeTime < a.intTradeTime)));
	}
	bool operator==(const SMarketItem& market_item) const {
		return shtMarketID == market_item.shtMarketID &&
			intTradeSeqs == market_item.intTradeSeqs &&
			intTradeDate == market_item.intTradeDate &&
			intTradeTime == market_item.intTradeTime;
	}
};

struct STransStatus
{
    unsigned short shtTransID;
    unsigned short shtSubTrans;
    map<unsigned short, SMarketItem> mapItems;
    STransStatus()
        :shtTransID(0)
        ,shtSubTrans(0)
    {
    }
};

struct trans_status {
	unsigned short trans_id;
	unsigned short sub_trans_number;
	SConInfo upstream_info;
	trans_status() : trans_id(0), sub_trans_number(0) {

	}
	trans_status(unsigned short _trans_id, unsigned short _sub_trans_number, SConInfo _upstream_info) {
		trans_id = _trans_id;
		sub_trans_number = _sub_trans_number;
		upstream_info = _upstream_info;
	}
};

class CMultiCon : public CTransCtrlCon {
private:
    HWND								m_hWnd;
    int									m_intPackNo;
    CMutex								m_lock;
    CMutex								m_lock2;
    PriorMarketInfoMap					m_mapTrans;
    bool								m_blnIsSubTrans;
    bool								m_blnIsConnect;
    string								m_strID;
    string								m_repKey;
    unsigned int						m_intSubTrans;
    map<unsigned short, STransDefine>	m_mapDefs;
	map<unsigned short, STransDefine>	m_mapDefsNotify;
	map<unsigned short, STransStatus>	m_mapData;
    map<unsigned short, SMarketItem>	m_mapStandartTime;
    vector<SMarketItem>					vecUpdateData;
    vector<int>							vecUpdateSubs;

public:
    CMultiCon(void);
    ~CMultiCon(void);
    void InitConnect(const string & strIp, 
					 const unsigned short shtPort, 
					 const string & strNote, 
					 vector<unsigned short> vectMarket, 
					 bool blnIsSubTrans, 
					 HWND hWnd,
					 unsigned short ushtConnectionNo);
    virtual void NewPacketNotify(const char * pPacket, const int intLen);
    virtual void ConStatusNotify(const bool blnIsConnect);
	// Do not return the reference of the shared data because it may cause race conditions in multi-thread code.
    PriorMarketInfoMap GetMarketData() {
        CAutoLock lock(m_lock);
        return m_mapTrans;
    }
    map<unsigned short, SMarketItem> GetStandardTime() {
        CAutoLock lock(m_lock);
        return m_mapStandartTime;
    }
    void GetData(vector<SMarketItem>& vecData) {
        CAutoLock lock(m_lock);
        vecUpdateData.swap(vecData);
    }
    void GetSubs(vector<int>& vecSubs) {
        CAutoLock lock(m_lock);
        vecUpdateSubs.swap(vecSubs);
    }
    map<unsigned short, STransDefine> GetDefs() {
        CAutoLock lock(m_lock);
        return m_mapDefs;
    }
    unsigned int GetSubTrans() {
        CAutoLock lock(m_lock2);
        return m_intSubTrans;
    }
    string GetID() { return m_strID; }
	unsigned short GetConnnectionID() { return connection_id_; }
private:
    void requestPacket(int requestType = 0);
	void uncompress_trans_status(const char* packet_content, const int content_size);
// The unique ID for each connection.
protected:
	unsigned short	connection_id_;
	unsigned short  trans_id_offset;
};
