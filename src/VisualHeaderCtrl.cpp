#include "stdafx.h"
#include "VisualHeaderCtrl.h"


CVisualHeaderCtrl::CVisualHeaderCtrl() : m_nSortCol(-1)
{
}


CVisualHeaderCtrl::~CVisualHeaderCtrl()
{
}

int CVisualHeaderCtrl::SetSortImage(int nCol, BOOL bAsc)
{
	// Set the sort indicator
	int nPrevCol = m_nSortCol;
	m_nSortCol = nCol;
	m_bSortAsc = bAsc;

	// Change the item to owner drawn.
	// This will ensure that the DrawItem() function will get called.
	HD_ITEM hditem;
	hditem.mask = HDI_FORMAT;
	GetItem(nCol, &hditem);
	hditem.fmt |= HDF_OWNERDRAW;
	SetItem(nCol, &hditem);

	// Invalidate header control so that it gets redrawn
	// and any previous sort indicator is removed and the new one is displayed.
	Invalidate();

	return nPrevCol;
}

// This function is called for each item in the header control that has the HDF_OWNERDRAW format.
void CVisualHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Add your code to draw the specified item

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	// Get the column rect
	CRect rcLabel(lpDrawItemStruct->rcItem);
	// Save DC
	int nSavedDC = dc.SaveDC();
	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcLabel);
	dc.SelectObject(&rgn);
	rgn.DeleteObject();

	// Draw the background
	dc.FillRect(rcLabel, &CBrush(::GetSysColor(COLOR_3DFACE)));


	// Labels are offset by a certain amount, aks the margin.	  
	// This offset is related to the width of a space character
	int offset = dc.GetTextExtent(_T(" "), 1).cx * 2;

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;
	GetItem(lpDrawItemStruct->itemID, &hditem);

	// Determine format for drawing column label
	// The flag DT_END_ELLIPSIS tells the DrawText() function that if the text doesn't fit with the rectangle specified, 
	// then the text should be shortened and three dots appended to the text so that the result fits within the rectangle.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS;
	if (hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if (hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// Adjust the rect if the mouse button is pressed on it
	if (lpDrawItemStruct->itemState == ODS_SELECTED)
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}

	// Adjust the rect further if Sort arrow is to be displayed
	if (lpDrawItemStruct->itemID == (UINT)m_nSortCol)
	{
		rcLabel.right -= 3 * offset;
	}
	rcLabel.left += offset;
	rcLabel.right -= offset;

	// Draw column label
	if (rcLabel.left < rcLabel.right)
		dc.DrawText(buf, -1, rcLabel, uFormat);

	// Draw the Sort arrow
	// The COLOR_3DHILIGHT color is used for edges facing the light source, 
	// and the COLOR_3DSHADOW color is used for the shadow.
	if (lpDrawItemStruct->itemID == (UINT)m_nSortCol) {
		CRect rcIcon(lpDrawItemStruct->rcItem);
		// Set up pens to use for drawing the triangle
		CPen	penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen	penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen*	pOldPen = dc.SelectObject(&penLight);
		if (m_bSortAsc) {
			// Draw triangle pointing upwards
			dc.MoveTo(rcIcon.right - 2 * offset, offset - 1);
			dc.LineTo(rcIcon.right - 3 * offset / 2, rcIcon.bottom - offset);
			dc.LineTo(rcIcon.right - 5 * offset / 2 - 2, rcIcon.bottom - offset);
			dc.MoveTo(rcIcon.right - 5 * offset / 2 - 1, rcIcon.bottom - offset - 1);
			
			dc.SelectObject(&penShadow);
			dc.LineTo(rcIcon.right - 2 * offset, offset - 2);
		}
		else {
			// Draw triangle pointing downwords
			dc.MoveTo(rcIcon.right - 3 * offset / 2, offset - 1);
			dc.LineTo(rcIcon.right - 2 * offset - 1, rcIcon.bottom - offset + 1);
			dc.MoveTo(rcIcon.right - 2 * offset - 1, rcIcon.bottom - offset);
			
			dc.SelectObject(&penShadow);
			dc.LineTo(rcIcon.right - 5 * offset / 2 - 1, offset - 1);
			dc.LineTo(rcIcon.right - 3 * offset / 2, offset - 1);
		}
		
		// Restore the pen
		dc.SelectObject(pOldPen);
	}
	
	// Restore dc
	dc.RestoreDC(nSavedDC);
	
	// Detach the handle. Otherwise the DC would be released when the CDC object is destroyed.
	dc.Detach();

}
