#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT UpdateGraph(LPVOID);
void DrawPoint(CStatusGraphCtrl*,long int,bool=true);

// used to display the current value above the graph
# define TOP_MARGIN 17 


CPen *DrawPen,*ErasePen;
CWinThread* UpdateThread;


CStatusGraphCtrl::CStatusGraphCtrl()
{
	m_BackColor = RGB(255,250,250);
	m_ForeColor = RGB(255,0,0);
	m_CurValue = 0;
	m_MinValue = 0;
	m_MaxValue =100;
	m_nDelay = 1000;
	m_GraphMode = BAR_GRAPH;
	m_lPreviousY = 0;
	m_bActive = TRUE;
	UpdateThread = NULL;
	m_nSamplingInterval = 5;
	m_pGraphData = NULL;
}

CStatusGraphCtrl::~CStatusGraphCtrl()
{
	if (UpdateThread!=NULL) 
		UpdateThread->SuspendThread();
	m_bActive=0; // stop the update thread
	delete UpdateThread;
	delete m_pGraphData;
	delete DrawPen;
	delete ErasePen;
}

BOOL CStatusGraphCtrl::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
	// BS_OWNERDRAW is compulsory for user defined drawing function
	// ie. DrawItem will be called only if button style is BS_OWNERDRAW
	m_nCtrlId = nID;
	return CWnd::Create(NULL,lpszCaption,dwStyle,rect,pParentWnd,nID);

}

long CStatusGraphCtrl::GetMinValue()
{
	return m_MinValue;
}

long CStatusGraphCtrl::GetMaxValue()
{
	return m_MaxValue;
}

void CStatusGraphCtrl::SetMinValue(long v)
{	
	m_MinValue = v;
}

void CStatusGraphCtrl::SetMaxValue(long v)
{
	m_MaxValue = v;
}

long CStatusGraphCtrl::GetCurrentValue()
{
	return m_CurValue;
}

StatusGraphType CStatusGraphCtrl::GetGraphMode()
{
	return m_GraphMode;
}


void CStatusGraphCtrl::SetGraphMode(StatusGraphType mode)
{
	m_GraphMode = mode;
	Invalidate();
}

UINT UpdateGraph(LPVOID param)
{
	CStatusGraphCtrl *TheCtrl = (CStatusGraphCtrl*)param;
	CRect rcBounds;
	TheCtrl->GetClientRect(&rcBounds);
	rcBounds.top += TOP_MARGIN;
	while(TheCtrl->m_bActive) 
	{
		Sleep(TheCtrl->GetRefreshDelay() - 200);							// Please wake up as quickly as possible.
		long int cury;
		cury = (long)ceil((((float)TheCtrl->GetCurrentValue()/TheCtrl->GetMaxValue())*rcBounds.Height()));
		DrawPoint((CStatusGraphCtrl*)param,cury);
	}
	return 0;
}

void CStatusGraphCtrl::StartUpdate() 
{
	// TODO: Add your dispatch handler code here
	UpdateThread = AfxBeginThread(UpdateGraph,this);
}


void CStatusGraphCtrl::SetCurrentValue(long ptr)
{
	if(ptr > m_MaxValue)
		m_CurValue = m_MaxValue;
	else if(ptr  < m_MinValue)
		m_CurValue = m_MinValue;
	else
		m_CurValue = ptr;
}

int CStatusGraphCtrl::GetRefreshDelay()
{
	return m_nDelay;
}

void CStatusGraphCtrl::SetRefreshDelay(int nDelay)
{
	m_nDelay = nDelay;
}

void CStatusGraphCtrl::SetSamplingInterval(short nInt)
{
	m_nSamplingInterval = nInt;
}

short CStatusGraphCtrl::GetSamplingInterval()
{
	return m_nSamplingInterval;
}


