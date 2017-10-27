// MDSDialog.cpp : 实现文件
//

#include "stdafx.h"

IMPLEMENT_DYNAMIC(CMDSDialog, CDialog)

CMDSDialog::CMDSDialog(CWnd* pParent /*=NULL*/) 
	: CDialog(CMDSDialog::IDD, pParent), connected_ip_number_(0), 
	list_ctrl_initialized_(false) {
}

CMDSDialog::~CMDSDialog() {
	for (auto& connection : connections_) {
		connection->StopThread();
		delete connection;
		connection = NULL;
	}
}

void CMDSDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MDS, mds_list_ctrl_);
}

BEGIN_MESSAGE_MAP(CMDSDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_NOTIFYMDSLISTUPDATE, OnNotifyMDSListUpdate)
	ON_MESSAGE(WM_NOTIFYMDSVERSIONS, OnNotifyMDSVersions)
END_MESSAGE_MAP()

void CMDSDialog::OnPaint() {
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
	} else {
		CDialog::OnPaint();
	}
}

void CMDSDialog::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	// TODO: add your code here.
}
HCURSOR CMDSDialog::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void CMDSDialog::OnClose() {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnClose();
}

BOOL CMDSDialog::OnInitDialog() {
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	///INIT_EASYSIZE;
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon
	windows_mini_lib::SetListCtrlStyle(&mds_list_ctrl_);
	mds_list_ctrl_.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	CRect rc;
	this->GetClientRect(&rc);
	rc.top += 60;
	rc.bottom -= 8;
	rc.left += 8;
	rc.right -= 8;
	mds_list_ctrl_.MoveWindow(&rc);

	LoadMDSServer("./server.xml");

	mds_list_ctrl_.InsertColumn(0, _T("No."), LVCFMT_CENTER, 40);
	mds_list_ctrl_.InsertColumn(1, _T("Server Name"), LVCFMT_CENTER, 108);
	mds_list_ctrl_.InsertColumn(2, _T("IP"), LVCFMT_LEFT, 108);
	mds_list_ctrl_.InsertColumn(3, _T("Port"), LVCFMT_CENTER, 60);
	mds_list_ctrl_.InsertColumn(4, _T("Status"), LVCFMT_CENTER, 168);
	mds_list_ctrl_.InsertColumn(5, _T("MDS Version"), LVCFMT_CENTER, 130);

	return TRUE;  //return TRUE unless you set the focus to a control
}

bool CMDSDialog::LoadMDSServer(const string & strFileName) {
	std::lock_guard<std::mutex> lock(connections_mutex_);
	using boost::property_tree::ptree;
	try {
		ptree pt;
		read_xml(strFileName, pt);
		{
			ptree serverlist = pt.get_child("serverlist");
			unsigned short connection_id = 0;
			for (ptree::iterator itr = serverlist.begin(); itr != serverlist.end(); itr++) {
				ptree svr = itr->second;
				string strServer = svr.get<string>("<xmlattr>.ip");
				int intPort = svr.get<int>("<xmlattr>.port");
				string strNote = svr.get<string>("<xmlattr>.note");
				bool blnSubTrans = svr.get<bool>("<xmlattr>.subtrans", false);
				CMDSMultiConn* pCon = new CMDSMultiConn;
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

LRESULT CMDSDialog::OnNotifyMDSListUpdate(WPARAM wp, LPARAM lp) {
	//bool initialized = wp;
	unsigned short connection_id = (unsigned short)lp;

	static bool connection_initialized[5]{ false };
	if (!connection_initialized[connection_id]) {
		++connected_ip_number_;
		std::vector<MDSBasicInfo> basic_info;
		connections_[connection_id]->GetBasicInfo(basic_info);
		for (auto& iter : basic_info) {
			total_basic_info_[iter.host_info] = iter;
		}
		connection_initialized[connection_id] = true;
	}

	if (!list_ctrl_initialized_) {
		std::lock_guard<std::mutex> lock(connections_mutex_);
		if (connected_ip_number_ == connections_.size()) {
			InitMDSBasicInfo();
			list_ctrl_initialized_ = true;
		}
	} else {
		UpdateMDSBasicInfo(connection_id);
	}

	return 0;
}

LRESULT CMDSDialog::OnNotifyMDSVersions(WPARAM wp, LPARAM lp) {
	unsigned short connection_id = (unsigned short)lp;
	if (list_ctrl_initialized_) {
		std::vector<MsgMDSVersion> mds_versions;
		connections_[connection_id]->GetMDSVersions(mds_versions);
		for (auto& mds_version : mds_versions) {
			if (servers_row_no_.find(mds_version.host_info) != servers_row_no_.end()) {
				auto row = servers_row_no_[mds_version.host_info];
				mds_list_ctrl_.SetItemText(row, 5, mds_version.mds_all_version.c_str());
			}
		}
	} else {
		connections_[connection_id]->requestPacket(PACK_PUSH_PC_MDSVERSION);
	}
	return 0;
}

void CMDSDialog::OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult) {
	NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;
	*pResult = 0;
}

void CMDSDialog::UpdateMDSBasicInfo(unsigned short connection_id) {
	mds_list_ctrl_.SetRedraw(FALSE);
	std::vector<MDSBasicInfo> basic_info;
	connections_[connection_id]->GetBasicInfo(basic_info);
	for (auto& info : basic_info) {
		if (servers_row_no_.find(info.host_info) != servers_row_no_.end()) {
			auto row = servers_row_no_[info.host_info];
			string connection_state = (info.status) ? CONNECTIONTIMEOUT : CONNECTIONOK;
			mds_list_ctrl_.SetItemText(row, 4, connection_state.c_str());
			if (info.status) {
				for (auto i = 0; i < 6; ++i)
					mds_list_ctrl_.PushSelfDefinedColor(row, i);
			} else {
				for (auto i = 0; i < 6; ++i) {
					mds_list_ctrl_.EraseSelfDefinedColor(row, i);
				}
			}
		}
	}
	// To push color for the whole row successfully.	
	for (int k = 0; k < mds_list_ctrl_.GetItemCount(); ++k) {
		for (int m = 0; m < 6; ++m) {
			CString	text = mds_list_ctrl_.GetItemText(k, m);
			mds_list_ctrl_.SetItemText(k, m, text);
		}
	}
	mds_list_ctrl_.SetRedraw(TRUE);
	mds_list_ctrl_.UpdateWindow();
}

void CMDSDialog::InitMDSBasicInfo() {
	mds_list_ctrl_.SetRedraw(FALSE);
	auto row_no = mds_list_ctrl_.GetItemCount();
	for (auto& iter : total_basic_info_) {
		mds_list_ctrl_.InsertItem(row_no, to_string(row_no + 1).c_str());
		mds_list_ctrl_.SetItemText(row_no, 1, iter.second.server_name.c_str());
		mds_list_ctrl_.SetItemText(row_no, 2, iter.second.host_info);
		mds_list_ctrl_.SetItemText(row_no, 3, to_string(iter.second.port).c_str());
		string connection_state = (iter.second.status) ? CONNECTIONTIMEOUT : CONNECTIONOK;
		mds_list_ctrl_.SetItemText(row_no, 4, connection_state.c_str());
		if (iter.second.status) {
			for (auto i = 0; i < 6; ++i)
				mds_list_ctrl_.PushSelfDefinedColor(row_no, i);
		}
		servers_row_no_[iter.second.host_info] = row_no++;
	}
	mds_list_ctrl_.SetRedraw(TRUE);
	mds_list_ctrl_.UpdateWindow();
}