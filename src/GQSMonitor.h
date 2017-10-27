#pragma once
#include "ZListCtrl.h"
#include "GQSConnection.h"
// GQSMonitor dialog

class GQSMonitor : public CDialog
{
	DECLARE_DYNAMIC(GQSMonitor)
public:
	GQSMonitor(CWnd* pParent = NULL);   // standard constructor
	virtual ~GQSMonitor();
	CZListCtrl& GetListCtrl() { return gqs_list_ctrl_; }
	virtual BOOL OnInitDialog();
// Dialog Data
	enum { IDD = IDD_DIALOG_TRANS_GQS };
	void UpdateGQSBasicInfo(unsigned short connection_id);
	void InitGQSBasicInfo();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT OnNotifyGQSListUpdate(WPARAM, LPARAM);
	bool LoadGQSServer(const string& file_name);
	DECLARE_MESSAGE_MAP()
	CZListCtrl								gqs_list_ctrl_;
	std::mutex								connections_mutex_;
	std::vector<GQSConnection*>				connections_;
	std::map<std::string, GQSServerInfo>	total_basic_info_;
	unsigned short							connected_ip_number_;
	bool									list_ctrl_initialized_;
	std::map<std::string, int>				servers_row_no_;
private:
	HICON									m_hIcon;
};


