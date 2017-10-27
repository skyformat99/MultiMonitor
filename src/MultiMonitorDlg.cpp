// MultiMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MultiMonitorDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    // 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支持

    // 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CMultiMonitorDlg 对话框

CMultiMonitorDlg::CMultiMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiMonitorDlg::IDD, pParent), mds_dialog_created_(false), data_source_dialog_created_(false), position_initialized_(false), gqs_dialog_created_(false) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    initialized_ = false;
	acceleratorSearch_ = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	acceleratorShowDetails_ = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR2));
	trans_dialog_ = std::make_shared<CTransDialogDelay>(this);
	can_auto_popup_ = true;
	can_play_music_ = false;
}

CMultiMonitorDlg::~CMultiMonitorDlg() {
	// Free the menu resource.
	::DestroyMenu(handleMainMenu_);
}

void CMultiMonitorDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, tab_);
}

BEGIN_MESSAGE_MAP(CMultiMonitorDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
    ON_NOTIFY(LVN_ODCACHEHINT, IDC_LIST_DATA, OnOdcachehintList)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMultiMonitorDlg::OnTcnSelchangeTab1)
	ON_COMMAND(ID_EDIT_SEARCH_TRANS, &CMultiMonitorDlg::OnEditSearchTrans)
	ON_COMMAND(ID_EDIT_DETAILS, &CMultiMonitorDlg::OnEditDetails)
	ON_COMMAND(ID_EDIT_HELP, &CMultiMonitorDlg::OnEditHelp)
	ON_COMMAND(ID_EDIT_AUTOPOPUP, &CMultiMonitorDlg::OnEditAutopopup)
	ON_COMMAND(ID_EDIT_PLAYMUSIC, &CMultiMonitorDlg::OnEditPlaymusic)
END_MESSAGE_MAP()

//BEGIN_EASYSIZE_MAP(CMultiMonitorDlg)
//	EASYSIZE(IDC_TAB1, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDD_DIALOG_TRANSDELAY, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDD_DIALOG_MDS, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDD_DIALOG_CALCULATION_SERVER, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDD_DIALOG_TRANS_GQS, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//	EASYSIZE(IDD_DIALOG_GRA, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
//END_EASYSIZE_MAP
// CMultiMonitorDlg 消息处理程序

void CMultiMonitorDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI) {
	CPoint pt(800, 600);
	lpMMI->ptMinTrackSize = pt;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CMultiMonitorDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    // 执行此操作
    ///INIT_EASYSIZE;
    SetIcon(m_hIcon, TRUE);				// Set big icon
    SetIcon(m_hIcon, FALSE);			// Set small icon
    // TODO: 在此添加额外的初始化代码	
	string strLogDirName = "log";
    if (_access(strLogDirName.c_str(), 0) != 0) {
        CreateDirectory(strLogDirName.c_str(), NULL);
    }
    string strLogFilePath = strLogDirName + "/tv";
    CLog::s_pLog = new CLog();
    CLog::s_pLog->Init(strLogFilePath.c_str(), false, CLog::Warn);

	tab_.SetPadding(CSize(40, 3)); 
	tab_.InsertItem(0, _T("Multi-Trans"));
    tab_.InsertItem(1, _T("Calculation Server"));
    tab_.InsertItem(2, _T("Multi-data System"));
	tab_.InsertItem(3, _T("Global Quote Server"));
	tab_.InsertItem(4, _T("Data Source"));
	CFont font;
    font.CreateFont(16,8,0,0,300,0,0,0,1,0,0,0,0,_T("微软雅黑(6)"));
    tab_.SetFont(&font);

	trans_dialog_->Create(IDD_DIALOG_TRANSDELAY, &tab_);
	// Create calculation server dialog directly cause it doesn't have socket connection in the background.
	trans_dialog_->calculation_server_dialog_.Create(IDD_DIALOG_CALCULATION_SERVER, &tab_);
    CRect rc;
    tab_.GetClientRect(&rc);

    // Adjust children dialogs' position in the parent dialog.
    rc.top += 29;
    rc.bottom -= 30;
    rc.left += 8;
    rc.right -= 8;
	trans_dialog_->MoveWindow(&rc);
	trans_dialog_->calculation_server_dialog_.MoveWindow(&rc);

    // Set the position of the list ctrl.
    rc.top += 5;
    rc.bottom -= 20;
    rc.left += 8;
    rc.right -= 8;
	trans_dialog_->GetListCtrl().MoveWindow(&rc);
	trans_dialog_->calculation_server_dialog_.GetListCtrl().MoveWindow(&rc);
	trans_dialog_->ShowWindow(SW_SHOW);
	mainMenu_.LoadMenuA(IDR_MENU2);
	ASSERT(mainMenu_);
	// This API can only be called in the main dialog.
	SetMenu(&mainMenu_);
	handleMainMenu_ = mainMenu_.GetSafeHmenu();
    return TRUE; //return TRUE unless you set the focus to a control
}

void CMultiMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
// 来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
// 这将由框架自动完成。

void CMultiMonitorDlg::OnPaint() {
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
		if (!position_initialized_) {
			ShowWindow(SW_SHOWMAXIMIZED);
			// Set the caption of the main window.
			CWnd::SetWindowText(_T("Transviewer 4.0.0 -- 东方财富"));
			position_initialized_ = true;
		}
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CMultiMonitorDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CMultiMonitorDlg::OnSize(UINT nType, int cx, int cy) {
    CDialog::OnSize(nType, cx, cy);

	//UPDATE_EASYSIZE;
    if (initialized_ && (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)) {
        CRect rect;
        GetWindowRect(&rect);
        ScreenToClient(&rect);
        //set tab coordinates
        rect.top += 50;
        rect.bottom -= 20;
        rect.left += 8;
        rect.right -= 8;
        tab_.MoveWindow(&rect);

        //set dialog coordinates
        CRect rc, mdsrc, rcFullList;
        tab_.GetClientRect(&rc);
        rc.top += 20;
        rc.bottom -= 10;
        rc.left += 8;
        rc.right -= 8;

        mdsrc = rc;
        //mdsrc.right -= 250;
		if (mds_dialog_created_)
			mds_dialog_.MoveWindow(&rc);
		if (data_source_dialog_created_)
			data_source_dialog_.MoveWindow(&rc);
		if (gqs_dialog_created_)
			gqs_dialog_.MoveWindow(&rc);
		trans_dialog_->MoveWindow(&rc);
		trans_dialog_->calculation_server_dialog_.MoveWindow(&rc);
        // Set the position of the list ctrl.
        rc.top -= 12;
        rc.bottom -= 20;
        rc.left += 8;
        rc.right -= 8;
		rcFullList = rc;

        mdsrc.top += 5;
        mdsrc.bottom -= 20;
        mdsrc.left += 8;
		if (mds_dialog_created_)
			mds_dialog_.GetListCtrl().MoveWindow(&mdsrc);
		if (data_source_dialog_created_)
			data_source_dialog_.GetListCtrl().MoveWindow(&rc);
		if (gqs_dialog_created_)
			gqs_dialog_.GetListCtrl().MoveWindow(&mdsrc);
		trans_dialog_->calculation_server_dialog_.GetListCtrl().MoveWindow(&rc);
		// Fix the width of the first list ctrl in trans_dlg_delay.
		rc.right = 260;
		trans_dialog_->GetListCtrl().MoveWindow(&rc);
		rcFullList.left = rc.right;
		trans_dialog_->GetFullListCtrl().MoveWindow(&rcFullList);
		if (mds_dialog_created_)
			mds_dialog_.SendMessage(WM_SIZE, NULL, NULL);
		if (data_source_dialog_created_)
			data_source_dialog_.PostMessage(WM_SIZE,(WPARAM)(nType), NULL);
		if (gqs_dialog_created_)
			gqs_dialog_.SendMessage(WM_SIZE, NULL, NULL);
		trans_dialog_->SendMessage(WM_SIZE, NULL, NULL);
		trans_dialog_->calculation_server_dialog_.SendMessage(WM_SIZE, NULL, NULL);
    }
    initialized_ = true;
}

void CMultiMonitorDlg::OnClose() {
    // TODO: 在此添加消息处理程序代码和/或调用默认值
	if (mds_dialog_created_)
		mds_dialog_.SendMessage(WM_CLOSE, NULL, NULL);
	if (data_source_dialog_created_)
		data_source_dialog_.SendMessage(WM_CLOSE, NULL, NULL);
	if (gqs_dialog_created_)
		gqs_dialog_.SendMessage(WM_CLOSE, NULL, NULL);
	trans_dialog_->SendMessage(WM_CLOSE, NULL, NULL);
	trans_dialog_->SendMessage(WM_CLOSE, NULL, NULL);
    delete CLog::s_pLog;
    CLog::s_pLog = NULL;
	// Destroy this window.
	EndDialog(IDCANCEL);
	CDialog::OnClose();
}

//This is called to give you a chance to catch data. Useless in most cases :-)
void CMultiMonitorDlg::OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult) {
    NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;

    /* TRACE(_T("Chache item %d to item %d\n"),
    pCacheHint->iFrom,
    pCacheHint->iTo );*/

    // cache the data pCacheHint->iFrom to pCacheHint->iTo ...

    *pResult = 0;
}


void CMultiMonitorDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
	auto tab_count = tab_.GetItemCount();
	auto selected_tab = tab_.GetCurSel();
	for (int i = 0; i < tab_count; ++i) {
		BOOL highlighted = (i == selected_tab) ? TRUE : FALSE;
		tab_.HighlightItem(i, highlighted);
	}

	switch (selected_tab) {
		case 0:
			if (mds_dialog_created_)
				mds_dialog_.ShowWindow(SW_HIDE);
			if (data_source_dialog_created_)
				data_source_dialog_.ShowWindow(SW_HIDE);
			if (gqs_dialog_created_)
				gqs_dialog_.ShowWindow(SW_HIDE);
			trans_dialog_->ShowWindow(SW_SHOW);
			trans_dialog_->calculation_server_dialog_.ShowWindow(SW_HIDE);
			break;
		case 1:
			if (mds_dialog_created_)
				mds_dialog_.ShowWindow(SW_HIDE);
			if (data_source_dialog_created_)
				data_source_dialog_.ShowWindow(SW_HIDE);
			if (gqs_dialog_created_)
				gqs_dialog_.ShowWindow(SW_HIDE);
			trans_dialog_->ShowWindow(SW_HIDE);
			trans_dialog_->calculation_server_dialog_.ShowWindow(SW_SHOW);
			break;
		case 2:
			if (!mds_dialog_created_) {
				mds_dialog_.Create(IDD_DIALOG_MDS, &tab_);
				AdjustPosition(&mds_dialog_, &mds_dialog_.GetListCtrl());
				mds_dialog_created_ = true;
			}
			mds_dialog_.ShowWindow(SW_SHOW);
			if (data_source_dialog_created_)
				data_source_dialog_.ShowWindow(SW_HIDE);
			if (gqs_dialog_created_)
				gqs_dialog_.ShowWindow(SW_HIDE);
			trans_dialog_->ShowWindow(SW_HIDE);
			trans_dialog_->calculation_server_dialog_.ShowWindow(SW_HIDE);
			break;
		case 3:
			if (mds_dialog_created_)
				mds_dialog_.ShowWindow(SW_HIDE);
			if (data_source_dialog_created_)
				data_source_dialog_.ShowWindow(SW_HIDE);
			if (!gqs_dialog_created_) {
				gqs_dialog_.Create(IDD_DIALOG_TRANS_GQS, &tab_);
				AdjustPosition(&gqs_dialog_, &gqs_dialog_.GetListCtrl());
				gqs_dialog_created_ = true;
			}
			gqs_dialog_.ShowWindow(SW_SHOW);
			trans_dialog_->ShowWindow(SW_HIDE);
			trans_dialog_->calculation_server_dialog_.ShowWindow(SW_HIDE);
			break;
		case 4:
			if (mds_dialog_created_)
				mds_dialog_.ShowWindow(SW_HIDE);
			if (!data_source_dialog_created_) {
				data_source_dialog_.Create(IDD_DIALOG_GRA,&tab_);
				DataSourceAdjustPosition(&data_source_dialog_, &data_source_dialog_.GetListCtrl());
				data_source_dialog_created_ = true;
			}
			data_source_dialog_.ShowWindow(SW_SHOW);
			if (gqs_dialog_created_)
				gqs_dialog_.ShowWindow(SW_HIDE);
			trans_dialog_->ShowWindow(SW_HIDE);
			trans_dialog_->calculation_server_dialog_.ShowWindow(SW_HIDE);
			break;		
		default:
			trans_dialog_->ShowWindow(SW_SHOW);
			break;
    }
}

