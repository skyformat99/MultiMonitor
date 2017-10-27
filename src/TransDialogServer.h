#pragma once
#include "windows_mini_lib.h"

class CTransDialogServer : public CDialog {
	typedef map<int, string, comp> PriorMap;
	typedef windows_mini_lib::ColumnSortedState ColumnState;
	DECLARE_DYNAMIC(CTransDialogServer)
public:
	CTransDialogServer(CWnd* pParent = NULL);   // standard constructor
	CTransDialogServer(unsigned short serverToDisplay, PriorMap& codeMap, map<unsigned short, STransDefine>& mapDefs,
					   vector<int>& vecSubs, vector<SMarketItem>& vecData,
					   map<unsigned short, SMarketItem>& mapStandardTime, CWnd* pParent = NULL);
	virtual ~CTransDialogServer();
	CZListCtrl& GetListCtrl() { return m_list; }
	void UpdateDetails(const vector <SMarketItem>&, const map <unsigned short, SMarketItem>&);
	unsigned short GetSelectedCol() { return selected_column_no_; }
	// Dialog Data
	enum { IDD = IDD_DIALOG_TRANSSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporting DDX/DDV 
	BOOL  OnInitDialog();
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
	unsigned short						serverToDisplay_;
	//CRect								m_rect;						// Record the dialog's rect before resizing.
	string								m_strDlgName;
	// The following four members correspond to the last four members in dialog CTransDialog.
	// Use these four data members to update the child dialog.
	map<unsigned short, STransDefine>	m_mapDefs;
	vector<int>							m_vecSubs;
	vector<SMarketItem>					m_vecData;
	map<unsigned short, SMarketItem>	m_mapStandardTime;
	// Use the following data members to realize the function of sorting.
	unsigned short						selected_column_no_;
	std::map <unsigned short, ColumnState>	map_column_sorted_state_;
public:
	afx_msg void OnLvnItemchangedListTrans(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	static int CALLBACK SortColProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};
