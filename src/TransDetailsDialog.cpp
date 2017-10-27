// TransDetailsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TransDetailsDialog.h"
#include "afxdialogex.h"


// TransDetailsDialog dialog

IMPLEMENT_DYNAMIC(TransDetailsDialog, CDialogEx)

TransDetailsDialog::TransDetailsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(TransDetailsDialog::IDD, pParent) {

}

TransDetailsDialog::~TransDetailsDialog() {
	CDialog::OnClose();
}

void TransDetailsDialog::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_REFRESH_TIME1, refreshTime1_);
	DDX_Control(pDX, IDC_EDIT_IPLIST1, connectedIP1_);
	DDX_Control(pDX, IDC_EDIT_IPLIST2, connectedIP2_);
	DDX_Control(pDX, IDC_EDIT_IPLIST3, connectedIP3_);
	DDX_Control(pDX, IDC_EDIT_REFRESH_TIME2, refreshTime2_);
	DDX_Control(pDX, IDC_EDIT_REFRESH_TIME3, refreshTime3_);
	DDX_Control(pDX, IDC_STATIC_IPLIST, pureTextIPList_);
	DDX_Control(pDX, IDC_STATIC_REFRESHTIME, pureTextRefreshTime_);
}

BEGIN_MESSAGE_MAP(TransDetailsDialog, CDialogEx)
END_MESSAGE_MAP()

// TransDetailsDialog message handlers

void TransDetailsDialog::OnClose() {
	// TODO: add your handler code here.
	ShowWindow(SW_HIDE);
}

BOOL TransDetailsDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here
	CFont font;
	font.CreateFont(24, 8, 0, 0, 300, 0, 0, 0, 1, 0, 0, 0, 0, _T("Î¢ÈíÑÅºÚ(6)"));
	connectedIP1_.SetFont(&font);
	connectedIP2_.SetFont(&font);
	connectedIP3_.SetFont(&font);
	refreshTime1_.SetFont(&font);
	refreshTime2_.SetFont(&font);
	refreshTime3_.SetFont(&font);
	pureTextIPList_.SetFont(&font);
	pureTextRefreshTime_.SetFont(&font);
	font.DeleteObject();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
