#pragma once
#include "afxwin.h"
// CTransDialogDelay dialog

#include <memory>
#include "VisualHeaderCtrl.h"
#include "ZListCtrl.h"
#include "afxcmn.h"
#include "CalculationServerDialog.h"
#include "TransSearchingDialog.h"
#include "TransDetailsDialog.h"
#include "windows_mini_lib.h"

class MonitorDialogCallBack {
public:
	virtual void popupMainDialog() = 0;
	virtual bool canPlayMusic() = 0;
};

class CTransDialogDelay : public CDialog, public TransDialogCallBack {
	typedef std::map<int, std::string, comp> PriorMap;
	typedef windows_mini_lib::ColumnSortedState ColumnState;
	DECLARE_DYNAMIC(CTransDialogDelay)
	//DECLARE_EASYSIZE
public:
	CTransDialogDelay(MonitorDialogCallBack* parentDialog = nullptr, CWnd* pParent = NULL);   // standard constructor
	virtual ~CTransDialogDelay();
	CZListCtrl& GetListCtrl() { return summary_list_ctrl_; }
	CZListCtrl& GetFullListCtrl() { return full_list_ctrl_; }
	unsigned short GetSelectedCol() { return market_list_selected_column_no_; }
	unsigned short GetFullListSelectedColumnNo() { return full_list_selected_column_no_; }
	BOOL OnInitDialog();
	// Self-defined onClose().
	void showSearchingDialog();
	void showDetailsDialog();
	virtual void responceComboboxSelection(int selected_index = 0);
	virtual void responceComboboxInput(std::string text_input = "");
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Dialog Data
	enum { IDD = IDD_DIALOG_TRANSDELAY };
	// This child dialog synchronized with full_list_ctrl_.
	CalculationServerDialog	calculation_server_dialog_;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg LRESULT OnNotifyListUpdate(WPARAM, LPARAM);
	bool readConfigFile(std::string configFileName);
	bool LoadMarketCode(const std::string & strFileName);
	bool LoadServer(const std::string & strFileName, const std::vector<unsigned short> & vectMarket);
	void InitFullList(const std::map<unsigned short, STransDefine>& trans_defines);
	void UpdateFullList(const std::map<unsigned short, STransDefine>& trans_defines,
		const std::vector<int>& sub_trans,
		const std::vector<SMarketItem>& market_items,
		const std::map<unsigned short, SMarketItem>& standard_times);
	// Call this function after each column sorting in order to draw colors of the cells in the full list correctly.
	void UpdateColoredItems(CZListCtrl&);
	void UpdateMarketStandardTimes(std::map<unsigned short, SMarketItem>& standard_times);
	bool isTransSearchingDialogCreated();
	bool isTransDetailsDialogCreated();
	void setWindowInitialPosition(CDialog* dialog);
	DECLARE_MESSAGE_MAP()
	static int CALLBACK SortColProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK SortListCtrl(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	afx_msg void OnLvnItemchangedListTrans(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnFullListColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickFullList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickSummaryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDetailsPickupacolor();
	afx_msg void OnMenuCopyServerIp();
	afx_msg void OnMenuCopyUpstreamIp();
	afx_msg void OnDetailsDetailsmarket();
	/*afx_msg void OnDetailsDisablepopup();
	afx_msg void OnDetailsEnablepopup();*/
	virtual void OnOK();
	virtual void OnCancel();
	void updateMarketCellStates(const std::map<unsigned int, SMarketItem>& market_items, 
		const std::map<unsigned short, SMarketItem>& market_standard_times);
	void updateSummaryListCells(const std::vector<SMarketItem>& market_items,
		const std::map<unsigned short, SMarketItem>& market_standard_times);
	void setConnectedIPNotifiedTime(const unsigned short& connection_id = 0);
	void forceUpdateAllMarketCells();
protected:
	HICON										m_hIcon;
	CZListCtrl									summary_list_ctrl_;
	CZListCtrl									full_list_ctrl_;
	std::map<unsigned short, SMarketItem>		m_mapMarketMaxTime;
	std::map<unsigned short, int>				m_mapMarketListPos;
	std::map<unsigned short, int>				m_mapMarketFullListPos;
	std::map<unsigned short, int>				m_mapTransFullListPos;
	std::vector<CMultiCon*>						connections_;
	PriorMap									markets_;
	// The key is the market id.
	std::map<unsigned int, CTransDialog*>		market_info_dialogs_;
	std::vector<CTransDialog*>					markets_info_dialogs_;
	// Map the server's ID to its Chinese name.
	std::map<unsigned short, std::string>		servers_;
	std::map<unsigned short, CString>			map_connection_id_to_ip_;
	unsigned short								market_list_selected_column_no_;
	unsigned short								full_list_selected_column_no_;
	// The first index represents the market list and the second index represents the transServer list.
	std::map<unsigned short, std::map<unsigned short, bool> >	m_mapColoredItem;
	// The following four members intends to save some data for child dialogs' initialization.
	std::map<unsigned short, STransDefine>		m_mapDefsChild;
	std::vector<int>							m_vecSubsChild;
	std::vector<SMarketItem>					m_vecDataChild;
	// Unify the standard market time from different connected IPs.
	std::map<unsigned short, SMarketItem>		market_standard_times_;
	// Map the market's Chinese name to its ID.
	std::map<std::string, UINT16>				m_mapMarketNameToID;
	bool										full_list_initialized_;
	std::map<unsigned short, ColumnState>		map_column_sorted_state_;
	std::map<unsigned short, ColumnState>		map_market_list_column_sorted_state_;
	std::map<unsigned int, ItemColorState>		cellsSelfDefinedColor_;
	CRect										currentWindowSize_;
	// Map the server's IP to its ID.
	std::map<std::string, unsigned short>		map_server_ip_to_id_;
	SYSTEMTIME									launchTime_;
	// Base time for double checking the market cells in the full list ctrl.
	SYSTEMTIME									baseTime_;
	TransSearchingDialog*						transSearchingDialog_;
	TransDetailsDialog*							transDetailsDialog_;
	MonitorDialogCallBack*						parentDialog_;
	HACCEL										acceleratorSearch_;
	HACCEL										acceleratorShowDetails_;
	// Key = trans_id * 1000 + market_id.
	std::map<unsigned int, SMarketItem>			latestMarketItems_;
	std::string									backgroundMusicPath_;
	std::string									warningMusicPath_;
	bool										isWaringMusicOn_;
	bool										isNormalMusicOn_;
	//std::set<unsigned short>					marketsPopupDisabled_;
};
