#pragma once
#include <mutex>
#include "afxwin.h"
// CMDSDialog dialog

class CMDSDialog : public CDialog {
	DECLARE_DYNAMIC(CMDSDialog)

public:
	CMDSDialog(CWnd* pParent = NULL);   // Standard constructor
	virtual ~CMDSDialog();
	CZListCtrl& GetListCtrl() { return mds_list_ctrl_; }
	// dialog data
	enum { IDD = IDD_DIALOG_MDS };
	void UpdateMDSBasicInfo(unsigned short connection_id);
	void InitMDSBasicInfo();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT OnNotifyMDSListUpdate(WPARAM, LPARAM);
	LRESULT OnNotifyMDSVersions(WPARAM, LPARAM);
	bool LoadMDSServer(const string & strFileName);
	DECLARE_MESSAGE_MAP()

	std::mutex													connections_mutex_;
	std::vector<CMDSMultiConn*>									connections_;
	std::map<std::string, MDSBasicInfo>							total_basic_info_;
	unsigned short												connected_ip_number_;
	CZListCtrl													mds_list_ctrl_;
	bool														list_ctrl_initialized_;
	std::map<std::string, int>									servers_row_no_;
private:
	HICON														m_hIcon;
};
