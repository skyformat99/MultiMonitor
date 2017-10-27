// TRANSDilag.cpp : 实现文件
//

#include "stdafx.h"
#include "TransDialog.h"
#include "windows_mini_lib.h"

IMPLEMENT_DYNAMIC(CTransDialog, CDialog)

CTransDialog::CTransDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTransDialog::IDD, pParent),
	selected_column_no_(65535) {
}

CTransDialog::CTransDialog(std::vector<unsigned short> marketsToDisplay, PriorMap& codeMap, std::map<unsigned short, STransDefine>& mapDefs, 
	std::vector<int>& vecSubs, std::vector<SMarketItem>& vecData, std::map<unsigned short, SMarketItem>& mapStandardTime,
	CWnd* pParent)
	: CDialog(CTransDialog::IDD, pParent),
	selected_column_no_(65535) {
	marketsToDisplay_ = marketsToDisplay;
	m_mapCodeMap = codeMap;
	m_mapDefs = mapDefs;
	m_vecSubs = vecSubs;
	m_vecData = vecData;
	m_mapStandardTime = mapStandardTime;
}

CTransDialog::~CTransDialog() {
	CDialog::OnClose();
}

void CTransDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TRANS, m_list);
}

BEGIN_MESSAGE_MAP(CTransDialog, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TRANS, &CTransDialog::OnLvnItemchangedListTrans)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TRANS, &CTransDialog::OnLvnColumnclick)
END_MESSAGE_MAP()

void CTransDialog::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // Device Context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Set the icon in the center of the working rectangle. 
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw icon
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

void CTransDialog::OnSize(UINT nType, int cx, int cy) {
    CDialog::OnSize(nType, cx, cy);
    // TODO: Add your message handler code here
	// If the dialog is minisized, cx == 0 && cy == 0. This will cause error in ChangeSize().
	if (nType != SIZE_MINIMIZED && cx != 0 && cy != 0) {
		// Lay out the list ctrl.
		CRect rect;
		GetClientRect(&rect);
		CWnd* pWnd = GetDlgItem(IDC_LIST_TRANS);
		if (pWnd)
			pWnd->MoveWindow(rect);
	}
}

void CTransDialog::setInitialPosition() {
	// Get the size of the screen
	int xSize = ::GetSystemMetrics(SM_CXSCREEN);
	int ySize = ::GetSystemMetrics(SM_CYSCREEN);
	// Set the size of the window  
	int cx = (xSize * 4) / 12;
	int cy = (ySize * 3) / 4 + 220;
	// Set the initial position on the screen  
	int x = (xSize - cx) - 700;
	int y = (ySize - cy) / 2 - 12;

	CWnd::MoveWindow(CRect(x, y, cx + x, cy + y));
}

HCURSOR CTransDialog::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CTransDialog::OnClose() {
	// The default implementation of CDialog::OnClose() calls DestroyWindow.
	// But we just want to hide the window here and destroy the window in the destructor later.
	ShowWindow(SW_HIDE);
	//CDialog::OnClose();
}

