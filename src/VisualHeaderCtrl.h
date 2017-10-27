#pragma once
#include "afxcmn.h"

// We need to derive the class from CHeaderCtrl so that we can use an owner drawn function. 
class CVisualHeaderCtrl : public CHeaderCtrl
{
public:
	CVisualHeaderCtrl();
	~CVisualHeaderCtrl();
	int SetSortImage(int nCol, BOOL bAsc);
protected:
	int		m_nSortCol;
	BOOL	m_bSortAsc;				// Indicate the sorting order.
	
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

