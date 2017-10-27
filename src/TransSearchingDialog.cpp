// TransSearchingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TransSearchingDialog.h"
#include "afxdialogex.h"

// TransSearchingDialog dialog

IMPLEMENT_DYNAMIC(TransSearchingDialog, CDialogEx)

TransSearchingDialog::TransSearchingDialog(TransDialogCallBack* parentDialog, CWnd* pParent /*=NULL*/)
	: CDialogEx(TransSearchingDialog::IDD, pParent), parentDialog_(parentDialog) {

}

TransSearchingDialog::~TransSearchingDialog() {
	CDialog::OnClose();
}

void TransSearchingDialog::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEARCH_TRANS, marketLists_);
	DDX_Control(pDX, IDC_STATIC_SEARCH, pureText_);
}

BEGIN_MESSAGE_MAP(TransSearchingDialog, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH_TRANS, &TransSearchingDialog::OnCbnSelchangeComboMarket)
END_MESSAGE_MAP()

// TransSearchingDialog message handlers

BOOL TransSearchingDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here
	CWnd::SetWindowTextA(_T("Trans Market Searching"));
	// RGB(255, 0, 255) is the COLORREF for magenta. With that all magenta pixels on the screen will be transparent.
	// The background of the dialog will be painted on WM_ERASEBKGND.
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(255, 0, 255), 0, LWA_COLORKEY);

	CFont font;
	font.CreateFont(25, 8, 0, 0, 300, 0, 0, 0, 1, 0, 0, 0, 0, _T("Î¢ÈíÑÅºÚ(6)"));
	marketLists_.SetFont(&font);
	pureText_.SetFont(&font);
	font.DeleteObject();
	pureText_.MoveWindow(&CRect(0, 20, 70, 40));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TransSearchingDialog::OnClose() {
	// TODO: add your handler code here.
	ShowWindow(SW_HIDE);
}

void TransSearchingDialog::OnOK() {
	// TODO: Add your specialized code here and/or call the base class
	if (GetDlgItem(IDC_COMBO_SEARCH_TRANS) == GetFocus()->GetParent()) {
		CString cStr;
		GetDlgItemText(IDC_COMBO_SEARCH_TRANS, cStr);
		string text_input = cStr.GetBuffer(cStr.GetLength());
		if (!text_input.empty()) {
			parentDialog_->responceComboboxInput(text_input);
			// Reset the content of the edit ctrl.
			SetDlgItemText(IDC_COMBO_SEARCH_TRANS, "");
		}
		return;
	}
	CDialogEx::OnOK();
}

void TransSearchingDialog::appendMarketLists(std::string strMarketLists) {
	marketLists_.AddString(strMarketLists.c_str());
}

void TransSearchingDialog::setSelectedIndex(int selected_index) {
	marketLists_.SetCurSel(selected_index);
}

void TransSearchingDialog::setComboboxFocused() {
	marketLists_.SetFocus();
}

BOOL TransSearchingDialog::OnEraseBkgnd(CDC* pDC) {
	CRect clientRect;
	GetClientRect(&clientRect);
	pDC->FillSolidRect(clientRect, RGB(255, 0, 255));  // paint background in magenta

	return FALSE;
}

HBRUSH TransSearchingDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) {
		case CTLCOLOR_STATIC: {
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0, 0, 0));
			HBRUSH brush = CreateSolidBrush(RGB(220, 220, 220)); 
			return brush;
		} case CTLCOLOR_EDIT: {
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0, 0, 0));
			HBRUSH brush = CreateSolidBrush(RGB(220, 220, 220));
			return brush;
		} case CTLCOLOR_SCROLLBAR: {
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 255, 255));
			HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
			return brush;
		}
	}
	return hbr;
}

void TransSearchingDialog::OnCbnSelchangeComboMarket() {
	int selected_index = marketLists_.GetCurSel() + 1;
	//displayed_markets_.GetLBText(selected_index, strCBText);
	parentDialog_->responceComboboxSelection(selected_index);
}