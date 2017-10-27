#include "stdafx.h"


CMDSTreeItem::CMDSTreeItem()
{
}


CMDSTreeItem::CMDSTreeItem(const std::string& name,const XYCoordinate& id)
	:CTreeItem(name,id)
{
}

CMDSTreeItem::~CMDSTreeItem()
{

}

const std::string&	CMDSTreeItem::GetShortName()
{
	return m_name;
}

void CMDSTreeItem::SetShortName(const string& name)
{
	m_name = name;
}

const XYCoordinate& CMDSTreeItem::GetId()
{
	return m_id;
}


void CMDSTreeItem::DisplayItem(CZListCtrl &cTree, CTreeItem* parent)
{
	if(HasSubObjects())
	{
		if(m_name != MDSROOT)
		{
			LVFINDINFO info;
			int nIndex;
			info.flags =  LVFI_PARTIAL|LVFI_STRING;
			info.psz = m_name.c_str();
			nIndex= cTree.FindItem(&info);
			if(nIndex == -1)
				int nRow = cTree.InsertItem(m_id.x,(LPCTSTR)m_name.c_str());//插入行
		}
		std::vector<CTreeItem*>::iterator it = m_listItems.begin();
		for(; it != m_listItems.end(); it++)
		{
			(*it)->DisplayItem(cTree,parent);
		
		}
	}
	else
	{
		LVFINDINFO info;
		int nIndex;
		info.flags =  LVFI_PARTIAL|LVFI_STRING;
		info.psz = m_name.c_str();
		nIndex= cTree.FindItem(&info);
		if(nIndex == -1)
			cTree.SetItemText(m_id.x, m_id.y, (LPCTSTR)m_name.c_str());//设置数据
	}
}


bool   CMDSTreeItem::FindItemName(const string& name)
{
	bool find = false;
	if(m_name == name)
		return true;

	if(HasSubObjects())
	{
		std::vector<CTreeItem*>::iterator it = m_listItems.begin();
		for(; it != m_listItems.end(); it++)
		{
			find = (*it)->FindItemName(name);
			if(find)
				return find;

		}
	} 

	return find;

}

 bool CMDSTreeItem::FindItem(CTreeItem* item)
 {
	 bool find = false;
     if(m_id.x == item->GetId().x && m_id.y == item->GetId().y)
			return true;
	 

	if(HasSubObjects())
	{
		std::vector<CTreeItem*>::iterator it = m_listItems.begin();
		for(; it != m_listItems.end(); it++)
		{
			find = (*it)->FindItem(item);
			if(find)
				return find;
		
		}
	} 

	return find;
 }

 CTreeItem*  CMDSTreeItem::GetItem(CTreeItem* item)
 {
	CTreeItem* ct = NULL;
	if(m_id.x == item->GetId().x && m_id.y == item->GetId().y)
			return this;
	 

	if(HasSubObjects())
	{
		std::vector<CTreeItem*>::iterator it = m_listItems.begin();
		for(; it != m_listItems.end(); it++)
		{
			ct = (*it)->GetItem(item);
			if(ct != NULL)
				return ct;
		
		}
	} 

	return ct;
 
 }

void CMDSTreeItem::AppendSubObjects(CTreeItem* item)
{
	if(!FindItem(item))
		m_listItems.push_back(item);
}

bool CMDSTreeItem::HasSubObjects()
{
	return !m_listItems.empty();
}


void  CMDSTreeItem::DestroyItems()
{
	if(HasSubObjects())
	{
		std::vector<CTreeItem*>::iterator it = m_listItems.begin();
		for(; it != m_listItems.end(); it++)
		{
			(*it)->DestroyItems();
			delete *it;
		}
	}
}



