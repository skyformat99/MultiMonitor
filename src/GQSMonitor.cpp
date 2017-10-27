// GQSMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "GQSMonitor.h"
#include "afxdialogex.h"


// GQSMonitor dialog

IMPLEMENT_DYNAMIC(GQSMonitor, CDialog)

GQSMonitor::GQSMonitor(CWnd* pParent /*=NULL*/)
	: CDialog(GQSMonitor::IDD, pParent), 
	connected_ip_number_(0),
	list_ctrl_initialized_(false) {
}

GQSMonitor::~GQSMonitor() {
	for (auto& connection : connections_) {
		connection->StopThread();
		delete connection;
		connection = NULL;
	}
}

void GQSMonitor::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRANS_GQS, gqs_list_ctrl_);
}


BEGIN_MESSAGE_MAP(GQSMonitor, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_NOTIFYGQSLISTUPDATE, OnNotifyGQSListUpdate)
END_MESSAGE_MAP()


// GQSMonitor message handlers

void GQSMonitor::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}
}

void GQSMonitor::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	// TODO: add your code here.
}

HCURSOR GQSMonitor::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void GQSMonitor::OnClose() {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnClose();
}

void GQSMonitor::OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult) {
	NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;
	*pResult = 0;
}

BOOL GQSMonitor::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

	LONG lStyle;
	lStyle = GetWindowLong(gqs_list_ctrl_.m_hWnd, GWL_STYLE);				// 获取当前窗口style
	lStyle &= ~LVS_TYPEMASK;												// 清除显示方式位
	lStyle |= LVS_REPORT;													// 设置style
	SetWindowLong(gqs_list_ctrl_.m_hWnd, GWL_STYLE, lStyle);				// 设置style
	DWORD dwStyle = gqs_list_ctrl_.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	gqs_list_ctrl_.SetExtendedStyle(dwStyle);								// 设置扩展风格
	gqs_list_ctrl_.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	CRect rc;
	this->GetClientRect(&rc);
	rc.top += 60;
	rc.bottom -= 8;
	rc.left += 8;
	rc.right -= 8;
	gqs_list_ctrl_.MoveWindow(&rc);

	LoadGQSServer("./server.xml");

	gqs_list_ctrl_.InsertColumn(0, _T("No."), LVCFMT_CENTER, 40);
	gqs_list_ctrl_.InsertColumn(1, _T("Server Name"), LVCFMT_CENTER, 108);
	gqs_list_ctrl_.InsertColumn(2, _T("IP"), LVCFMT_LEFT, 108);
	gqs_list_ctrl_.InsertColumn(3, _T("Port"), LVCFMT_CENTER, 60);
	gqs_list_ctrl_.InsertColumn(4, _T("Status"), LVCFMT_CENTER, 168);
	gqs_list_ctrl_.InsertColumn(5, _T("GQS Version"), LVCFMT_CENTER, 130);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void GQSMonitor::InitGQSBasicInfo() {
	gqs_list_ctrl_.SetRedraw(FALSE);
	auto row_no = gqs_list_ctrl_.GetItemCount();
	for (auto& iter : total_basic_info_) {
		gqs_list_ctrl_.InsertItem(row_no, to_string(row_no + 1).c_str());
		gqs_list_ctrl_.SetItemText(row_no, 1, iter.second.server_name.c_str());
		gqs_list_ctrl_.SetItemText(row_no, 2, iter.second.server_ip.c_str());
		gqs_list_ctrl_.SetItemText(row_no, 3, to_string(iter.second.port).c_str());
		string connection_state = (iter.second.connected) ? CONNECTIONOK : CONNECTIONTIMEOUT;
		gqs_list_ctrl_.SetItemText(row_no, 4, connection_state.c_str());
		if (!iter.second.connected) {
			for (auto i = 0; i < 6; ++i)
				gqs_list_ctrl_.PushSelfDefinedColor(row_no, i);
		}
		servers_row_no_[iter.second.server_ip] = row_no++;
	}
	gqs_list_ctrl_.SetRedraw(TRUE);
	gqs_list_ctrl_.UpdateWindow();
}

void GQSMonitor::UpdateGQSBasicInfo(unsigned short connection_id) {
	gqs_list_ctrl_.SetRedraw(FALSE);
	std::vector<GQSServerInfo> basic_info;
	connections_[connection_id]->GetBasicInfo(basic_info);
	for (auto& info : basic_info) {
		if (servers_row_no_.find(info.server_ip) != servers_row_no_.end()) {
			auto row = servers_row_no_[info.server_ip];
			string connection_state = (info.connected) ? CONNECTIONOK : CONNECTIONTIMEOUT;
			gqs_list_ctrl_.SetItemText(row, 4, connection_state.c_str());
			if (!info.connected) {
				for (auto i = 0; i < 6; ++i)
					gqs_list_ctrl_.PushSelfDefinedColor(row, i);
			} else {
				for (auto i = 0; i < 6; ++i) {
					gqs_list_ctrl_.EraseSelfDefinedColor(row, i);
				}
			}
		}
	}
	// To push color for the whole row successfully.	
	for (int k = 0; k < gqs_list_ctrl_.GetItemCount(); ++k) {
		for (int m = 0; m < 6; ++m) {
			CString	text = gqs_list_ctrl_.GetItemText(k, m);
			gqs_list_ctrl_.SetItemText(k, m, text);
		}
	}
	gqs_list_ctrl_.SetRedraw(TRUE);
	gqs_list_ctrl_.UpdateWindow();
}

bool GQSMonitor::LoadGQSServer(const string& file_name) {
	std::lock_guard<std::mutex> lock(connections_mutex_);
	using boost::property_tree::ptree;
	try {
		ptree pt;
		read_xml(file_name, pt);
		{
			ptree serverlist = pt.get_child("serverlist");
			unsigned short connection_id = 0;
			for (ptree::iterator itr = serverlist.begin(); itr != serverlist.end(); itr++) {
				ptree svr = itr->second;
				string strServer = svr.get<string>("<xmlattr>.ip");
				int intPort = svr.get<int>("<xmlattr>.port");
				string strNote = svr.get<string>("<xmlattr>.note");
				bool blnSubTrans = svr.get<bool>("<xmlattr>.subtrans", false);
				GQSConnection* pCon = new GQSConnection;
				vector<unsigned short> vectMarket;
				pCon->InitConnect(strServer, intPort, m_hWnd, connection_id++);
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

LRESULT GQSMonitor::OnNotifyGQSListUpdate(WPARAM wp, LPARAM lp) {
	//bool initialized = wp;
	unsigned short connection_id = (unsigned short)lp;

	static bool connection_initialized[5]{ false };
	if (!connection_initialized[connection_id]) {
		++connected_ip_number_;
		std::vector<GQSServerInfo> basic_info;
		connections_[connection_id]->GetBasicInfo(basic_info);
		for (auto& iter : basic_info) {
			total_basic_info_[iter.server_ip] = iter;
		}
		connection_initialized[connection_id] = true;
	}

	if (!list_ctrl_initialized_) {
		std::lock_guard<std::mutex> lock(connections_mutex_);
		if (connected_ip_number_ == connections_.size()) {
			InitGQSBasicInfo();
			list_ctrl_initialized_ = true;
		}
	} else {
		UpdateGQSBasicInfo(connection_id);
	}

	return 0;
}



