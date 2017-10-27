#pragma once
#include "afxwin.h"

// TransSearchingDialog dialog

class TransDialogCallBack {
public:
	virtual void responceComboboxSelection(int selected_index = 0) = 0;
	virtual void responceComboboxInput(std::string text_input = "") = 0;
};

class TransSearchingDialog : public CDialogEx {
	DECLARE_DYNAMIC(TransSearchingDialog)
public:
	TransSearchingDialog(TransDialogCallBack* parentDialog = nullptr, CWnd* pParent = NULL);   // standard constructor
	virtual ~TransSearchingDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEARCH_TRANS };
	void appendMarketLists(std::string strMarketLists = "");
	void setSelectedIndex(int selected_index = 0);
	void setComboboxFocused();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCbnSelchangeComboMarket();
	CComboBox	marketLists_;
	TransDialogCallBack* parentDialog_;
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
protected:
	CStatic pureText_;
};