BOOL CTransDialog::OnInitDialog() {
    CDialog::OnInitDialog();
    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    // 执行此操作
    // INIT_EASYSIZE;
    SetIcon(m_hIcon, TRUE);         // Set big icon.
    SetIcon(m_hIcon, FALSE);        // Set small icon.
	windows_mini_lib::SetListCtrlStyle(&m_list);
	// Make every Detailed Dialog independent on the taskbar.
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	setInitialPosition();
	ShowWindow(SW_SHOW);
	CWnd::SetWindowText(_T(" Market Details.."));
	// set virtual list count
	m_list.SetItemCount(m_mapCodeMap.size());
	// Set the caption of each column
	m_list.InsertColumn(0, _T("No."), LVCFMT_CENTER, 30);
	m_list.InsertColumn(1, _T("Server Name."), LVCFMT_CENTER, 100);
	m_list.InsertColumn(2, _T("Server IP"), LVCFMT_CENTER, 110);
	m_list.InsertColumn(3, _T("Upstream Address"), LVCFMT_CENTER, 130);
	m_list.InsertColumn(4, _T("Subs"), LVCFMT_CENTER, 45);
    int i = 5;															
    for (auto it : m_mapCodeMap) {
        if (it.first != 100 && std::find(marketsToDisplay_.begin(), marketsToDisplay_.end(), it.first) != marketsToDisplay_.end()) {
			m_mapMarketListPos[it.first] = i;
			m_list.InsertColumn(i, _T(it.second.c_str()), LVCFMT_CENTER, 110);
			++i;
		}
    }
	// Set basic information
	if (!m_mapDefs.empty()) {
		m_mapDefsChild = m_mapDefs;
		int j = 0;
		for (auto iter : m_mapDefs) {
			if (std::string(iter.second.szName).find("计算") == std::string::npos &&
				std::string(iter.second.szName).find("(调)") == std::string::npos &&
				std::string(iter.second.szName).find("转发真如") == std::string::npos &&
				std::string(iter.second.szName).find("(港)") == std::string::npos) {
				std::string upstream;
				stringstream ss;
				struct in_addr inaddr;
				SConInfo qtInfo = iter.second.qtSvrInfo;
				inaddr.S_un.S_addr = iter.second.intIP;
				upstream = string(qtInfo.m_strIp) + string(":");
				ss << qtInfo.m_shtPort;
				upstream += ss.str().c_str();
				m_list.InsertItem(j, to_string(j + 1).c_str());
				m_list.SetItemText(j, 1, iter.second.szName);
				m_list.SetItemText(j, 2, inet_ntoa(inaddr));
				m_list.SetItemText(j, 3, upstream.c_str());
				m_mapTransListPos[iter.first] = j++;
			}
		}
		m_mapDefs.clear();
	}
	// Initialize down stream information
	for (auto iter : m_vecSubs) {
		char pTmp[20] = { 0 };
		unsigned short shtTransID = iter / 1000;
		if (m_mapTransListPos.find(shtTransID) != m_mapTransListPos.end()) {
			int intXPos = m_mapTransListPos[shtTransID];
			sprintf(pTmp, "[%d]", iter % 1000);
			m_list.SetItemText(intXPos, 4, pTmp);
		}
	}
	// Set initial market information
	auto vecData = m_vecData;
	for (auto iter = vecData.begin(); iter != vecData.end(); ++iter) {
		unsigned short shtMarketID = iter->shtMarketID;
		if (m_mapMarketListPos.find(shtMarketID) != end(m_mapMarketListPos) &&
			m_mapTransListPos.find(iter->shtTransID) != m_mapTransListPos.end()) {
			unsigned short intXPos = m_mapTransListPos[iter->shtTransID];
			unsigned short intYPos = m_mapMarketListPos[shtMarketID];
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
			auto iter2 = m_mapStandardTime.find(shtMarketID);
			if (iter2 != end(m_mapStandardTime)) {
				if (iter2->second.intTradeSeqs != iter->intTradeSeqs ||
					iter2->second.intTradeDate != iter->intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 10) {
					m_list.PushColored(intXPos, intYPos);
				} else {
					m_list.EraseColored(intXPos, intYPos);
				}
			}
			m_list.SetItemText(intXPos, intYPos, pTmp);
		}
	}
	windows_mini_lib::autoAdjustColumnWidth(&m_list);
	return TRUE;  //return TRUE unless you set the focus to a control
}

