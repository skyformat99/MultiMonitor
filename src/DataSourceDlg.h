#pragma once

#define		IDC_STATIC_SEL(i)	IDC_STATIC_SELi
#define		IDC_EDIT_SEL(i)		IDC_EDIT_SELi
#define		BUYSELNUM			5


// CDataSourceDlg 对话框
class CDataSourceDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataSourceDlg)

public:
	CDataSourceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataSourceDlg();
// 对话框数据
	enum { IDD = IDD_DIALOG_GRA };
	CZListCtrl& GetListCtrl() { return m_marketList; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	BOOL  OnInitDialog();
	void  OnPaint();
	int   OnCreate(LPCREATESTRUCT lpCreateStruct);
	bool  LoadDSServer(const string & strFileName);
	void  InitCodeList();
	LRESULT OnNotifyMDSListUpdate(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();

	void  OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);

	void InitMarketData();
	void PrintStdQtToScreen();
	void InitServers();
	void FlashData();
	void DestroyListItems();
	DECLARE_MESSAGE_MAP()

private:
	CZListCtrl    m_marketList;
	CTreeItem*	  m_itemRoot;
	CComboBox     m_comboBoxServer;
	CComboBox	  m_comMarketlist;
	CRichEditCtrl m_srvEditctrl;
	CRichEditCtrl m_lastPriceEC;
	CRichEditCtrl m_highPriceEC;
	CRichEditCtrl m_lowPriceEC;
	CRichEditCtrl m_volumeEC;
	CRichEditCtrl m_amountEC;
	CRichEditCtrl m_dateEC;
	CRichEditCtrl m_timeEC;
	CRichEditCtrl m_codeEC;
	CRichEditCtrl m_invstHoldEC;
	CRichEditCtrl m_openEC;
	CRichEditCtrl m_closeEC;
	CRichEditCtrl m_presettlementEC;
	CRichEditCtrl m_portEC;
	CRichEditCtrl m_buy[BUYSELNUM];
	CRichEditCtrl m_sel[BUYSELNUM];
	CRichEditCtrl m_avgEc;
	CRichEditCtrl m_settlementEC;
	CRichEditCtrl m_preInvEC;
	CRichEditCtrl m_trdNumEC;

	HICON									m_hIcon;
	CStatusGraphCtrl						m_ctrlStatusGraph;
	std::vector<CDSConn*>					m_listDSCon;
	mapMarkets								m_marketData;
	CMutex									m_lock;
	bool									m_init;
	std::map<string,std::vector<string> >	m_mapServers;
public:
	afx_msg void OnCbnSelchangeComboMarket();
	afx_msg void OnCbnSelchangeComboServer();
	afx_msg void OnBnClickedFlash();
};
