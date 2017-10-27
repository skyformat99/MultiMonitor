#pragma once

#include "VisualHeaderCtrl.h"

struct ItemColorState {
	COLORREF item_text_color;
	COLORREF item_background_color;
	ItemColorState() = default;
	ItemColorState(COLORREF text_color, COLORREF background_color) {
		item_text_color = text_color;
		item_background_color = background_color;
	}
};

class CZListCtrl : public CListCtrl {
public:
    CZListCtrl(void);
    virtual ~CZListCtrl(void);
    void PushColored(int iRow, int iCol);
    void EraseColored(int iRow, int iCol);
	void PushSelfDefinedColor(int row_number, int column_number, 
							  COLORREF item_text_color = RGB(0, 0, 0), 
							  COLORREF item_background_color = RGB(254, 254, 65));
	void EraseSelfDefinedColor(int row_number, int column_number);
	void ClearSelfDefinedColor() { map_item_to_self_defined_color_.clear(); }
	void AdjustColumnWidth();
    int GetColumnCount();
	CVisualHeaderCtrl& GetVisualHeaderCtrl();
protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
public:
    CRect m_rectClient;
private:
    std::set<int>			m_setColored;
protected:
	CVisualHeaderCtrl				m_visualHeaderCtrl;
	std::map <int, ItemColorState>	map_item_to_self_defined_color_;
	virtual void PreSubclassWindow();
};

