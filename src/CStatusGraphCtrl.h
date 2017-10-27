#if !defined(AFX_MONITORCTRL_H__249FD37A_5E1B_4A1B_B706_E2C97D000B9F__INCLUDED_)
#define AFX_MONITORCTRL_H__249FD37A_5E1B_4A1B_B706_E2C97D000B9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum 
{
	BAR_GRAPH,
	LINE_GRAPH
}StatusGraphType;

class CStatusGraphCtrl : public CWnd
{
private:
	long m_MinValue;
	long m_MaxValue;
	long m_CurValue;
	StatusGraphType m_GraphMode;
	int m_nDelay;
	short m_nSamplingInterval;
	UINT m_nCtrlId;
public:
	int GetRefreshDelay();
	void SetRefreshDelay(int nDelay);
	void SetCurrentValue(long ptr);
	long int m_lPreviousY;
	virtual BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
	CStatusGraphCtrl();
	virtual ~CStatusGraphCtrl();
	StatusGraphType GetGraphMode();
	long GetMinValue();
	long GetMaxValue();
	void SetMinValue(long);
	void SetMaxValue(long);
	long GetCurrentValue();
	void SetGraphMode(StatusGraphType);
	void StartUpdate();
	void SetSamplingInterval(short);
	short GetSamplingInterval();
	BOOL m_bActive;
	CGraphData *m_pGraphData;
	COLORREF m_BackColor;
	COLORREF m_ForeColor;
protected:
	void CStatusGraphCtrl::OnPaint();
	afx_msg void OnLButtonUp(UINT,CPoint);
	DECLARE_MESSAGE_MAP()
};

#endif  