void CMultiMonitorDlg::AdjustPosition(CDialog* dialog, CListCtrl* list_ctrl) {
	CRect rc;
	tab_.GetClientRect(&rc);

	// Set the position of the dialog appropriately
	rc.top += 20;
	rc.bottom -= 30;
	rc.left += 8;
	rc.right -= 8;

	dialog->MoveWindow(&rc);

	// Set the position of the list control
	rc.top += 5;
	rc.bottom -= 20;
	rc.left += 8;
	rc.right -= 8;

	list_ctrl->MoveWindow(&rc);
}

void CMultiMonitorDlg::DataSourceAdjustPosition(CDialog* dialog, CListCtrl* list_ctrl) {
	CRect rc;
	tab_.GetClientRect(&rc);

	// Set the position of the dialog appropriately
	rc.top += 20;
	rc.bottom -= 30;
	rc.left += 8;
	rc.right -= 8;

	dialog->MoveWindow(&rc);

	// Set the position of the list control
	rc.top += 30;
	rc.bottom -= 20;
	rc.left += 8;
	// Fix the width of the list ctrl when initializing.
	rc.right = 325;

	list_ctrl->MoveWindow(&rc);
}

void CMultiMonitorDlg::OnEditSearchTrans() {
	// TODO: Add your command handler code here
	trans_dialog_->showSearchingDialog();
}

BOOL CMultiMonitorDlg::PreTranslateMessage(MSG* pMsg) {
	// TODO: Add your specialized code here and/or call the base class
	if (TranslateAccelerator(m_hWnd, acceleratorSearch_, pMsg)) {
		trans_dialog_->showSearchingDialog();
		return true;
	} else if (TranslateAccelerator(m_hWnd, acceleratorShowDetails_, pMsg)) {
		trans_dialog_->showDetailsDialog();
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMultiMonitorDlg::OnEditDetails() {
	// TODO: Add your command handler code here
	trans_dialog_->showDetailsDialog();
}

void CMultiMonitorDlg::OnCancel() {
	// TODO: Add your specialized code here and/or call the base class
	// Comment out the default statement and deal with the details in OnClose().
	return;
	//CDialog::OnCancel();
}

void CMultiMonitorDlg::OnEditHelp() {
	AfxMessageBox(_T("Help yourself...(Use the key: Esc)"));
}

void CMultiMonitorDlg::popupMainDialog() {
	if (can_auto_popup_) {
		WINDOWPLACEMENT lwndpl;
		WINDOWPLACEMENT* lpwndpl;
		lpwndpl = &lwndpl;
		GetWindowPlacement(lpwndpl);
		if (lpwndpl->showCmd != SW_SHOWMAXIMIZED) {
			lpwndpl->showCmd = SW_SHOWMAXIMIZED;
			SetWindowPlacement(lpwndpl);
		}
	}
}

void CMultiMonitorDlg::OnEditAutopopup() {
	CMenu* menu_popup = GetMenu();
	if (menu_popup) {
		if (can_auto_popup_) {
			menu_popup->CheckMenuItem(ID_EDIT_AUTOPOPUP, MF_UNCHECKED | MF_BYCOMMAND);
			can_auto_popup_ = false;
		} else {
			menu_popup->CheckMenuItem(ID_EDIT_AUTOPOPUP, MF_CHECKED | MF_BYCOMMAND);
			can_auto_popup_ = true;
		}
	}
}

bool CMultiMonitorDlg::canPlayMusic() {
	return can_play_music_;
}

void CMultiMonitorDlg::OnEditPlaymusic() {
	CMenu* menu_play_music = GetMenu();
	if (menu_play_music) {
		if (can_play_music_) {
			menu_play_music->CheckMenuItem(ID_EDIT_PLAYMUSIC, MF_UNCHECKED | MF_BYCOMMAND);
			can_play_music_ = false;
		} else {
			menu_play_music->CheckMenuItem(ID_EDIT_PLAYMUSIC, MF_CHECKED | MF_BYCOMMAND);
			can_play_music_ = true;
		}
	}
}
