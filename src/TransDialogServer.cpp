// TRANSDilag.cpp : 实现文件
//

#include "stdafx.h"
#include "TransDialogServer.h"

IMPLEMENT_DYNAMIC(CTransDialogServer, CDialog)

CTransDialogServer::CTransDialogServer(CWnd* pParent /*=NULL*/)
									: CDialog(CTransDialogServer::IDD, pParent),
									serverToDisplay_(0), selected_column_no_(65535) {
}

CTransDialogServer::CTransDialogServer(unsigned short serverToDisplay, PriorMap& codeMap, map<unsigned short, STransDefine>& mapDefs,
									vector<int>& vecSubs, vector<SMarketItem>& vecData,
									map<unsigned short, SMarketItem>& mapStandardTime, CWnd* pParent)
									: CDialog(CTransDialogServer::IDD, pParent),
									selected_column_no_(65535) {
	serverToDisplay_ = serverToDisplay;
	m_mapCodeMap = codeMap;
	m_mapDefs = mapDefs;
	m_vecSubs = vecSubs;
	m_vecData = vecData;
	m_mapStandardTime = mapStandardTime;
}

CTransDialogServer::~CTransDialogServer() {
	CDialog::OnClose();
}

void CTransDialogServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRANSSERVER, m_list);
}


BEGIN_MESSAGE_MAP(CTransDialogServer, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TRANSSERVER, &CTransDialogServer::OnLvnItemchangedListTrans)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TRANSSERVER, &CTransDialogServer::OnLvnColumnclick)
END_MESSAGE_MAP()


void CTransDialogServer::OnPaint() {
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

void CTransDialogServer::setInitialPosition() {
	// Get the size of the screen
	int xSize = ::GetSystemMetrics(SM_CXSCREEN);
	int ySize = ::GetSystemMetrics(SM_CYSCREEN);
	// Set the size of the window  
	int cx = (xSize * 4) / 10 - 380;
	int cy = (ySize * 3) / 4 + 220;
	// Set the initial position on the screen  
	int x = (xSize - cx) - 400;
	int y = (ySize - cy) / 2 - 12;

	CWnd::MoveWindow(CRect(x, y, cx + x, cy + y));
}

void CTransDialogServer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here

	// If the dialog is minisized, cx == 0 && cy == 0. This will cause error in ChangeSize().
	if (nType != SIZE_MINIMIZED && cx != 0 && cy != 0) {
		// Lay out the list ctrl.
		CRect rect;
		GetClientRect(&rect);
		CWnd* pWnd = GetDlgItem(IDC_LIST_TRANSSERVER);
		if (pWnd)
			pWnd->MoveWindow(rect);
	}
}
HCURSOR CTransDialogServer::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTransDialogServer::OnClose() {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);
	//CDialog::OnClose();
}


