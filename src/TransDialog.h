#pragma once
#include "windows_mini_lib.h"

class CTransDialog : public CDialog {
	typedef map<int, string, comp> PriorMap;
	typedef windows_mini_lib::ColumnSortedState ColumnState;
	DECLARE_DYNAMIC(CTransDialog)
public:
	CTransDialog(CWnd* pParent = NULL);   // standard constructor
	CTransDialog(std::vector<unsigned short> marketsToDisplay, PriorMap& codeMap, map<unsigned short, STransDefine>& mapDefs, 
		vector<int>& vecSubs,vector<SMarketItem>& vecData, map<unsigned short, SMarketItem>& mapStandardTime,
		CWnd* pParent = NULL);
	virtual ~CTransDialog();
	CZListCtrl& GetListCtrl() { return m_list; }
	void UpdateDetails(const map<unsigned short, STransDefine>&, const vector<int>&, 
		const vector<SMarketItem>&, const map<unsigned short, SMarketItem>&);
	unsigned short GetSelectedCol() { return selected_column_no_; }
// 对话框数据
	enum { IDD = IDD_DIALOG_TRANS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporting DDX/DDV 
	BOOL OnInitDialog();
	void setInitialPosition();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
private:
	HICON								m_hIcon;
	CZListCtrl							m_list;
    map<unsigned short, int>			m_mapTransListPos;
    map<unsigned short, int>			m_mapMarketListPos;
	PriorMap							m_mapCodeMap;
	std::vector<unsigned short>			marketsToDisplay_;
	// The following four members correspond to the last four members in dialog CTransDialogDelay.
	// Use these four data members to update the child dialog.
	map<unsigned short, STransDefine>	m_mapDefs;
	// Transfer data to the child dialog.
	map<unsigned short, STransDefine>	m_mapDefsChild;
	vector<int>							m_vecSubs;
	vector<SMarketItem>					m_vecData;
	map<unsigned short, SMarketItem>	m_mapStandardTime;
	unsigned short						selected_column_no_;
	map<unsigned short, ColumnState>	map_column_sorted_state_;
public:
	afx_msg void OnLvnItemchangedListTrans(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	static int CALLBACK SortColProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};
