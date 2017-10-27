#pragma once

enum VssPackType{PACK_VSS_MARKETDATA = 1000, PACK_RAW_MARKETDATA = 1100};
#define MARKET_CODE_LEN 16
#define HKINDEX    "HKINDEXF"
#define CNYOFFSTEMP     "CNYOFFS"
#define HKCNYF      "HKCNYF"
#define HKSTOCKF	"HKSTOCKF"
#define HKMETALFS    "HKMETALFS"
#pragma pack(1)

typedef struct DSConData
{
public:
	int		responseFlag;
	string  market;
	map<string, SStdQt> mq;
	DSConData()
	{
		responseFlag = 0;
	}

	SStdQt& operator[](const string& symbol){
		return mq[symbol];
	}

}DSConData;

typedef std::map<string, map<string, SStdQt> > mapMarkets;

#pragma pack()


template<class out_type, class in_value>
out_type convert(const in_value & t)

{
	int prec=numeric_limits<double>::digits10; // 18
	stringstream stream;
	stream.precision(prec);
	stream<<t;//向流中传值

	out_type result;//这里存储转换结果

	stream>>result;//向result中写入值

	return result;

}


class CDSConn :
	public CTransCtrlCon
{
public:
	CDSConn(void);
	~CDSConn(void);


	void InitConnect(const string & strIp, const int shtPort, HWND hWnd,const vector<string>& vm,const string& fServer);
	virtual void NewPacketNotify(const char * pPacket, const int intLen);
	virtual void ConStatusNotify(const bool blnIsConnect);
	void PackReq2(vector<char> & buf,const string& market);
	const mapMarkets GetData()
	{
		CAutoLock lock(m_lock);
		return m_data;
	}

	const string GetServerName();
private:
	HWND m_hWnd;
	int m_intPackNo;
	CMutex m_lock;
	bool m_blnIsSubTrans;
	bool m_blnIsConnect;
	string m_strID;
	string m_repKey;
	string m_fServer;
	int    m_port;
	unsigned int m_intSubTrans;
	vector<int> vecUpdateSubs;
	mapMarkets	m_data;
	map<string, SStdQt> m_mq;
	vector<string> m_mapMarkets;
};

