#pragma once
#include "stdafx.h"
#include <mutex>

#pragma pack(push, 1)
struct GQSServerInfo {
	std::string  server_name;
	std::string  server_ip;
	int          port;
	bool        connected;
	GQSServerInfo() {
		port = 0;
		connected = false;
	}

	bool operator==(const GQSServerInfo& c) {
		return server_ip == c.server_ip;
	}

	GQSServerInfo& operator=(const GQSServerInfo& c) {
		if (this == &c)
			return *this;

		server_ip = c.server_ip;
		connected = c.connected;
		port = c.port;
		server_name = c.server_name;
		return *this;
	}
	//GQSServerInfo& operator=(GQSServerInfo&& c) = default;
};
#pragma pack(pop)

class GQSConnection : public CTransCtrlCon {
public:
	GQSConnection();
	~GQSConnection();
    void InitConnect(const string& server_ip, const unsigned short port, HWND hWnd, unsigned short connection_id);
    virtual void NewPacketNotify(const char * packet, const int packet_size);
    virtual void ConStatusNotify(const bool connected);
	void requestPacket(int requestType = 0);
	// In multi-thread app, it is not advised to return a shared member's reference.
	void GetBasicInfo(std::vector<GQSServerInfo>& basic_info) {
		std::lock_guard<std::mutex>	lock(basic_info_mutex_);
		basic_info = basic_info_;
	}
	HWND							m_hWnd;
protected:
	int								package_no_;
	std::mutex						basic_info_mutex_;
	bool							connected_;
	bool							post_message_;
	std::vector<GQSServerInfo>		basic_info_;
	unsigned short					connection_id_;
};