void CTransDialog::UpdateDetails(const map<unsigned short, STransDefine>& _mapDefs, const vector<int>& _vecSubs,
	const vector<SMarketItem>& _vecData, const map<unsigned short, SMarketItem>& _mapStandardTime) {
	m_list.SetRedraw(FALSE);
	// Update upstream information
	for (auto iter : _mapDefs) {
		if (m_mapTransListPos.find(iter.first) != m_mapTransListPos.end()) {
			int row = m_mapTransListPos[iter.first];
			string upstream;
			stringstream ss;
			SConInfo qtInfo = iter.second.qtSvrInfo;
			upstream = string(qtInfo.m_strIp) + string(":");
			ss << qtInfo.m_shtPort;
			upstream += ss.str().c_str();
			if (selected_column_no_ != 65535) {
				row = map_column_sorted_state_[selected_column_no_].map_sorted_row_no[row];
			}
			m_list.SetItemText(row, 3, upstream.c_str());
		}
	}
	// Update downstream information
	for (auto iter : _vecSubs) {
		char pTmp[20] = { 0 };
		unsigned short shtTransID = iter / 1000;
		if (m_mapTransListPos.find(shtTransID) != m_mapTransListPos.end()) {
			int intXPos = m_mapTransListPos[shtTransID];
			if (selected_column_no_ != 65535) {
				intXPos = map_column_sorted_state_[selected_column_no_].map_sorted_row_no[intXPos];
			}
			sprintf(pTmp, "[%d]", iter % 1000);
			m_list.SetItemText(intXPos, 4, pTmp);
		}
	}
	// Update market informaiton
	for (auto iter = _vecData.begin(); iter != _vecData.end(); ++iter) {
		unsigned short shtMarketID = iter->shtMarketID;
		if (m_mapMarketListPos.find(shtMarketID) != end(m_mapMarketListPos) &&
			m_mapTransListPos.find(iter->shtTransID) != m_mapTransListPos.end()) {
			unsigned short intXPos = m_mapTransListPos[iter->shtTransID];
			unsigned short intYPos = m_mapMarketListPos[shtMarketID];
			if (selected_column_no_ != 65535) {
				intXPos = map_column_sorted_state_[selected_column_no_].map_sorted_row_no[intXPos];
			}
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
			auto iter2 = _mapStandardTime.find(shtMarketID);
			if (iter2 != end(_mapStandardTime)) {
				if (iter2->second.intTradeSeqs != iter->intTradeSeqs ||
					iter2->second.intTradeDate != iter->intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 10) {
					m_list.PushColored(intXPos, intYPos);
				} else {
					m_list.EraseColored(intXPos, intYPos);
				}
			}
			m_list.SetItemText(intXPos, intYPos, pTmp);
		}
	}
	m_list.SetRedraw(TRUE);
	m_list.UpdateWindow();
}

void CTransDialog::OnLvnItemchangedListTrans(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTransDialog::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	selected_column_no_ = pNMLV->iSubItem;						// The selected column
	map_column_sorted_state_[selected_column_no_];

	int item_count = m_list.GetItemCount();
	for (int i = 0; i < item_count; ++i) {
		// The second parameter is the first and the second parameters in SortColProc.
		m_list.SetItemData(i, i);
	}
	m_list.SortItems(SortColProc, (DWORD_PTR)this);			// The second parameter is the third parameter in SortColProc.

	auto& column_sorted_state = map_column_sorted_state_[selected_column_no_];
	for (int k = 0; k < item_count; ++k) {
		CString	cstring_original_row_no = m_list.GetItemText(k, 0);
		unsigned short original_row_no = atoi(cstring_original_row_no) - 1;
		column_sorted_state.map_sorted_row_no[original_row_no] = k;
	}

	m_list.GetVisualHeaderCtrl().SetSortImage(selected_column_no_, !column_sorted_state.visual_arrow_flag);
	column_sorted_state.SetVisualArrowFlag();
	UpdateDetails(m_mapDefsChild, m_vecSubs, m_vecData, m_mapStandardTime);
	*pResult = 0;
}

// The sorting function
int CALLBACK CTransDialog::SortColProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	// Get the row numbers to compare
	int				row1	= (int)lParam1;
	int				row2	= (int)lParam2;
	CTransDialog*	pDlg	= (CTransDialog*)lParamSort;
	auto selected_column_no = pDlg->GetSelectedCol();
	CString			lp1		= pDlg->m_list.GetItemText(row1, selected_column_no);
	CString			lp2		= pDlg->m_list.GetItemText(row2, selected_column_no);
	if (selected_column_no == 0) {
		if (pDlg->map_column_sorted_state_[selected_column_no].visual_arrow_flag) {
			return atoi(lp1) - atoi(lp2);
		} else {
			return atoi(lp2) - atoi(lp1);
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

