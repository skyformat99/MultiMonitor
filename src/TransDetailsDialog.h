#pragma once
#include "afxwin.h"

// TransDetailsDialog dialog

class TransDetailsDialog : public CDialogEx {
	DECLARE_DYNAMIC(TransDetailsDialog)

public:
	TransDetailsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~TransDetailsDialog();
// Dialog Data
	enum { IDD = IDD_DIALOG_TRANS_DETAILS };
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
	// I will use a list ctrl in the future.
public:
	CEdit connectedIP1_;
	CEdit connectedIP2_;
	CEdit connectedIP3_;
	CEdit refreshTime1_;
	CEdit refreshTime2_;
	CEdit refreshTime3_;
protected:
	CStatic pureTextIPList_;
	CStatic pureTextRefreshTime_;
};
