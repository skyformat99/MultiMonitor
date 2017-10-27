#pragma once
#include <mutex>

#pragma pack(push, 1)
enum MDSSTATUS {
	MDS_RES_OK = 0,
	MDS_RES_NOK = 1
};

typedef struct SPcReqHead
{
    int    m_nPkgSize; 
    short  m_nMsgId;    
    int    m_nOwnerId;  
    int    m_nMagicId;  
    SPcReqHead()
    {
        memset(this, 0, sizeof(SPcReqHead));
    }
}SPcReqHead;

typedef struct SPcRetHead
{
	int    m_nPkgSize;   
	short  m_nMsgId;   
	int    m_nOwnerId;  
	char   m_bResult;
	char   m_bEncrypt;
	char   m_bCompressed;
	int    m_nMagicId;	
    SPcRetHead()
    {
        memset(this, 0, sizeof(SPcRetHead));
    }
}SPcRetHead;

typedef struct SPcStkShakeReq
{
	SPcReqHead head;
}SPcStkShakeReq;

typedef struct SPcStkShakeRet
{
	SPcRetHead head;

}SPcStkShakeRet;

struct MDSBasicInfo {
	char		host_info[28];
	MDSSTATUS	status;
	int			port;
	std::string server_name;
	MDSBasicInfo() {
		memset(host_info, 0, sizeof host_info);
		status = MDSSTATUS::MDS_RES_NOK;
		port = 0;
	}

	bool operator==(const MDSBasicInfo& c) {
		return !strcmp(this->host_info, c.host_info);
	}

	MDSBasicInfo& operator=(const MDSBasicInfo& c) {
		if (this == &c)
			return *this;

		strcpy(host_info, c.host_info);
		status = c.status;
		port = c.port;
		server_name = c.server_name;
		return *this;
	}
};

struct MsgMDSVersion {
	char            host_info[28];
	std::string     mds_all_version;
	MsgMDSVersion() {
		memset(host_info, 0, sizeof(host_info));
		mds_all_version = "Not Received";
	}

	bool operator == (const MsgMDSVersion &c) {
		return !strcmp(this->host_info, c.host_info);
	}

	MsgMDSVersion& operator=(const MsgMDSVersion& c) {
		if (this == &c)
			return *this;

		strcpy(host_info, c.host_info);
		mds_all_version = c.mds_all_version;
		return *this;
	}
	MsgMDSVersion& operator=(MsgMDSVersion&& c) {
		strcpy(host_info, c.host_info);
		mds_all_version = std::move(c.mds_all_version);
		return *this;
	}
};

#pragma pack(pop)

class CMDSMultiConn : public CTransCtrlCon {
public:
	CMDSMultiConn(void);
	~CMDSMultiConn(void);
	HWND							m_hWnd;
    int								m_intPackNo;
    string							m_strID;
    string							m_repKey;
protected:
	std::mutex						basic_info_mutex_;
	std::mutex						mds_versions_mutex_;
	bool							connected_;
	bool							post_message_;
	bool							request_mds_versions_;
	std::vector<MDSBasicInfo>		basic_info_;
	std::vector<MsgMDSVersion>		mds_versions_;
	unsigned short					connection_id_; 
public:
    void InitConnect(const string & strIp, const unsigned short shtPort, HWND hWnd, unsigned short connection_id);
    virtual void NewPacketNotify(const char * pPacket, const int intLen);
    virtual void ConStatusNotify(const bool blnIsConnect);
	void requestPacket(int requestType = 0);
	// In multi-thread app, it is not advised to return a shared member's reference.
	void GetBasicInfo(std::vector<MDSBasicInfo>& basic_info) {
		std::lock_guard<std::mutex>	lock(basic_info_mutex_);
		basic_info = basic_info_;
	}
	void GetMDSVersions(std::vector<MsgMDSVersion>& mds_versions) {
		std::lock_guard<std::mutex>	lock(mds_versions_mutex_);
		mds_versions_.swap(mds_versions);
	}
};

