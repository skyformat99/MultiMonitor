// MultiMonitorDlg.h : 头文件
//

#pragma once

#include <memory>
#include "TransDialogDelay.h"
#include "GQSMonitor.h"
// CMultiMonitorDlg 对话框
class CMultiMonitorDlg : public CDialog, public MonitorDialogCallBack {
    //DECLARE_EASYSIZE
public:
    CMultiMonitorDlg(CWnd* pParent = NULL);			// standard constructor
	~CMultiMonitorDlg();
	void AdjustPosition(CDialog* dialog, CListCtrl* list_ctrl);
	void DataSourceAdjustPosition(CDialog* dialog, CListCtrl* list_ctrl);
	// 对话框数据
    enum { IDD = IDD_MULTIMONITOR_DIALOG };
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEditSearchTrans();
	afx_msg void OnEditDetails();
	afx_msg void OnEditHelp();
	// Limit the minimum size of the main window.
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	virtual void popupMainDialog();
	virtual bool canPlayMusic();
protected:
    virtual void DoDataExchange(CDataExchange* pDX); // support DDX/DDV 
    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
	HICON								m_hIcon;
	CTabCtrl							tab_;
	CMDSDialog							mds_dialog_;
	CDataSourceDlg						data_source_dialog_;
	GQSMonitor							gqs_dialog_;
	std::shared_ptr<CTransDialogDelay>	trans_dialog_;
	CMenu								mainMenu_;
	HMENU								handleMainMenu_;
	bool								initialized_;
	bool								mds_dialog_created_;
	bool								data_source_dialog_created_;
	bool								position_initialized_;
	bool								gqs_dialog_created_;
	HACCEL								acceleratorSearch_;
	HACCEL								acceleratorShowDetails_;
	bool								can_auto_popup_;
	bool								can_play_music_;
public:
	afx_msg void OnEditAutopopup();
	afx_msg void OnEditPlaymusic();
};
