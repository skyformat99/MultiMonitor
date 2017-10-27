#include "stdafx.h"
#include "transctrlcon.h"
#include "mdsmulticonn.h"

template<typename Tp>
int OutFromBuf(Tp & data, const char * pBuf)
{
    memcpy(&data, pBuf, sizeof(data));
    return sizeof(data);
}

inline int OutStrFromBuf(std::string& data, const char* pBuf)
{
	int read_size = 0;
	unsigned short string_size = 0;
	read_size += OutFromBuf(string_size, pBuf);
	if (string_size != 0) {
		vector <char> tmp;
		tmp.insert(tmp.end(), pBuf + read_size, pBuf + read_size + string_size);
		read_size += string_size;
		tmp.push_back(0);
		data = &*tmp.begin();
	}
	return read_size;
}

CMDSMultiConn::CMDSMultiConn(void)
	: connected_(false),
	request_mds_versions_(true),
	m_intPackNo(0),
	connection_id_(0),
	post_message_(true) {
}

CMDSMultiConn::~CMDSMultiConn(void)
{
}

void CMDSMultiConn::InitConnect(const string & strIp, const unsigned short shtPort, HWND hWnd, unsigned short connection_id)
{
	Log(CLog::Info, "CMDSMultiConn", "InitConnect", "Begin initialize the connection.");
    Connect(strIp.c_str(), shtPort);
	connected_ = true;
    m_strID =  strIp;
    m_repKey = strIp;
    m_hWnd = hWnd;
	connection_id_ = connection_id;
	Log(CLog::Info, "CMDSMultiConn", "InitConnect", "End initialize the connection.");
}


void CMDSMultiConn::NewPacketNotify(const char * pPacket, const int intLen)
{
	Log(CLog::Info, "CMDSMultiConn", "NewPacketNotify", "Begin NewPacketNotify.");
	SPackHead head;
	CPack::UnpackHead(&head, pPacket, CPack::_const_pack_head_size);

	switch (head.m_shtType) {
		case PACK_PUSH_PC_SHAKEHAND: {
			std::lock_guard<std::mutex> lock(basic_info_mutex_);
			int message_number = 0;
			const char* pPos = pPacket;
			pPos += CPack::_const_pack_head_size;
			pPos += OutFromBuf(message_number, pPos);
			for (int i = 0; i < message_number; i++) {
				MDSBasicInfo notify;
				pPos += OutFromBuf(notify.host_info, pPos);
				pPos += OutFromBuf(notify.status, pPos);
				pPos += OutFromBuf(notify.port, pPos);
				if (m_strID == "172.16.56.39")  // Comment out later.
					pPos += OutStrFromBuf(notify.server_name, pPos);
				std::vector<MDSBasicInfo>::iterator it = std::find(basic_info_.begin(), basic_info_.end(), notify);
				if (it == basic_info_.end()) {
					basic_info_.push_back(notify);
				} else {
					if ((*it).status != notify.status) {
						*it = notify;
						post_message_ = true;
					}
				}
			}
			if (post_message_) {
				PostMessage(m_hWnd, WM_NOTIFYMDSLISTUPDATE, (WPARAM)true, (LPARAM)connection_id_);
				post_message_ = false;
			}
			break;
		} case PACK_PUSH_PC_MDSVERSION: {
			std::lock_guard<std::mutex> lock(mds_versions_mutex_);
			int tsize = 0;
			const char* pPos = pPacket;
			pPos += CPack::_const_pack_head_size;
			pPos += OutFromBuf(tsize, pPos);
			for (int i = 0; i < tsize; i++) {
				MsgMDSVersion msg;
				pPos += OutFromBuf(msg.host_info, pPos);
				pPos += OutStrFromBuf(msg.mds_all_version, pPos);
				std::vector <MsgMDSVersion>::iterator it = std::find(mds_versions_.begin(), mds_versions_.end(), msg);
				if (it == mds_versions_.end()) {
					mds_versions_.push_back(std::move(msg));
				} else {
					*it = std::move(msg);
				}
			}
			PostMessage(m_hWnd, WM_NOTIFYMDSVERSIONS, (WPARAM)true, (LPARAM)connection_id_);
			request_mds_versions_ = false;
			break;
		} default: {
			break;
		}
	}
	Log(CLog::Info, "CMDSMultiConn", "NewPacketNotify", "End NewPacketNotify.");
}

void CMDSMultiConn::ConStatusNotify(const bool connected)
{
	Log(CLog::Info, "CMDSMultiConn", "ConStatusNotify", "Begin ConStatusNotify.");
    if (connected) {
		requestPacket(PACK_PUSH_PC_SHAKEHAND);
	  if (request_mds_versions_)
		  requestPacket(PACK_PUSH_PC_MDSVERSION);
    }
  	Log(CLog::Info, "CMDSMultiConn", "ConStatusNotify", "End ConStatusNotify.");
}

void CMDSMultiConn::requestPacket(int requestType) {
	std::vector<char> buf;
	buf.resize(CPack::_const_pack_head_size);
    int intHeadSize = CPack::_const_pack_head_size;
	CPack::PackHead(&*buf.begin(), intHeadSize, buf.size(), m_intPackNo++, 100, requestType);
    SendPacket(&*buf.begin(), buf.size());
}

