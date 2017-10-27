#include "stdafx.h"
#include "GQSConnection.h"

template<typename Tp>
int OutFromBuf(Tp & data, const char * pBuf) {
    memcpy(&data, pBuf, sizeof(data));
    return sizeof(data);
}

inline int OutStrFromBuf(std::string& data, const char* pBuf) {
	int read_size = 0;
	unsigned short string_size = 0;
	read_size += OutFromBuf(string_size, pBuf);
	if (string_size != 0) {
		vector<char> tmp;
		tmp.insert(tmp.end(), pBuf + read_size, pBuf + read_size + string_size);
		read_size += string_size;
		tmp.push_back(0);
		data = &*tmp.begin();
	}
	return read_size;
}


GQSConnection::GQSConnection(void)
	: connected_(false),
	package_no_(0),
	connection_id_(0),
	post_message_(true) {
}

GQSConnection::~GQSConnection(void) {
}

void GQSConnection::InitConnect(const string& server_ip, const unsigned short port, HWND hWnd, unsigned short connection_id) {
	Log(CLog::Info, "GQSConnection", "InitConnect", "Begin initialize the connection.");
    Connect(server_ip.c_str(), port);
	connected_ = true;
    m_hWnd = hWnd;
	connection_id_ = connection_id;
	Log(CLog::Info, "GQSConnection", "InitConnect", "End initialize the connection.");
}

void GQSConnection::NewPacketNotify(const char* packet, const int packet_size) {
	Log(CLog::Info, "GQSConnection", "NewPacketNotify", "Begin NewPacketNotify.");
	SPackHead head;
	CPack::UnpackHead(&head, packet, CPack::_const_pack_head_size);

	switch (head.m_shtType) {
		case PACK_PUSH_GQSBASICINFO: {
			std::lock_guard<std::mutex> lock(basic_info_mutex_);
			int message_number = 0;
			const char* pPos = packet;
			pPos += CPack::_const_pack_head_size;
			pPos += OutFromBuf(message_number, pPos);
			for (int i = 0; i < message_number; i++) {
				GQSServerInfo notify;
				pPos += OutStrFromBuf(notify.server_name, pPos);
				pPos += OutStrFromBuf(notify.server_ip, pPos);
				pPos += OutFromBuf(notify.port, pPos);
				pPos += OutFromBuf(notify.connected, pPos);
				auto it = std::find(basic_info_.begin(), basic_info_.end(), notify);
				if (it == basic_info_.end()) {
					basic_info_.push_back(notify);
				} else {
					if ((*it).connected != notify.connected) {
						*it = notify;
						post_message_ = true;
					}
				}
			}
			if (post_message_) {
				PostMessage(m_hWnd, WM_NOTIFYGQSLISTUPDATE, (WPARAM)true, (LPARAM)connection_id_);
				post_message_ = false;
			}
			break;
		}
		default:
			break;
	}
	Log(CLog::Info, "GQSConnection", "NewPacketNotify", "End NewPacketNotify.");
}

void GQSConnection::ConStatusNotify(const bool connected) {
	Log(CLog::Info, "GQSConnection", "ConStatusNotify", "Begin ConStatusNotify.");
    if (connected)
		requestPacket(PACK_PUSH_GQSBASICINFO);
  	Log(CLog::Info, "GQSConnection", "ConStatusNotify", "End ConStatusNotify.");
}

void GQSConnection::requestPacket(int requestType) {
	std::vector<char> buf;
	buf.resize(CPack::_const_pack_head_size);
    int intHeadSize = CPack::_const_pack_head_size;
	CPack::PackHead(&*buf.begin(), intHeadSize, buf.size(), package_no_++, 100, requestType);
    SendPacket(&*buf.begin(), buf.size());
}


