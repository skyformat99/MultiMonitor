// TransDialogDelay.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")
#include "TransDialogDelay.h"
#include "afxdialogex.h"


// CTransDialogDelay dialog

// CTransDialogDelay message handlers
BEGIN_MESSAGE_MAP(CTransDialogDelay, CDialog)
	ON_MESSAGE(WM_NOTIFYLISTUPDATE, OnNotifyListUpdate)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TRANSDELAY, &CTransDialogDelay::OnLvnItemchangedListTrans)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TRANSDELAY, &CTransDialogDelay::OnLvnColumnclick)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TRANSFULL, &CTransDialogDelay::OnLvnFullListColumnclick)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TRANSDELAY, &CTransDialogDelay::OnNMDblclkList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TRANSFULL, &CTransDialogDelay::OnRclickFullList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TRANSDELAY, &CTransDialogDelay::OnRclickSummaryList)
	ON_COMMAND(ID_MENU_COPY_SERVER_IP, &CTransDialogDelay::OnMenuCopyServerIp)
	ON_COMMAND(ID_MENU_COPY_UPSTREAM_IP, &CTransDialogDelay::OnMenuCopyUpstreamIp)
	ON_COMMAND(ID_DETAILS_DETAILSMARKET, &CTransDialogDelay::OnDetailsDetailsmarket)
	/*ON_COMMAND(ID_DETAILS_DISABLEPOPUP, &CTransDialogDelay::OnDetailsDisablepopup)
	ON_COMMAND(ID_DETAILS_ENABLEPOPUP, &CTransDialogDelay::OnDetailsEnablepopup)*/
	ON_COMMAND(ID_DETAILS_PICKUPACOLOR, &CTransDialogDelay::OnDetailsPickupacolor)
END_MESSAGE_MAP()

//BEGIN_EASYSIZE_MAP(CTransDialogDelay)
//	/*EASYSIZE(IDC_STATIC_SEARCH, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDC_STATIC_REFRESH_TIME, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDC_STATIC_IP_LIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)*/
//	//EASYSIZE(IDC_COMBO_MARKET, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDC_COMBO_CONNECTEDIPS, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDC_EDIT_TIME, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CTransDialogDelay, CDialog)

CTransDialogDelay::CTransDialogDelay(MonitorDialogCallBack* parentDialog, CWnd* pParent)
		: CDialog(CTransDialogDelay::IDD, pParent),
		transSearchingDialog_(nullptr),
		transDetailsDialog_(nullptr),
		isWaringMusicOn_(false),
		isNormalMusicOn_(false),
		market_list_selected_column_no_(65535),
		full_list_selected_column_no_(65535),
		full_list_initialized_(false),
		parentDialog_(parentDialog) {
	memset(&launchTime_, 0, sizeof launchTime_);
	acceleratorSearch_ = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	acceleratorShowDetails_ = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR2));
}

CTransDialogDelay::~CTransDialogDelay() {
	for (auto& c : market_info_dialogs_) {
		if (c.second) {
			delete c.second;
			c.second = nullptr;
		}
	}
	for (auto& ptr : connections_) {
		if (ptr) {
			ptr->StopThread();
			delete ptr;
			ptr = nullptr;
		}
	}
	if (transSearchingDialog_) {
		delete transSearchingDialog_;
		transSearchingDialog_ = nullptr;
	}
	if (transDetailsDialog_) {
		delete transDetailsDialog_;
		transDetailsDialog_ = nullptr;
	}
}

void CTransDialogDelay::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRANSDELAY, summary_list_ctrl_);
	DDX_Control(pDX, IDC_LIST_TRANSFULL, full_list_ctrl_);
}

void CTransDialogDelay::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // Context for painting.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw icon.
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

void CTransDialogDelay::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	//UPDATE_EASYSIZE;
	// TODO: add your handler code here.
	// If the dialog is minisized, cx == 0 && cy == 0. This will cause error in ChangeSize().
	if (nType != SIZE_MINIMIZED && cx != 0 && cy != 0) {
		//GetClientRect(&currentWindowSize_);
		//CWnd* pWnd = GetDlgItem(IDC_LIST_TRANSDELAY);
		//CRect summaryListSize;
		//pWnd->GetClientRect(&summaryListSize);
	//	windows_mini_lib::adjustCtrlPosition(pWnd, cx, cy, currentWindowSize_);
	//	pWnd = GetDlgItem(IDC_COMBO_MARKET);
	//	windows_mini_lib::adjustCtrlPosition(pWnd, cx, cy, currentWindowSize_);
	}
}

HCURSOR CTransDialogDelay::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void CTransDialogDelay::OnClose() {
	// TODO: add your handler code here.
	EndDialog(IDCANCEL);
	CDialog::OnClose();
}

bool CTransDialogDelay::readConfigFile(std::string configFileName) {
	using namespace boost::property_tree;
	ptree pt;
	read_xml(configFileName.c_str(), pt);
	ptree child = pt.get_child("con");
	auto name = child.get<std::string>("name");
	auto musicFiles = pt.get_child("con.musicfiles");
	auto index = 0;
	for (auto iter : musicFiles) {
		if (index == 0)
			backgroundMusicPath_ = iter.second.data();
		else if (index == 1)
			warningMusicPath_ = iter.second.data();
		++index;
	}
	return true;
}