BOOL CTransDialogServer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	// INIT_EASYSIZE;
	SetIcon(m_hIcon, TRUE);         // 设置大图标
	SetIcon(m_hIcon, FALSE);        // 设置小图标
	windows_mini_lib::SetListCtrlStyle(&m_list);
	// Make every Detailed Dialog independent on the taskbar.
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	setInitialPosition();
	ShowWindow(SW_SHOW);
	// set virtual list count
	m_list.SetItemCount(4 * m_mapCodeMap.size());
	// Set the caption of each column
	m_list.InsertColumn(0, _T("No."), LVCFMT_CENTER, 40);
	m_list.InsertColumn(1, _T("ID."), LVCFMT_CENTER, 50);
	m_list.InsertColumn(2, _T("Market"), LVCFMT_CENTER, 80);

	// Set basic information
	if (!m_mapDefs.empty()) {
		int j = 0;
		m_mapTransListPos.clear();
		for (auto iter : m_mapDefs) {
			m_mapTransListPos[iter.first] = j;
			if (iter.first == serverToDisplay_) {
				m_list.InsertColumn(3, _T(iter.second.szName), LVCFMT_CENTER, 168);
				m_strDlgName = iter.second.szName + string("--Server Detailed");
				CWnd::SetWindowText(_T(m_strDlgName.c_str()));
			}
			++j;
		}
		m_mapDefs.clear();
	}
	int i = 6;
	for (PriorMap::iterator it = m_mapCodeMap.begin(); it != m_mapCodeMap.end(); it++) {
		if (it->first != 100) {
			auto row = i - 6;
			m_list.InsertItem(row, to_string(row + 1).c_str());
			m_list.SetItemText(row, 1, to_string(it->first).c_str());
			m_list.SetItemText(row, 2, it->second.c_str());
			m_mapMarketListPos[it->first] = i++;
		}
	}
	// Set market information
	char pTmp[256] = { 0 };
	auto vecData = m_vecData;
	auto mapStandardTime = m_mapStandardTime;
	for (auto iter = vecData.begin(); iter != vecData.end(); ++iter) {
		unsigned short shtMarketID = iter->shtMarketID;
		if (m_mapMarketListPos.find(shtMarketID) != end(m_mapMarketListPos) &&
			iter->shtTransID == serverToDisplay_) {
			unsigned short intXPos = m_mapMarketListPos[shtMarketID] - 6;				// The real row number of the market.
			unsigned short intYPos = 3;
			sprintf(pTmp, "%d[%d %d]", iter->intTradeSeqs, iter->intTradeDate, iter->intTradeTime);
			auto iter2 = mapStandardTime.find(shtMarketID);
			if (iter2 != end(mapStandardTime)) {
				if (iter2->second.intTradeSeqs != iter->intTradeSeqs ||
					iter2->second.intTradeDate != iter->intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 10) {
					for (int column = 0; column < 4; ++column)
						m_list.PushColored(intXPos, column);
				} else {
					for (int column = 0; column < 4; ++column)
						m_list.EraseColored(intXPos, column);
				}
			}
			m_list.SetItemText(intXPos, intYPos, pTmp);
		}
	}

	return TRUE;  //return TRUE unless you set the focus to a control
}
void CTransDialogServer::UpdateDetails(const vector <SMarketItem>& _vecData, const map<unsigned short, SMarketItem>& _mapStandardTime)
{
	m_list.SetRedraw(FALSE);
	// Update market informaiton
	char pTmp[256] = { 0 };
	for (auto iter = _vecData.begin(); iter != _vecData.end(); ++iter) {
		unsigned short shtMarketID = iter->shtMarketID;
		if (m_mapMarketListPos.find(shtMarketID) != end(m_mapMarketListPos) &&
			iter->shtTransID == serverToDisplay_) {
			unsigned short intXPos = m_mapMarketListPos[shtMarketID] - 6;				// The real row number of the market.
			if (selected_column_no_ != 65535) {
				intXPos = map_column_sorted_state_[selected_column_no_].map_sorted_row_no[intXPos];
			}
			unsigned short intYPos = 3;
			sprintf(pTmp, "%d[%d %d]", iter->intTradeSeqs, iter->intTradeDate, iter->intTradeTime);
			auto iter2 = _mapStandardTime.find(shtMarketID);
			if (iter2 != end(_mapStandardTime)) {
				if (iter2->second.intTradeSeqs != iter->intTradeSeqs ||
					iter2->second.intTradeDate != iter->intTradeDate ||
					abs(int(GetSeconds(iter2->second.intTradeTime) - GetSeconds(iter->intTradeTime))) > 20) {
					for (int column = 0; column < 4; ++column)
						m_list.PushColored(intXPos, column);
				} else {
					for (int column = 0; column < 4; ++column)
						m_list.EraseColored(intXPos, column);
				}
			}
			m_list.SetItemText(intXPos, intYPos, pTmp);
		}
	}
	// To push color for the whole row successfully.	
	for (int k = 0; k < m_list.GetItemCount(); ++k) {
		for (int m = 0; m < 4; ++m) {
			CString	text = m_list.GetItemText(k, m);
			m_list.SetItemText(k, m, text);
		}
	}
	m_list.SetRedraw(TRUE);
	m_list.UpdateWindow();
}

void CTransDialogServer::OnLvnItemchangedListTrans(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTransDialogServer::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
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
	UpdateDetails(m_vecData, m_mapStandardTime);
	*pResult = 0;
}

// The sorting function
int CALLBACK CTransDialogServer::SortColProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// Get the row numbers to compare
	int					row1 = (int)lParam1;
	int					row2 = (int)lParam2;
	CTransDialogServer*	pDlg = (CTransDialogServer*)lParamSort;
	auto selected_column_no  = pDlg->GetSelectedCol();
	CString				lp1  = pDlg->m_list.GetItemText(row1, selected_column_no);
	CString				lp2  = pDlg->m_list.GetItemText(row2, selected_column_no);

	if (selected_column_no == 0 || selected_column_no == 1) {
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

