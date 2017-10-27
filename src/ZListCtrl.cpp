#include "stdafx.h"
#include "zlistctrl.h"
#include "memdc.h"

CZListCtrl::CZListCtrl(void)
{
}

CZListCtrl::~CZListCtrl(void)
{
}

BEGIN_MESSAGE_MAP(CZListCtrl, CListCtrl)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CZListCtrl::OnSize(UINT nType, int cx, int cy)
{
    CListCtrl::OnSize(nType, cx, cy);

    GetClientRect(m_rectClient);

    CHeaderCtrl* pHC;
    pHC = GetHeaderCtrl();
    if (pHC != NULL)
    {
        CRect rectHeader;
        pHC->GetItemRect(0, &rectHeader);
        m_rectClient.top += rectHeader.bottom;
    }
}

BOOL CZListCtrl::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    UNUSED_ALWAYS(pDC);
    //return CTreeCtrl::OnEraseBkgnd(pDC);
    return TRUE;
}

void CZListCtrl::OnPaint()
{
    CPaintDC dc(this);
    // Paint to a memory device context to reduce screen flicker.
    zlei::CMemDC memDC(&dc, &m_rectClient);
    // Let the window do its default painting
    CWnd::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

void CZListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) {
    NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
    // By default set the return value to do the default behavior.
    *pResult = CDRF_DODEFAULT ;

    // Obtain row and column of item
    int iRow = pCD->nmcd.dwItemSpec;
    int iCol = pCD->iSubItem;

    // Remove standard highlighting of selected (sub)item.
    pCD->nmcd.uItemState = CDIS_DEFAULT;

    switch (pCD->nmcd.dwDrawStage) {
		case CDDS_PREPAINT: { // First stage (for the whole control)
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		} case CDDS_ITEMPREPAINT: {
			if ((pCD->nmcd.uItemState & CDIS_SELECTED) && (pCD->iSubItem == 3 || pCD->iSubItem == 4)) {
				pCD->clrText = RGB(255,0,0);
			}
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		} case CDDS_ITEMPREPAINT | CDDS_SUBITEM : {// Stage three
			// if (sub)item is of interest, set custom text/background color
			// 如果选中
			if (LVIS_SELECTED == this->GetItemState(iRow, LVIS_SELECTED)) { //判断当前项是否选中
			// 所画项是选中项
				pCD->clrText = RGB(138, 43, 226);		// font color : purple
				pCD->clrTextBk = RGB(222, 222, 222);	// grey
			} else {
				if (m_setColored.find(iRow * 1000 + iCol) != std::end(m_setColored)) {
					pCD->clrText = RGB(0, 0, 0);				// Black item text for delayed items.
					pCD->clrTextBk = RGB(254, 254, 65);			// Golden item background for delayed items.
				} else {
					pCD->clrText = RGB(0,0,255);
					pCD->clrTextBk = RGB(255,255,255);
				}
				auto result = map_item_to_self_defined_color_.find(iRow * 1000 + iCol);
				if (result != std::end(map_item_to_self_defined_color_)) {
					pCD->clrText = result->second.item_text_color;
					pCD->clrTextBk = result->second.item_background_color;
				}
			}
			*pResult =  CDRF_NOTIFYPOSTPAINT;
			break;
		} case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: {// Stage four (called for each subitem of the focused item)
			pCD->clrText = RGB(255, 0, 0);
			break;
		} default: { // it wasn't a notification that was interesting to us.
			*pResult = CDRF_DODEFAULT;
			break;
		}
    }
}

void CZListCtrl::PushColored(int iRow, int iCol) {
    m_setColored.insert(iRow * 1000 + iCol);
}

void CZListCtrl::EraseColored(int iRow, int iCol)
{
    m_setColored.erase(iRow * 1000 + iCol);
}

int CZListCtrl::GetColumnCount() {
    CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
    return (pHeaderCtrl->GetItemCount());
}


// Comment out this function cause it makes:
// 1. the loading performance of the application very slow.
// 2. the user update the interface manually. This is weird.
void CZListCtrl::AdjustColumnWidth() {
    //SetRedraw(FALSE); 
    //int nColumnCount = GetColumnCount(); 
    //for (int i = 0; i < nColumnCount; i++) {
    //    SetColumnWidth(i, LVSCW_AUTOSIZE); 
    //    int nColumnWidth = GetColumnWidth(i); 
    //    SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
    //    int nHeaderWidth = GetColumnWidth(i); 
    //    SetColumnWidth(i, max(nColumnWidth, nHeaderWidth));
    //}
    //SetRedraw(TRUE);
} 

CVisualHeaderCtrl& CZListCtrl::GetVisualHeaderCtrl()
{
	return m_visualHeaderCtrl;
}

void CZListCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CListCtrl::PreSubclassWindow();

	// Change the style to the report view mode because the header control is created only when the control first taken to the report view mode.
	// I don't restore the style here. So it may produce some bugs in the future.
	ModifyStyle(0, LVS_REPORT);

	// Sub-class the header control so that the DrawItem() function in CVisualHeaderCtrl can get called.
	m_visualHeaderCtrl.SubclassWindow(::GetDlgItem(m_hWnd, 0));
}

void CZListCtrl::PushSelfDefinedColor(int row_number, int column_number, COLORREF item_text_color, COLORREF item_background_color)
{
	map_item_to_self_defined_color_[row_number * 1000 + column_number] = ItemColorState(item_text_color, item_background_color);
}
void CZListCtrl::EraseSelfDefinedColor(int row_number, int column_number)
{
	map_item_to_self_defined_color_.erase(row_number * 1000 + column_number);
}
