// CalculationServerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CalculationServerDialog.h"
#include "afxdialogex.h"

// CalculationServerDialog dialog

IMPLEMENT_DYNAMIC(CalculationServerDialog, CDialog)

CalculationServerDialog::CalculationServerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CalculationServerDialog::IDD, pParent) {
}

CalculationServerDialog::~CalculationServerDialog() {
}

void CalculationServerDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CALCULATION_SERVER, calculation_server_list_ctrl_);
}

BEGIN_MESSAGE_MAP(CalculationServerDialog, CDialog)
END_MESSAGE_MAP()

// CalculationServerDialog message handlers

BOOL CalculationServerDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	SetIcon(icon_, TRUE);         // Set big icon
	SetIcon(icon_, FALSE);        // Set small icon
	windows_mini_lib::SetListCtrlStyle(&calculation_server_list_ctrl_);
	calculation_server_list_ctrl_.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	calculation_server_list_ctrl_.InsertColumn(0, _T("No."), LVCFMT_CENTER, 30);
	calculation_server_list_ctrl_.InsertColumn(1, _T("Server."), LVCFMT_CENTER, 87);
	calculation_server_list_ctrl_.InsertColumn(2, _T("Server IP"), LVCFMT_LEFT, 100);
	// These two columns are idle at present.
	calculation_server_list_ctrl_.InsertColumn(3, _T("Upstream Address"), LVCFMT_CENTER, 0);
	calculation_server_list_ctrl_.InsertColumn(4, _T("Subs"), LVCFMT_CENTER, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CalculationServerDialog::InitListCtrl(const PriorMap& markets, const map<unsigned short, STransDefine>& trans_defines) {
	calculation_server_list_ctrl_.SetRedraw(FALSE);
	int column_no = 5;
	for (auto& iter : markets) {
		if (iter.first != 100) {
			string column_name = iter.second + string(" [ ") + to_string(iter.first) + string(" ]");
			markets_position_[iter.first] = column_no;
			calculation_server_list_ctrl_.InsertColumn(column_no++, _T(column_name.c_str()), LVCFMT_CENTER, 118);
		}
	}
	// Initialize basic information
	if (!trans_defines.empty()) {
		int j = calculation_server_list_ctrl_.GetItemCount();
		for (auto& iter : trans_defines) {
			string server_name(iter.second.szName);
			if (server_name.find("º∆À„") != std::string::npos) {
				string upstream;
				stringstream ss;
				struct in_addr inaddr;
				SConInfo qtInfo = iter.second.qtSvrInfo;
				inaddr.S_un.S_addr = iter.second.intIP;
				upstream = string(qtInfo.m_strIp) + string(":");
				ss << qtInfo.m_shtPort;
				upstream += ss.str().c_str();
				calculation_server_list_ctrl_.InsertItem(j, to_string(j + 1).c_str());
				calculation_server_list_ctrl_.SetItemText(j, 1, iter.second.szName);
				calculation_server_list_ctrl_.SetItemText(j, 2, inet_ntoa(inaddr));
				calculation_server_list_ctrl_.SetItemText(j, 3, upstream.c_str());
				servers_position_[iter.first] = j;
				if (server_name.find("÷˜") != std::string::npos)
					calculation_server_list_ctrl_.PushSelfDefinedColor(j, 1, RGB(0, 180, 80), RGB(255, 255, 255));
				++j;
			}
		}
	}
	calculation_server_list_ctrl_.SetRedraw(TRUE);
	calculation_server_list_ctrl_.UpdateWindow();
}

void CalculationServerDialog::UpdateListCtrl(const std::map<unsigned short, STransDefine>& trans_defines,
											 const std::vector<int>& sub_trans,
											 const std::vector<SMarketItem>& market_items,
											 const std::map<unsigned short, SMarketItem>& standard_times) {
	calculation_server_list_ctrl_.SetRedraw(FALSE);
	// Update upstream information.
	for (auto& iter : trans_defines) {
		if (servers_position_.find(iter.first) != servers_position_.end()) {
			auto row_no = servers_position_[iter.first];
			string upstream;
			stringstream ss;
			struct in_addr inaddr;
			SConInfo qtInfo = iter.second.qtSvrInfo;
			inaddr.S_un.S_addr = iter.second.intIP;
			upstream = string(qtInfo.m_strIp) + string(":");
			ss << qtInfo.m_shtPort;
			upstream += ss.str().c_str();
			calculation_server_list_ctrl_.SetItemText(row_no, 3, upstream.c_str());
		}
	}
	// Update downstream information.
	for (auto& iter : sub_trans) {
		char tmp_buffer[256] = { 0 };
		unsigned short trans_id = iter / 1000;
		if (servers_position_.find(trans_id) != servers_position_.end()) {
			auto row_no = servers_position_[trans_id];
			sprintf(tmp_buffer, "[%d]", iter % 1000);
			calculation_server_list_ctrl_.SetItemText(row_no, 4, tmp_buffer);
			if (iter % 1000 >= 15) {
				calculation_server_list_ctrl_.PushSelfDefinedColor(row_no, 4, RGB(255, 0, 0), RGB(255, 255, 255));
			} else {
				calculation_server_list_ctrl_.EraseSelfDefinedColor(row_no, 4);
			}
		}
	}
	// Update market information
	for (auto& iter : market_items) {
		char pTmp[30] = { 0 };
		if (markets_position_.find(iter.shtMarketID) != markets_position_.end() &&
			servers_position_.find(iter.shtTransID) != servers_position_.end()) {
			unsigned short row_no = servers_position_[iter.shtTransID];
			unsigned short column_no = markets_position_[iter.shtMarketID];
			auto tradeSequence = (iter.intTradeSeqs >= 20170101 && iter.intTradeSeqs <= 20171231) ?
				iter.intTradeSeqs % 20170000 : iter.intTradeSeqs;
			auto tradeDate = (iter.intTradeDate >= 20170101 && iter.intTradeDate <= 20171231) ?
				iter.intTradeDate % 20170000 : iter.intTradeDate;
			if (iter.intTradeSeqs != iter.intTradeDate) {
				sprintf(pTmp, "%04u[%04u %06u]", tradeSequence, tradeDate, iter.intTradeTime);
			} else {
				sprintf(pTmp, "%04u : %06u", tradeSequence, iter.intTradeTime);
			}
			auto iter2 = standard_times.find(iter.shtMarketID);
			if (iter2 != end(standard_times)) {
				if (iter2->second.intTradeSeqs != iter.intTradeSeqs ||
					iter2->second.intTradeDate != iter.intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter.intTradeTime))) > 10) {
					calculation_server_list_ctrl_.PushSelfDefinedColor(row_no, column_no, RGB(0, 0, 0), RGB(254, 254, 65));
				} else {
					calculation_server_list_ctrl_.EraseSelfDefinedColor(row_no, column_no);
				}
			}
			calculation_server_list_ctrl_.SetItemText(row_no, column_no, pTmp);
		}
	}
	calculation_server_list_ctrl_.SetRedraw(TRUE);
	calculation_server_list_ctrl_.UpdateWindow();
}
