#include "stdafx.h"
#include <afxinet.h>
#include "transctrlcon.h"
#include "multicon.h"
#include "windows_mini_lib.h"

using namespace windows_mini_lib;

CMultiCon::CMultiCon(void) : m_blnIsSubTrans(false),
							 m_blnIsConnect(false),
							 m_intPackNo(0),
							 m_intSubTrans(0),
							 connection_id_(0),
							 trans_id_offset(0) {
}

CMultiCon::~CMultiCon(void) {
}

void CMultiCon::InitConnect(const string & strIp, 
							const unsigned short shtPort, 
							const string & strNote, 
							vector<unsigned short> vectMarket, 
							bool blnIsSubTrans, 
							HWND hWnd, 
							unsigned short connection_id) {
    Log(CLog::Info, "CMultiCon", "InitConnect", "Begin Initialize the connection.");
    {
        CAutoLock lock(m_lock);
        for (unsigned int i = 0; i < vectMarket.size(); i++) {
            m_mapTrans[vectMarket[i]];
        }
    }

    Connect(strIp.c_str(), shtPort);
    m_blnIsConnect = true;
    m_strID = strNote + ":" + strIp;
    //m_repKey = strIp + "(" + strNote + ")";
    m_repKey = strIp;
    m_blnIsSubTrans = blnIsSubTrans;
    m_hWnd = hWnd;
	connection_id_ = connection_id;
	// Assuming every connected IP has trans servers no more than 200.
	trans_id_offset = connection_id * 200;

    Log(CLog::Info, "CMultiCon", "InitConnect", "End Initialize the connection.");
}

void CMultiCon::NewPacketNotify(const char* pPacket, const int intLen) {
    Log(CLog::Info, "CMultiCon", "NewPacketNotify", "Begin NewPacketNotify.");
    SPackHead head;
    CPack::UnpackHead(&head, pPacket, CPack::_const_pack_head_size);
    const char* pPos = pPacket;
    pPos += CPack::_const_pack_head_size;
    bool initialize = false;
    switch (head.m_shtType) {
		case PACK_PUSH_TRANSDEFINE : {
			CAutoLock lock(m_lock);
			int intTransSize = 0;
			pPos += OutFromBuf(intTransSize, pPos);
			for (int i = 0; i < intTransSize; ++i) {
				char szName[32] = {0};
				pPos += OutFromBuf(szName, pPos);
				unsigned int intIP  = 0;
				pPos += OutFromBuf(intIP, pPos);
				unsigned short shtTransID = 0;
				pPos += OutFromBuf(shtTransID, pPos);
				shtTransID += trans_id_offset;
				m_mapDefs[shtTransID].shtTransID = shtTransID;
				m_mapDefs[shtTransID].intIP = intIP;
				strncpy(m_mapDefs[shtTransID].szName, szName, sizeof (m_mapDefs[shtTransID].szName));
			}
			if (m_mapDefs != m_mapDefsNotify) {
				initialize = true;
				PostMessage(m_hWnd, WM_NOTIFYLISTUPDATE, (WPARAM)initialize, (LPARAM)connection_id_);
				m_mapDefsNotify = m_mapDefs;
			}
			break;
		} case PACK_PUSH_TRANSSTATUS: {
			CAutoLock lock(m_lock);
			int intTransSize = 0;
			pPos += OutFromBuf(intTransSize, pPos);
			for (int i = 0; i < intTransSize; ++i) {
				unsigned short shtTransID = 0;
				pPos += OutFromBuf(shtTransID, pPos);
				shtTransID += trans_id_offset;
				unsigned short shtSubTrans = 0;
				pPos += OutFromBuf(shtSubTrans, pPos);
				// 上游节点的IP和端口信息
				SConInfo qtInfo;
				pPos += OutFromBuf(qtInfo, pPos);
				memcpy(&m_mapDefs[shtTransID].qtSvrInfo, &qtInfo, sizeof(SConInfo));

				unsigned short shtMarketSize = 0;
				pPos += OutFromBuf(shtMarketSize, pPos);
				for (int j = 0; j < shtMarketSize; ++j) {
					SMarketItem item;
					item.shtTransID = shtTransID;
					pPos += OutFromBuf(item.shtMarketID, pPos);
					pPos += OutFromBuf(item.intTradeSeqs, pPos);
					pPos += OutFromBuf(item.intTradeDate, pPos);
					pPos += OutFromBuf(item.intTradeTime, pPos);
					if (item.shtMarketID != 0) {
						// Update standard time
						// We do not talk about the time of the calculation servers here.
						if (m_mapStandartTime[item.shtMarketID] < item &&
							std::string(m_mapDefs[item.shtTransID].szName).find("计算") == std::string::npos &&
							std::string(m_mapDefs[item.shtTransID].szName).find("(调)") == std::string::npos &&
							std::string(m_mapDefs[item.shtTransID].szName).find("转发真如") == std::string::npos &&
							std::string(m_mapDefs[item.shtTransID].szName).find("(港)") == std::string::npos) {
							m_mapStandartTime[item.shtMarketID] = item;
						}
						// if data has changed, notify the screen redraw.
						SMarketItem& tmpItem = m_mapData[shtTransID].mapItems[item.shtMarketID];
						if (memcmp(&tmpItem, &item, sizeof(item)) != 0)
							vecUpdateData.push_back(item);

						if (m_mapData[shtTransID].shtSubTrans != shtSubTrans)
							vecUpdateSubs.push_back(shtTransID * 1000 + shtSubTrans);
						m_mapData[shtTransID].shtTransID = shtTransID;
						m_mapData[shtTransID].shtSubTrans = shtSubTrans;
						tmpItem = item;
					}
				}
			}
			//TRACE("update item: %d \n", vecUpdateData.size());
			if (!vecUpdateSubs.empty() || !vecUpdateData.empty())
				PostMessage(m_hWnd, WM_NOTIFYLISTUPDATE, (WPARAM)(false), (LPARAM)connection_id_);
			break;
		} case PACK_PUSH_COMPRESSEDSTATUS: {
			uncompress_trans_status(pPos, intLen - CPack::_const_pack_head_size);
			if (!vecUpdateSubs.empty() || !vecUpdateData.empty())
				PostMessage(m_hWnd, WM_NOTIFYLISTUPDATE, (WPARAM)(false), (LPARAM)connection_id_);
			break;
		} default: {
			break;
		}
    }
    Log(CLog::Info, "CMultiCon", "NewPacketNotify", "End NewPacketNotify.");
}

