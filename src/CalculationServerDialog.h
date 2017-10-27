#pragma once
#include "ZListCtrl.h"

// CalculationServerDialog dialog

class CalculationServerDialog : public CDialog {
	typedef map<int, string, comp> PriorMap;
	DECLARE_DYNAMIC(CalculationServerDialog)
public:
	CalculationServerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CalculationServerDialog();
	CZListCtrl& GetListCtrl() { return calculation_server_list_ctrl_; }
	void InitListCtrl(const PriorMap& markets, const std::map<unsigned short, STransDefine>& trans_defines);
	void UpdateListCtrl(const std::map<unsigned short, STransDefine>& trans_defines,
						const std::vector<int>& sub_trans,
						const std::vector<SMarketItem>& market_items,
						const std::map<unsigned short, SMarketItem>& standard_times);

// Dialog Data
	enum { IDD = IDD_DIALOG_CALCULATION_SERVER };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
protected:
	HICON							icon_;
	CZListCtrl						calculation_server_list_ctrl_;
	std::map<unsigned short, int>	servers_position_;
	std::map<unsigned short, int>	markets_position_;
};