BOOL CTransDialogDelay::OnInitDialog() {
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

	windows_mini_lib::SetListCtrlStyle(&summary_list_ctrl_);
	windows_mini_lib::SetListCtrlStyle(&full_list_ctrl_);
	GetClientRect(&currentWindowSize_);
	LoadMarketCode("./marketcode.xml");
	// Set the caption of each column.
	summary_list_ctrl_.InsertColumn(0, _T("No."), LVCFMT_CENTER, 30);
	summary_list_ctrl_.InsertColumn(1, _T("ID"), LVCFMT_CENTER, 30);
	summary_list_ctrl_.InsertColumn(2, _T("Market"), LVCFMT_CENTER, 70);
	summary_list_ctrl_.InsertColumn(3, _T("Max Time"), LVCFMT_CENTER, 90);
	full_list_ctrl_.InsertColumn(0, _T("No."), LVCFMT_CENTER, 30);
	full_list_ctrl_.InsertColumn(1, _T("Server."), LVCFMT_CENTER, 77);
	full_list_ctrl_.InsertColumn(2, _T("Server IP"), LVCFMT_LEFT, 100);
	full_list_ctrl_.InsertColumn(3, _T("Upstream Address"), LVCFMT_CENTER, 130);
	full_list_ctrl_.InsertColumn(4, _T("Subs"), LVCFMT_CENTER, 45);
	
	if (!isTransSearchingDialogCreated())
		AfxMessageBox(_T("Error Creating Dialog(TransSearchingDialog)."));
	if (!isTransDetailsDialogCreated())
		AfxMessageBox(_T("Error Creating Dialog(TransDetailsDialog)."));
	
	int row = 0;
	int column_no = 5;
	std::vector<unsigned short> vectMarket;
	for (PriorMap::iterator it = markets_.begin(); it != markets_.end(); it++) {
		if (it->first != 100) {
			m_mapMarketFullListPos[it->first] = column_no;
			string colName = it->second + string(" [ ") + to_string(it->first) + string(" ]");
			full_list_ctrl_.InsertColumn(column_no++, _T(colName.c_str()), LVCFMT_CENTER, 110);
			// Map the market' Chinese name to its ID for the use of searching.
			m_mapMarketNameToID[it->second] = it->first;
			vectMarket.push_back(it->first);

			m_mapMarketListPos[it->first] = row;
			summary_list_ctrl_.InsertItem(row, to_string(row + 1).c_str());
			summary_list_ctrl_.SetItemText(row, 1, to_string(it->first).c_str());
			summary_list_ctrl_.SetItemText(row, 2, it->second.c_str());
			++row;
		}
	}

	// set virtual list count, change the arguments later for better performance.
	summary_list_ctrl_.SetItemCount(vectMarket.size());									
	full_list_ctrl_.SetItemCount(150);													// Preserve 100 items for 100 servers.
	LoadServer("./server.xml", vectMarket);
	readConfigFile("./trans_viewer_config.xml");
	transDetailsDialog_->connectedIP1_.SetWindowTextA(_T(map_connection_id_to_ip_[0]));
	transDetailsDialog_->connectedIP2_.SetWindowTextA(_T(map_connection_id_to_ip_[1]));
	transDetailsDialog_->connectedIP3_.SetWindowTextA(_T(map_connection_id_to_ip_[2]));
	GetLocalTime(&launchTime_);
	GetLocalTime(&baseTime_);
	if (parentDialog_->canPlayMusic())
		PlaySound(_T(backgroundMusicPath_.c_str()), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	isNormalMusicOn_ = true;
	//INIT_EASYSIZE;
	return TRUE;  //return TRUE unless you set the focus to a control
}

// Process message.
LRESULT CTransDialogDelay::OnNotifyListUpdate(WPARAM wp, LPARAM lp) {
	bool bool_get_msg_trans_define = (wp != 0);
	auto connection_id = (unsigned short)lp;
	if (connections_[connection_id]) {
		summary_list_ctrl_.SetRedraw(FALSE);
		setConnectedIPNotifiedTime(connection_id);
		// To push color for the whole row successfully.	
		for (auto k = 0; k < summary_list_ctrl_.GetItemCount(); ++k) {
			for (auto m = 0; m < 4; ++m) {
				CString	text = summary_list_ctrl_.GetItemText(k, m);
				summary_list_ctrl_.SetItemText(k, m, text);
			}
		}
		auto mapDefs = connections_[connection_id]->GetDefs();
		if (!mapDefs.empty() && bool_get_msg_trans_define) {
			m_mapDefsChild.insert(mapDefs.begin(), mapDefs.end());					// Record it for children dialogs' initialization.
			for (auto it1 : mapDefs) {
				servers_[it1.first] = std::string(it1.second.szName);
				for (auto it : m_mapMarketListPos) {
					m_mapColoredItem[it.second][it1.first] = false;
				}
			}
		}
		std::vector<int> vecSubs;
		connections_[connection_id]->GetSubs(vecSubs);

		static bool bool_get_child_subs[5]{ false };
		if (!bool_get_child_subs[connection_id] && !vecSubs.empty()) {
			m_vecSubsChild.insert(m_vecSubsChild.end(), vecSubs.begin(), vecSubs.end());		// Record it for children dialogs' initialization
			bool_get_child_subs[connection_id] = true;
		}
		// Update market information.
		std::vector<SMarketItem> vecData;
		connections_[connection_id]->GetData(vecData);
		std::map<unsigned short, SMarketItem> mapStandardTime = connections_[connection_id]->GetStandardTime();
		UpdateMarketStandardTimes(mapStandardTime);
		// Record the information of the whole full_list_ctrl_ for its children dialogs.
		static bool bool_get_data[5]{ false };
		static auto counter = 0;
		if (!bool_get_data[connection_id] && !vecData.empty()) {
			m_vecDataChild.insert(m_vecDataChild.begin(), vecData.begin(), vecData.end());			// Record it for child dialogs' initialization
			bool_get_data[connection_id] = true;
			++counter;
		}
		if (!full_list_initialized_) {
			SYSTEMTIME fullListInitializationTime;
			GetLocalTime(&fullListInitializationTime);
			if (counter == connections_.size() || std::abs(fullListInitializationTime.wSecond - launchTime_.wSecond) > 10) {
				InitFullList(m_mapDefsChild);
				std::map<unsigned short, STransDefine> tmp;
				UpdateFullList(tmp, m_vecSubsChild, m_vecDataChild, market_standard_times_);
				full_list_initialized_ = true;
			}
		}
		for (auto iter = vecData.begin(); iter != vecData.end(); ++iter) {
			std::string serverName(servers_[iter->shtTransID]);							// Get the server's Chinese name.
			auto& shtMarketID = iter->shtMarketID;
			if (m_mapMarketListPos.find(shtMarketID) != m_mapMarketListPos.end() &&
				serverName.find("(调)") == std::string::npos &&
				serverName.find("计算") == std::string::npos &&
				serverName.find("转发真如") == std::string::npos &&
				serverName.find("(港)") == std::string::npos) {
				unsigned short iRow = m_mapMarketListPos[shtMarketID];
				m_mapMarketMaxTime[iRow] = (m_mapMarketMaxTime[iRow] < *iter) ? *iter : m_mapMarketMaxTime[iRow];
				auto iter2 = market_standard_times_.find(shtMarketID);
				if (iter2 != end(market_standard_times_)) {
					if ((iter2->second.intTradeSeqs != iter->intTradeSeqs ||
						iter2->second.intTradeDate != iter->intTradeDate ||
						abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 10)) {
						m_mapColoredItem[iRow][iter->shtTransID] = true;
					} else {
						m_mapColoredItem[iRow][iter->shtTransID] = false;
					}
				}
			}
		}
		// Show the maximum time of each market.
		for (auto iter : m_mapMarketMaxTime) {
			char tradeDateFormat[10] = { 0 };
			if (iter.second.intTradeDate >= 20170101 && iter.second.intTradeDate <= 20171231)
				sprintf(tradeDateFormat, "%04u", iter.second.intTradeDate % 20170000);
			else
				sprintf(tradeDateFormat, "%u", iter.second.intTradeDate);
			char tradeTimeFormat[8] = { 0 };
			sprintf(tradeTimeFormat, "%06u", iter.second.intTradeTime);
			std::string strMarketMaxTime = (std::string(tradeDateFormat) + std::string(" : ") + std::string(tradeTimeFormat));
			unsigned short row_no = iter.first;
			if (market_list_selected_column_no_ != 65535) 
				row_no = map_market_list_column_sorted_state_[market_list_selected_column_no_].map_sorted_row_no[row_no];
			summary_list_ctrl_.SetItemText(row_no, 3, strMarketMaxTime.c_str());
		}
		// Decide which row to push/erase color.
		std::set<unsigned short> marketsDelayed;
		bool shouldWaring = false;
		for (auto it : m_mapColoredItem) {
			unsigned int counter = 0;
			for (auto it1 : it.second) {
				if (it1.second) {
					shouldWaring = true;
					++counter;
				}
			}
			unsigned short row_no = it.first;
			if (market_list_selected_column_no_ != 65535)
				row_no = map_market_list_column_sorted_state_[market_list_selected_column_no_].map_sorted_row_no[row_no];
			if (counter != 0) {
				CString	strMarketID = summary_list_ctrl_.GetItemText(row_no, 1);
				unsigned short marketID = static_cast<unsigned short>(atoi(strMarketID));
				marketsDelayed.insert(marketID);
			}
			if (counter > 0 && counter < 10) {
				for (int column = 0; column < 4; ++column)
					summary_list_ctrl_.PushSelfDefinedColor(row_no, column, RGB(0, 0, 0), RGB(254, 254, 65));
			} else if (counter >= 10) {
				for (int column = 0; column < 4; ++column)
					summary_list_ctrl_.PushSelfDefinedColor(row_no, column, RGB(138, 43, 226), RGB(255, 201, 147));
			} else {
				for (int column = 0; column < 4; ++column)
					summary_list_ctrl_.EraseSelfDefinedColor(row_no, column);
			}
		}
		if (!marketsDelayed.empty())
			parentDialog_->popupMainDialog();
		if (parentDialog_->canPlayMusic()) {
			if (shouldWaring && !isWaringMusicOn_) {
				PlaySound(_T(warningMusicPath_.c_str()), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
				isWaringMusicOn_ = true;
				isNormalMusicOn_ = false;
			} else if (!shouldWaring && !isNormalMusicOn_) {
				PlaySound(_T(backgroundMusicPath_.c_str()), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
				isNormalMusicOn_ = true;
				isWaringMusicOn_ = false;
			}
		} else {
			PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		}
		// Update the child dialogs.
		for (auto& pair_market_info_dialog : market_info_dialogs_) 
			pair_market_info_dialog.second->UpdateDetails(mapDefs, vecSubs, vecData, market_standard_times_);
		for (auto& markets_info_dialog : markets_info_dialogs_)
			markets_info_dialog->UpdateDetails(mapDefs, vecSubs, vecData, market_standard_times_);
		if (full_list_initialized_)
			UpdateFullList(mapDefs, vecSubs, vecData, market_standard_times_);
		forceUpdateAllMarketCells();
		summary_list_ctrl_.SetRedraw(TRUE);
		summary_list_ctrl_.UpdateWindow();
	}
	return 0;
}

bool CTransDialogDelay::LoadServer(const string & strFileName, const vector<unsigned short> & vectMarket) {
	using boost::property_tree::ptree;
	try {
		ptree pt;
		read_xml(strFileName, pt);
		{
			unsigned short connection_id = 0;
			ptree serverlist = pt.get_child("serverlist");
			for (ptree::iterator itr = serverlist.begin(); itr != serverlist.end(); itr++) {
				ptree svr = itr->second;
				string strServer = svr.get<string>("<xmlattr>.ip");
				map_connection_id_to_ip_[connection_id] = CString(strServer.c_str());
				int intPort = svr.get<int>("<xmlattr>.port");
				string strNote = svr.get<string>("<xmlattr>.note");
				bool blnSubTrans = svr.get<bool>("<xmlattr>.subtrans", false);
				CMultiCon* pCon = new CMultiCon;
				pCon->InitConnect(strServer, intPort, strNote, vectMarket, blnSubTrans, m_hWnd, connection_id++);
				pCon->StartThread();
				connections_.push_back(pCon);
			}
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

bool CTransDialogDelay::LoadMarketCode(const string & strFileName) {
	using boost::property_tree::ptree;
	try {
		ptree pt;
		read_xml(strFileName, pt);
		{
			ptree marketlist = pt.get_child("marketlist");
			for (ptree::iterator itr = marketlist.begin(); itr != marketlist.end(); itr++) {
				ptree market = itr->second;
				string strCode = market.get<string>("<xmlattr>.code");
				int intID = market.get<int>("<xmlattr>.id");
				markets_[intID] = strCode;
			}
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}

void CTransDialogDelay::OnLvnItemchangedListTrans(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTransDialogDelay::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	market_list_selected_column_no_ = pNMLV->iSubItem;						// The selected column
	map_market_list_column_sorted_state_[market_list_selected_column_no_];
	int item_count = summary_list_ctrl_.GetItemCount();
	for (int i = 0; i < item_count; ++i) {
		// The second parameter is the first and the second parameters in SortColProc.
		summary_list_ctrl_.SetItemData(i, i);
	}
	summary_list_ctrl_.SortItems(SortColProc, (DWORD_PTR)this);			// The second parameter is the third parameter in SortColProc.

	auto& column_sorted_state = map_market_list_column_sorted_state_[market_list_selected_column_no_];
	for (int k = 0; k < item_count; ++k) {
		CString	cstring_original_row_no = summary_list_ctrl_.GetItemText(k, 0);
		unsigned short original_row_no = atoi(cstring_original_row_no) - 1;
		column_sorted_state.map_sorted_row_no[original_row_no] = k;
	}

	summary_list_ctrl_.GetVisualHeaderCtrl().SetSortImage(market_list_selected_column_no_, !column_sorted_state.visual_arrow_flag);
	column_sorted_state.SetVisualArrowFlag();
	*pResult = 0;
}

// The sorting function
int CALLBACK CTransDialogDelay::SortColProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	// Get the row numbers to compare
	int					row1 = (int)lParam1;
	int					row2 = (int)lParam2;
	CTransDialogDelay*	pDlg = (CTransDialogDelay*)lParamSort;
	auto  selected_column_no = pDlg->GetSelectedCol();
	CString				lp1  = pDlg->summary_list_ctrl_.GetItemText(row1, selected_column_no);
	CString				lp2  = pDlg->summary_list_ctrl_.GetItemText(row2, selected_column_no);
	if (selected_column_no == 0 || selected_column_no == 1) {
		if (pDlg->map_market_list_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return atoi(lp1) - atoi(lp2);
		} else {
			return atoi(lp2) - atoi(lp1);
		}
	} else {
		if (pDlg->map_market_list_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return lp1.CompareNoCase(lp2);
		} else {
			return lp2.CompareNoCase(lp1);
		}
	}
	return 0;
}

void CTransDialogDelay::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	unsigned int selected_count = summary_list_ctrl_.GetSelectedCount();
	if (selected_count == 1) {
		int row = -1;
		row = summary_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
		ASSERT(row != -1);
		CString	strMarketID = summary_list_ctrl_.GetItemText(row, 1);
		unsigned int marketID = static_cast<unsigned int>(atoi(strMarketID));
		if (market_info_dialogs_.find(marketID) != market_info_dialogs_.end()) {
			market_info_dialogs_[marketID]->ShowWindow(SW_SHOW);
		} else {
			std::vector<unsigned short> marketIDs(1, marketID);
			CTransDialog* market_info_dialog = new CTransDialog(marketIDs, markets_, m_mapDefsChild, m_vecSubsChild,
				m_vecDataChild, market_standard_times_);
			market_info_dialog->Create(IDD_DIALOG_TRANS);
			market_info_dialog->ShowWindow(SW_SHOW);
			market_info_dialogs_.insert({ marketID, market_info_dialog });
		}
	}
	*pResult = 0;
}

void CTransDialogDelay::InitFullList(const std::map<unsigned short, STransDefine>& trans_defines) {
	full_list_ctrl_.SetRedraw(FALSE);
	// Initialize basic information
	if (!trans_defines.empty()) {
		int j = full_list_ctrl_.GetItemCount();
		for (auto iter : trans_defines) {
			string server_name(iter.second.szName);
			if (server_name.find("计算") == std::string::npos &&
				server_name.find("(调)") == std::string::npos &&
				server_name.find("转发真如") == std::string::npos &&
				server_name.find("(港)") == std::string::npos) {
				string upstream;
				stringstream ss;
				struct in_addr inaddr;
				SConInfo qtInfo = iter.second.qtSvrInfo;
				inaddr.S_un.S_addr = iter.second.intIP;
				upstream = string(qtInfo.m_strIp) + string(":");
				ss << qtInfo.m_shtPort;
				upstream += ss.str().c_str();
				full_list_ctrl_.InsertItem(j, to_string(j + 1).c_str());
				full_list_ctrl_.SetItemText(j, 1, iter.second.szName);
				full_list_ctrl_.SetItemText(j, 2, inet_ntoa(inaddr));
				full_list_ctrl_.SetItemText(j, 3, upstream.c_str());
				m_mapTransFullListPos[iter.first] = j;
				map_server_ip_to_id_[string(inet_ntoa(inaddr))] = iter.second.shtTransID;
				if (server_name.find("主") != std::string::npos)
					full_list_ctrl_.PushSelfDefinedColor(j, 1, RGB(0, 180, 80), RGB(255, 255, 255));
				if (server_name.find("灰") != std::string::npos)
					full_list_ctrl_.PushSelfDefinedColor(j, 1, RGB(92, 92, 92), RGB(255, 255, 255));
				++j;
			}
		}
	}
	calculation_server_dialog_.InitListCtrl(markets_, trans_defines);
	full_list_ctrl_.SetRedraw(TRUE);
	full_list_ctrl_.UpdateWindow();
}

void CTransDialogDelay::UpdateFullList(const std::map<unsigned short, STransDefine>& trans_defines,
	const std::vector<int>& sub_trans,
	const std::vector<SMarketItem>& market_items,
	const std::map<unsigned short, SMarketItem>& standard_times) {
	full_list_ctrl_.SetRedraw(FALSE);
	// Update upstream information.
	for (auto iter : trans_defines) {
		if (m_mapTransFullListPos.find(iter.first) != m_mapTransFullListPos.end()) {
			auto row_no = m_mapTransFullListPos[iter.first];
			string upstream;
			stringstream ss;
			struct in_addr inaddr;
			SConInfo qtInfo = iter.second.qtSvrInfo;
			inaddr.S_un.S_addr = iter.second.intIP;
			upstream = string(qtInfo.m_strIp) + string(":");
			ss << qtInfo.m_shtPort;
			upstream += ss.str().c_str();
			if (full_list_selected_column_no_ != 65535)
				row_no = map_column_sorted_state_[full_list_selected_column_no_].map_sorted_row_no[row_no];
			full_list_ctrl_.SetItemText(row_no, 3, upstream.c_str());
		}
	}
	// Update downstream information.
	for (auto iter : sub_trans) {
		char pTmp[256] = { 0 };
		unsigned short shtTransID = iter / 1000;
		if (m_mapTransFullListPos.find(shtTransID) != m_mapTransFullListPos.end()) {
			auto row_no = m_mapTransFullListPos[shtTransID];
			if (full_list_selected_column_no_ != 65535)
				row_no = map_column_sorted_state_[full_list_selected_column_no_].map_sorted_row_no[row_no];
			sprintf(pTmp, "[%d]", iter % 1000);
			full_list_ctrl_.SetItemText(row_no, 4, pTmp);
			if (iter % 1000 >= 15) {
				full_list_ctrl_.PushSelfDefinedColor(row_no, 4, RGB(255, 0, 0), RGB(255, 255, 255));
			} else {
				full_list_ctrl_.EraseSelfDefinedColor(row_no, 4);
			}
		}
	}
	// Update market information
	for (auto iter = market_items.begin(); iter != market_items.end(); ++iter) {
		if (m_mapMarketFullListPos.find(iter->shtMarketID) != m_mapMarketFullListPos.end() &&
			m_mapTransFullListPos.find(iter->shtTransID) != m_mapTransFullListPos.end()) {
			latestMarketItems_[iter->shtTransID * 1000 + iter->shtMarketID] = *iter;
			unsigned short intXPos = m_mapTransFullListPos[iter->shtTransID];
			auto original_row_no = intXPos;	
			if (full_list_selected_column_no_ != 65535)
				intXPos = map_column_sorted_state_[full_list_selected_column_no_].map_sorted_row_no[intXPos];
			unsigned short intYPos = m_mapMarketFullListPos[iter->shtMarketID];
			auto tradeSequence = (iter->intTradeSeqs >= 20170101 && iter->intTradeSeqs <= 20171231) ?
				iter->intTradeSeqs % 20170000 : iter->intTradeSeqs;
			auto tradeDate = (iter->intTradeDate >= 20170101 && iter->intTradeDate <= 20171231) ?
				iter->intTradeDate % 20170000 : iter->intTradeDate;
			char pTmp[30] = { 0 };
			if (iter->intTradeSeqs != iter->intTradeDate) {
				sprintf(pTmp, "%04u[%04u %06u]", tradeSequence, tradeDate, iter->intTradeTime);
			} else {
				sprintf(pTmp, "%04u : %06u", tradeSequence, iter->intTradeTime);
			}
			auto iter2 = standard_times.find(iter->shtMarketID);
			if (iter2 != standard_times.end()) {
				if (iter2->second.intTradeSeqs != iter->intTradeSeqs ||
					iter2->second.intTradeDate != iter->intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 10) {
					full_list_ctrl_.PushSelfDefinedColor(intXPos, intYPos, RGB(0, 0, 0), RGB(254, 254, 65));
					cellsSelfDefinedColor_.insert({ original_row_no * 1000 + intYPos, ItemColorState(RGB(0, 0, 0), RGB(254, 254, 65)) });
				} else {
					full_list_ctrl_.EraseSelfDefinedColor(intXPos, intYPos);
					cellsSelfDefinedColor_.erase(original_row_no * 1000 + intYPos);
				}
			}
			full_list_ctrl_.SetItemText(intXPos, intYPos, pTmp);
		}
	}
	calculation_server_dialog_.UpdateListCtrl(trans_defines, sub_trans, market_items, standard_times);
	windows_mini_lib::autoAdjustColumnWidth(&full_list_ctrl_);
	full_list_ctrl_.SetRedraw(TRUE);
	full_list_ctrl_.UpdateWindow();
}

// Override this function for better user experience.
void CTransDialogDelay::OnOK() {
	// TODO: Add your specialized code here and/or call the base class
	// Do not exit the program here.
	return;
	//CDialog::OnOK();
}

void CTransDialogDelay::OnLvnFullListColumnclick(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	full_list_selected_column_no_ = pNMLV->iSubItem;
	map_column_sorted_state_[full_list_selected_column_no_];
	int item_count = full_list_ctrl_.GetItemCount();
	for (int i = 0; i < item_count; ++i)  {
		// The second parameter is the first and the second parameters in SortColProc.
		full_list_ctrl_.SetItemData(i, i);
	}
	full_list_ctrl_.SortItems(SortListCtrl, (DWORD_PTR)this);			// The second parameter is the third parameter in SortColProc.

	auto& column_sorted_state = map_column_sorted_state_[full_list_selected_column_no_];
	for (int k = 0; k < item_count; ++k) {
		CString	cstring_original_row_no = full_list_ctrl_.GetItemText(k, 0);
		unsigned short original_row_no = atoi(cstring_original_row_no) - 1;
		column_sorted_state.map_sorted_row_no[original_row_no] = k;
	}

	full_list_ctrl_.GetVisualHeaderCtrl().SetSortImage(full_list_selected_column_no_, !column_sorted_state.visual_arrow_flag);
	column_sorted_state.SetVisualArrowFlag();
	UpdateColoredItems(full_list_ctrl_);
	*pResult = 0;
}

int CALLBACK CTransDialogDelay::SortListCtrl(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	int					row1 = (int)lParam1;
	int					row2 = (int)lParam2;
	CTransDialogDelay*	pDlg = (CTransDialogDelay*)lParamSort;
	auto selected_column_no  = pDlg->GetFullListSelectedColumnNo();
	CString				lp1  = pDlg->full_list_ctrl_.GetItemText(row1, selected_column_no);
	CString				lp2  = pDlg->full_list_ctrl_.GetItemText(row2, selected_column_no);
	if (selected_column_no == 0) {
		if (pDlg->map_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return atoi(lp1) - atoi(lp2);
		} else {
			return atoi(lp2) - atoi(lp1);
		}
	} else if (selected_column_no == 4) {
		int sub_trans1 = 0;
		int sub_trans2 = 0;
		windows_mini_lib::ExtractFirstNumericFromCString(lp1, sub_trans1);
		windows_mini_lib::ExtractFirstNumericFromCString(lp2, sub_trans2);
		if (pDlg->map_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return sub_trans1 - sub_trans2;
		} else {
			return sub_trans2 - sub_trans1;
		}
	} else if (selected_column_no == 1) {
		auto server_ip1 = pDlg->full_list_ctrl_.GetItemText(row1, selected_column_no + 1);
		auto server_ip2 = pDlg->full_list_ctrl_.GetItemText(row2, selected_column_no + 1);
		auto server_id1 = pDlg->map_server_ip_to_id_[server_ip1.GetBuffer(server_ip1.GetLength())];
		auto server_id2 = pDlg->map_server_ip_to_id_[server_ip2.GetBuffer(server_ip2.GetLength())];
		if (pDlg->map_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return server_id2 - server_id1;
		} else {
			return server_id1 - server_id2;
		}
	} else {
		if (pDlg->map_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return lp1.CompareNoCase(lp2);
		} else {
			return lp2.CompareNoCase(lp1);
		}
	}
	return 0;
}

void CTransDialogDelay::UpdateColoredItems(CZListCtrl& zlist_ctrl) {
	zlist_ctrl.ClearSelfDefinedColor();
	for (auto row = 0; row < zlist_ctrl.GetItemCount(); ++row) {
		auto server_name = zlist_ctrl.GetItemText(row, 1);
		if (server_name.Find("主") != -1)
			zlist_ctrl.PushSelfDefinedColor(row, 1, RGB(0, 180, 80), RGB(255, 255, 255));
		// Push color for sub trans overloaded.
		auto cstring_sub_trans = zlist_ctrl.GetItemText(row, 4);
		auto sub_trans = 0;
		windows_mini_lib::ExtractFirstNumericFromCString(cstring_sub_trans, sub_trans);
		if (sub_trans >= 15)
			zlist_ctrl.PushSelfDefinedColor(row, 4, RGB(255, 0, 0), RGB(255, 255, 255));
	}
	for (const auto& iter : cellsSelfDefinedColor_) {
		auto row_no = iter.first / 1000;
		auto column_no = iter.first % 1000;
		row_no = map_column_sorted_state_[full_list_selected_column_no_].map_sorted_row_no[row_no];
		zlist_ctrl.PushSelfDefinedColor(row_no, column_no, iter.second.item_text_color, iter.second.item_background_color);
	}
}

void CTransDialogDelay::UpdateMarketStandardTimes(std::map<unsigned short, SMarketItem>& standard_times) {
	for (auto& iter : standard_times) {
		if (market_standard_times_[iter.first] < iter.second)
			market_standard_times_[iter.first] = iter.second;
	}
}

void CTransDialogDelay::OnRclickFullList(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CMenu menu;
	if (menu.LoadMenu(IDR_MENU1)) {
		CMenu* popup_menu = menu.GetSubMenu(0);
		if (popup_menu) {
			CPoint cursor_point;
			GetCursorPos(&cursor_point);
			popup_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, cursor_point.x, cursor_point.y, this);
		}
	}
	*pResult = 0;
}

void CTransDialogDelay::OnRclickSummaryList(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CMenu menu;
	if (menu.LoadMenu(IDR_MENU3)) {
		CMenu* popup_menu = menu.GetSubMenu(0);
		if (popup_menu) {
			CPoint cursor_point;
			GetCursorPos(&cursor_point);
			popup_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, cursor_point.x, cursor_point.y, this);
		}
	}
	*pResult = 0;
}

void CTransDialogDelay::OnMenuCopyServerIp() {
	// TODO: Add your command handler code here
	unsigned int selected_count = full_list_ctrl_.GetSelectedCount();
	if (selected_count == 1) {
		auto row = -1;
		row = full_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
		ASSERT(row != -1);
		auto server_ip = full_list_ctrl_.GetItemText(row, 2);
		if (!windows_mini_lib::CopyContentToClipboard(m_hWnd, server_ip))
			AfxMessageBox(_T("Fail to copy server IP"));
	}
}

void CTransDialogDelay::OnMenuCopyUpstreamIp() {
	// TODO: Add your command handler code here
	unsigned int selected_count = full_list_ctrl_.GetSelectedCount();
	if (selected_count == 1) {
		auto row = -1;
		row = full_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
		ASSERT(row != -1);
		auto upstream_info = full_list_ctrl_.GetItemText(row, 3);
		auto upstream_ips = windows_mini_lib::StringParser(string(upstream_info.GetBuffer(upstream_info.GetLength())), ":");
		if (!windows_mini_lib::CopyContentToClipboard(m_hWnd, upstream_ips[0].c_str()))
			AfxMessageBox(_T("Fail to copy upstream IP"));
	}
}

bool CTransDialogDelay::isTransSearchingDialogCreated() {
	transSearchingDialog_ = new TransSearchingDialog(this);
	if (transSearchingDialog_) {
		auto ret = transSearchingDialog_->Create(IDD_DIALOG_SEARCH_TRANS, this);
		if (ret) {
			setWindowInitialPosition(transSearchingDialog_);
			transSearchingDialog_->ShowWindow(SW_HIDE);
			return true;
		}
	}
	return false;
}

bool CTransDialogDelay::isTransDetailsDialogCreated() {
	transDetailsDialog_ = new TransDetailsDialog();
	if (transDetailsDialog_) {
		auto ret = transDetailsDialog_->Create(IDD_DIALOG_TRANS_DETAILS, this);
		if (ret) {
			setWindowInitialPosition(transDetailsDialog_);
			transDetailsDialog_->ShowWindow(SW_HIDE);
			return true;
		}
	}
	return false;
}

void CTransDialogDelay::showSearchingDialog() {
	transSearchingDialog_->ShowWindow(SW_SHOW);
	transSearchingDialog_->setComboboxFocused();
}

void CTransDialogDelay::showDetailsDialog() {
	transDetailsDialog_->ShowWindow(SW_SHOW);
}

void CTransDialogDelay::setWindowInitialPosition(CDialog* dialog) {
	// Get the size of the screen
	int screenXSize = ::GetSystemMetrics(SM_CXSCREEN);
	int screenYSize = ::GetSystemMetrics(SM_CYSCREEN);
	// Set the size of the window  
	int cx = (screenXSize * 4) / 10;
	int cy = (screenYSize * 3) / 12;
	// Set the initial position on the screen  
	int x = (screenXSize - cx) - 500;
	int y = (screenYSize - cy) / 2 - 12;

	dialog->MoveWindow(CRect(x, y, cx + x, cy + y));
}

void CTransDialogDelay::responceComboboxSelection(int selected_index) {
	// TO DO in the future...
}

void CTransDialogDelay::responceComboboxInput(std::string text_input) {
	auto split_words = windows_mini_lib::StringParser(text_input, " ,./@#$%&*");
	std::vector<unsigned short> target_marketIDs;
	for (auto& word : split_words) {
		bool isNumber = (word.find_first_not_of("0123456789") == string::npos);
		if (!isNumber) {
			// In case of lower case input.
			std::transform(word.begin(), word.end(), word.begin(), ::toupper);
			auto iter = m_mapMarketNameToID.find(word);
			if (iter != m_mapMarketNameToID.end())
				target_marketIDs.push_back(iter->second);
		} else {
			auto marketID = std::stoi(word, nullptr);
			if (m_mapMarketFullListPos.find(marketID) != m_mapMarketFullListPos.end())
				target_marketIDs.push_back(marketID);
		}
	}
	CTransDialog* markets_info_dialog = new CTransDialog(target_marketIDs, markets_, m_mapDefsChild, m_vecSubsChild,
		m_vecDataChild, market_standard_times_);
	markets_info_dialog->Create(IDD_DIALOG_TRANS);
	markets_info_dialog->ShowWindow(SW_SHOW);
	markets_info_dialogs_.push_back(markets_info_dialog);
	return;
}

BOOL CTransDialogDelay::PreTranslateMessage(MSG* pMsg) {
	// TODO: Add your specialized code here and/or call the base class
	if (TranslateAccelerator(m_hWnd, acceleratorSearch_, pMsg)) {
		showSearchingDialog();
		return true;
	} else if (TranslateAccelerator(m_hWnd, acceleratorShowDetails_, pMsg)) {
		showDetailsDialog();
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTransDialogDelay::OnCancel() {
	// TODO: Add your specialized code here and/or call the base class
	// Comment out the default statement and deal with the details in OnClose().
	return;
	//__super::OnCancel();
}

void CTransDialogDelay::updateMarketCellStates(const std::map<unsigned int, SMarketItem>& market_items, 
	const std::map<unsigned short, SMarketItem>& market_standard_times) {
	full_list_ctrl_.SetRedraw(FALSE);
	for (const auto& iter : market_items) {
		auto market_id = iter.first % 1000;
		auto trans_id = iter.first / 1000;
		if (m_mapMarketFullListPos.find(market_id) != m_mapMarketFullListPos.end() &&
			m_mapTransFullListPos.find(trans_id) != m_mapTransFullListPos.end()) {
			unsigned short row_no = m_mapTransFullListPos[trans_id];
			auto original_row_no = row_no;
			if (full_list_selected_column_no_ != 65535)
				row_no = map_column_sorted_state_[full_list_selected_column_no_].map_sorted_row_no[row_no];
			unsigned short column_no = m_mapMarketFullListPos[market_id];
			auto iter2 = market_standard_times.find(market_id);
			if (iter2 != market_standard_times.end()) {
				if (iter2->second.intTradeSeqs != iter.second.intTradeSeqs ||
					iter2->second.intTradeDate != iter.second.intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter.second.intTradeTime))) > 10) {
					full_list_ctrl_.PushSelfDefinedColor(row_no, column_no, RGB(0, 0, 0), RGB(254, 254, 65));
					cellsSelfDefinedColor_.insert({ original_row_no * 1000 + column_no, ItemColorState(RGB(0, 0, 0), RGB(254, 254, 65)) });
				} else {
					full_list_ctrl_.EraseSelfDefinedColor(row_no, column_no);
					cellsSelfDefinedColor_.erase(original_row_no * 1000 + column_no);
				}
			}
		}
	}
	full_list_ctrl_.SetRedraw(TRUE);
	full_list_ctrl_.UpdateWindow();
}

void CTransDialogDelay::OnDetailsDetailsmarket() {
	std::vector<unsigned short> marketIDs;
	int row = -1;
	unsigned int selected_count = summary_list_ctrl_.GetSelectedCount();
	for (unsigned int counter = 0; counter < selected_count; ++counter) {
		row = summary_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
		ASSERT(row != -1);
		CString	strMarketID = summary_list_ctrl_.GetItemText(row, 1);
		unsigned int marketID = static_cast<unsigned int>(atoi(strMarketID));
		marketIDs.push_back(marketID);
	}
	CTransDialog* markets_info_dialog = new CTransDialog(marketIDs, markets_, m_mapDefsChild, m_vecSubsChild,
		m_vecDataChild, market_standard_times_);
	markets_info_dialog->Create(IDD_DIALOG_TRANS);
	markets_info_dialog->ShowWindow(SW_SHOW);
	markets_info_dialogs_.push_back(markets_info_dialog);
}

void CTransDialogDelay::updateSummaryListCells(const std::vector<SMarketItem>& market_items,
	const std::map<unsigned short, SMarketItem>& market_standard_times) {
	for (auto iter = market_items.begin(); iter != market_items.end(); ++iter) {
		std::string serverName(servers_[iter->shtTransID]);							// Get the server's Chinese name.
		auto& shtMarketID = iter->shtMarketID;
		if (m_mapMarketListPos.find(shtMarketID) != m_mapMarketListPos.end() &&
			serverName.find("(调)") == std::string::npos &&
			serverName.find("计算") == std::string::npos &&
			serverName.find("转发真如") == std::string::npos &&
			serverName.find("(港)") == std::string::npos) {
			unsigned short iRow = m_mapMarketListPos[shtMarketID];
			m_mapMarketMaxTime[iRow] = (m_mapMarketMaxTime[iRow] < *iter) ? *iter : m_mapMarketMaxTime[iRow];
			auto iter2 = market_standard_times_.find(shtMarketID);
			if (iter2 != end(market_standard_times_)) {
				if ((iter2->second.intTradeSeqs != iter->intTradeSeqs ||
					iter2->second.intTradeDate != iter->intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 10)) {
					m_mapColoredItem[iRow][iter->shtTransID] = true;
				} else {
					m_mapColoredItem[iRow][iter->shtTransID] = false;
				}
			}
		}
	}
	// Decide which row to push/erase color.
	for (auto it : m_mapColoredItem) {
		unsigned int counter = 0;
		for (auto it1 : it.second) {
			if (it1.second)
				++counter;
		}
		unsigned short row_no = it.first;
		if (market_list_selected_column_no_ != 65535)
			row_no = map_market_list_column_sorted_state_[market_list_selected_column_no_].map_sorted_row_no[row_no];
		if (counter > 0 && counter < 10) {
			for (int column = 0; column < 4; ++column)
				summary_list_ctrl_.PushSelfDefinedColor(row_no, column, RGB(0, 0, 0), RGB(254, 254, 65));
		} else if (counter >= 10) {
			for (int column = 0; column < 4; ++column)
				summary_list_ctrl_.PushSelfDefinedColor(row_no, column, RGB(138, 43, 226), RGB(255, 201, 147));
		} else {
			for (int column = 0; column < 4; ++column)
				summary_list_ctrl_.EraseSelfDefinedColor(row_no, column);
		}
	}
}

//void CTransDialogDelay::OnDetailsDisablepopup() {
//	// TODO: Add your command handler code here
//	int row = -1;
//	unsigned int selected_count = summary_list_ctrl_.GetSelectedCount();
//	for (unsigned int counter = 0; counter < selected_count; ++counter) {
//		row = summary_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
//		ASSERT(row != -1);
//		CString	strMarketID = summary_list_ctrl_.GetItemText(row, 1);
//		unsigned short marketID = static_cast<unsigned short>(atoi(strMarketID));
//		marketsPopupDisabled_.insert(marketID);
//	}
//}
//
//void CTransDialogDelay::OnDetailsEnablepopup() {
//	// TODO: Add your command handler code here
//	int row = -1;
//	unsigned int selected_count = summary_list_ctrl_.GetSelectedCount();
//	for (unsigned int counter = 0; counter < selected_count; ++counter) {
//		row = summary_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
//		ASSERT(row != -1);
//		CString	strMarketID = summary_list_ctrl_.GetItemText(row, 1);
//		unsigned short marketID = static_cast<unsigned short>(atoi(strMarketID));
//		marketsPopupDisabled_.erase(marketID);
//	}
//}

void CTransDialogDelay::OnDetailsPickupacolor() {
	// TODO: Add your command handler code here
	CColorDialog colorDialog;
	if (colorDialog.DoModal() == IDOK) {
		COLORREF targetColor = colorDialog.GetColor();
		unsigned int selected_count = full_list_ctrl_.GetSelectedCount();
		if (selected_count == 1) {
			auto row = -1;
			row = full_list_ctrl_.GetNextItem(row, LVNI_SELECTED);
			ASSERT(row != -1);
			// The default background color is white.
			full_list_ctrl_.PushSelfDefinedColor(row, 1, targetColor, RGB(255, 255, 255));
			cellsSelfDefinedColor_[row * 1000 + 1] = std::move(ItemColorState(targetColor, RGB(255, 255, 255)));
		}
	}
}

void CTransDialogDelay::setConnectedIPNotifiedTime(const unsigned short& connection_id) {
	auto notifiedTime = windows_mini_lib::GetCurrentTime();
	if (connection_id == 0)
		transDetailsDialog_->refreshTime1_.SetWindowTextA(notifiedTime.c_str());
	else if (connection_id == 1)
		transDetailsDialog_->refreshTime2_.SetWindowTextA(notifiedTime.c_str());
	else if (connection_id == 2)
		transDetailsDialog_->refreshTime3_.SetWindowTextA(notifiedTime.c_str());
}

void CTransDialogDelay::forceUpdateAllMarketCells() {
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	if (std::abs(current_time.wSecond - baseTime_.wSecond) > 10) {
		updateMarketCellStates(latestMarketItems_, market_standard_times_);
		// Update the child dialogs.
		std::map<unsigned short, STransDefine> transDefines;
		std::vector<int> subTrans;
		std::vector<SMarketItem> marketItems;
		std::transform(latestMarketItems_.begin(), latestMarketItems_.end(),
			std::back_inserter(marketItems), [](std::pair<unsigned int, SMarketItem> const& iter) {
			return iter.second;
		});
		updateSummaryListCells(marketItems, market_standard_times_);
		for (auto& pair_market_info_dialog : market_info_dialogs_)
			pair_market_info_dialog.second->UpdateDetails(transDefines, subTrans, marketItems, market_standard_times_);
		for (auto& markets_info_dialog : markets_info_dialogs_)
			markets_info_dialog->UpdateDetails(transDefines, subTrans, marketItems, market_standard_times_);
		GetLocalTime(&baseTime_);
	}
}