void CMultiCon::ConStatusNotify(const bool blnIsConnect) {
    Log(CLog::Info, "CMultiCon", "ConStatusNotify", "Begin ConStatusNotify.");
    if (blnIsConnect) {
        m_mapData.clear();
        vecUpdateSubs.clear();
        m_mapStandartTime.clear();
        vecUpdateData.clear();
        m_mapDefs.clear();
        m_mapTrans.clear();
		requestPacket(PACK_PUSH_TRANSDEFINE);
		requestPacket(PACK_PUSH_COMPRESSEDSTATUS);
		requestPacket(PACK_PUSH_TRANSSTATUS);
    } else {
        // socket close
    }
    Log(CLog::Info, "CMultiCon", "ConStatusNotify", "End ConStatusNotify.");
}

void CMultiCon::requestPacket(int requestType) {
	std::vector<char> buf;
    buf.resize(CPack::_const_pack_head_size);
    int intHeadSize = CPack::_const_pack_head_size;
	CPack::PackHead(&*buf.begin(), intHeadSize, buf.size(), m_intPackNo++, 100, requestType);
    SendPacket(&*buf.begin(), buf.size());
}

void CMultiCon::uncompress_trans_status(const char* packet_content, const int content_size) {
	if (!packet_content || content_size <= 0) {
		return;
	} else {
		auto packet_end = packet_content + content_size;
		unsigned short trans_number = 0;
		packet_content += OutFromBuf(trans_number, packet_content);
		map<unsigned short, trans_status> trans_statuss;
		for (unsigned short i = 0; i < trans_number; ++i) {
			unsigned short trans_id_plus_sub_trans_number = 0;
			packet_content += OutFromBuf(trans_id_plus_sub_trans_number, packet_content);
			unsigned short trans_id = trans_id_plus_sub_trans_number >> 6;
			trans_id += trans_id_offset;
			unsigned short sub_trans_num = trans_id_plus_sub_trans_number & 0x003F;
			int numeric_ip = 0;
			packet_content += OutFromBuf(numeric_ip, packet_content);
			struct in_addr inaddr;
			inaddr.S_un.S_addr = numeric_ip;
			string server_ip(inet_ntoa(inaddr));
			unsigned short port = 0;
			packet_content += OutFromBuf(port, packet_content);
			bool is_flag_changed = false;
			packet_content += OutFromBuf(is_flag_changed, packet_content);
			SConInfo qt_server_info;
			memcpy(qt_server_info.m_strIp, server_ip.c_str(), server_ip.size());
			qt_server_info.m_shtPort = port;
			// At present this flag is idle.
			qt_server_info.m_intFlag = (is_flag_changed == false) ? -1 : 0;
			memcpy(&m_mapDefs[trans_id].qtSvrInfo, &qt_server_info, sizeof (SConInfo));
			trans_statuss[trans_id] = trans_status(trans_id, sub_trans_num, qt_server_info);
		}
		while (packet_content != packet_end) {
			unsigned short item_number = 0;
			packet_content += OutFromBuf(item_number, packet_content);
			unsigned short market_id = 0;
			packet_content += OutFromBuf(market_id, packet_content);
			unsigned int trade_sequence = 0;
			packet_content += OutFromBuf(trade_sequence, packet_content);
			bool is_sequence_equal_to_date = false;
			packet_content += OutFromBuf(is_sequence_equal_to_date, packet_content);
			unsigned int trade_date = trade_sequence;
			if (!is_sequence_equal_to_date)
				packet_content += OutFromBuf(trade_date, packet_content);
			unsigned int trade_time = 0;
			packet_content += OutFromBuf(trade_time, packet_content);
			SMarketItem market_item;
			for (unsigned short j = 0; j < item_number; ++j) {
				unsigned short trans_id = 0;
				packet_content += OutFromBuf(trans_id, packet_content);
				trans_id += trans_id_offset;
				market_item = SMarketItem(trans_id, market_id, trade_sequence, trade_date, trade_time);
				if (market_item.shtMarketID != 0) {
					// update standard time
					if (m_mapStandartTime[market_item.shtMarketID] < market_item &&
						std::string(m_mapDefs[trans_id].szName).find("计算") == std::string::npos)
						m_mapStandartTime[market_item.shtMarketID] = market_item;
					// if data has changed, notify the screen redraw.
					SMarketItem& tmpItem = m_mapData[trans_id].mapItems[market_item.shtMarketID];
					if (memcmp(&tmpItem, &market_item, sizeof(market_item)))
						vecUpdateData.push_back(market_item);

					auto& sub_trans_number = trans_statuss[trans_id].sub_trans_number;
					if (m_mapData[trans_id].shtSubTrans != sub_trans_number)
						vecUpdateSubs.push_back(trans_id * 1000 + sub_trans_number);
					m_mapData[trans_id].shtTransID = trans_id;
					m_mapData[trans_id].shtSubTrans = sub_trans_number;
					tmpItem = market_item;
				}
			}
		}
	}
}