void CStatusGraphCtrl::OnPaint()
{
	CClientDC objDC(this);
	CRect rcBounds;
	GetClientRect(&rcBounds);
	CBrush tmp_brush(m_BackColor);
	objDC.FillRect(rcBounds, &tmp_brush);
	tmp_brush.DeleteObject();
	
	DrawPen = new CPen(PS_SOLID,1,m_ForeColor);
	ErasePen = new CPen(PS_SOLID,1,m_BackColor);
	if(m_pGraphData == NULL)
		m_pGraphData=new CGraphData(rcBounds.Width()/m_nSamplingInterval); 

	// Restore data on the graph
	struct _data *data;
	data = m_pGraphData->left;
	if(m_pGraphData->IsAllOk()) {
		while(data) {
			int i = data->Value;
			if(i >= 0 && i <= m_MaxValue*100)
				DrawPoint(this,data->Value,false);
			data = data->Next;
		}
	}
	CWnd::OnPaint();
}

afx_msg void CStatusGraphCtrl::OnLButtonUp(UINT nFlags,CPoint point)
{
	WPARAM wParam;
	LPARAM lParam;

	wParam = m_nCtrlId;
	lParam = (LPARAM)m_hWnd;

	GetParent()->SendMessage(WM_COMMAND,wParam,lParam);

	CWnd::OnLButtonUp(nFlags,point);
}

BEGIN_MESSAGE_MAP(CStatusGraphCtrl,CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void DrawPoint(CStatusGraphCtrl* TheCtrl,long int cury,bool Update)
{
	// updates the graph and adds a new point for the given value
	CDC* pdc=NULL;
	CRect rcBounds;
	short sSamplingInterval = TheCtrl->GetSamplingInterval();
	while(!pdc) 
		pdc = TheCtrl->GetDC();
	TheCtrl->GetClientRect(&rcBounds);
	rcBounds.top += TOP_MARGIN;
	pdc->BitBlt(rcBounds.left,rcBounds.top,rcBounds.Width()-sSamplingInterval,rcBounds.Height(),pdc,rcBounds.left+sSamplingInterval,rcBounds.top,SRCCOPY);
	pdc->SelectObject(ErasePen);
	pdc->MoveTo(rcBounds.right-sSamplingInterval,rcBounds.bottom);
	pdc->LineTo(rcBounds.right-sSamplingInterval,rcBounds.top);
		
	pdc->SelectObject(DrawPen);
	switch(TheCtrl->GetGraphMode())
	{
	case BAR_GRAPH:
		pdc->MoveTo(rcBounds.right-sSamplingInterval,rcBounds.bottom);
		pdc->LineTo(rcBounds.right-sSamplingInterval,rcBounds.bottom-cury);
		break;
	case LINE_GRAPH:
		pdc->MoveTo(rcBounds.right-2*sSamplingInterval,rcBounds.bottom-TheCtrl->m_lPreviousY);
		pdc->LineTo(rcBounds.right-sSamplingInterval,rcBounds.bottom-cury);
		break;
	//**************************************************************************//
	// TODO:
	// Update the enum type in the header (CStatusGraphCtrl.h) file accordingly
	//	Add additional case blocks to support your own graph modes
	//	cury - represents the current value to plot
	//  TheCtrl->m_lPreviousY - represents the previous point plotted.
	//	rcBounds - represent the bounds of the CStatusGraphCtrl Control.
	//*************************************************************************//
	}


	// this code is to update the current value that is displayed on top of the graph
	char cnt[10];
	long nReq = cury*TheCtrl->GetMaxValue()/rcBounds.Height();
	pdc->SetBkColor(TheCtrl->m_BackColor);
	pdc->SetTextColor(TheCtrl->m_ForeColor);
	sprintf(cnt,"  %02ld  ",nReq);
	pdc->TextOut(rcBounds.Width()/2,rcBounds.top-TOP_MARGIN,cnt);
	if(Update && TheCtrl->m_pGraphData) 
		TheCtrl->m_pGraphData->NewValue(cury);
	ReleaseDC(TheCtrl->m_hWnd,pdc->m_hDC); 
	TheCtrl->m_lPreviousY=cury;
}
