#pragma once

class CTreeItem
{
public:
	CTreeItem(void){};
	CTreeItem(const std::string& name,const XYCoordinate& id)
	{
		m_name = name;
		m_id = id;
	}
	virtual ~CTreeItem(void){};


	virtual const std::string&		GetShortName() = 0;
	virtual void SetShortName(const string& name) = 0;
	virtual void		DisplayItem(CZListCtrl &cTree,  CTreeItem* parent) = 0;
	virtual void		AppendSubObjects(CTreeItem* item) = 0;
	virtual const XYCoordinate& GetId()= 0;
	virtual void        DestroyItems() = 0;
	virtual bool        FindItem(CTreeItem* item) = 0;
	virtual bool        FindItemName(const string& name) = 0;
	virtual CTreeItem*  GetItem(CTreeItem* item) = 0;
	virtual bool		HasSubObjects() = 0;
protected:
  std::string m_name;
  std::vector<CTreeItem*> m_listItems;
  XYCoordinate m_id;
};

class CMDSTreeItem : public CTreeItem
{
public:
	CMDSTreeItem(void);
	CMDSTreeItem(const std::string& name,const XYCoordinate& id);
	virtual ~CMDSTreeItem(void);


	virtual const std::string&		GetShortName();
	virtual void SetShortName(const string& name);
	virtual void		DisplayItem(CZListCtrl &cTree,  CTreeItem* parent);
	virtual void		AppendSubObjects(CTreeItem* item);
	virtual bool		HasSubObjects();
	virtual const XYCoordinate& GetId();
	virtual void        DestroyItems();
	virtual bool        FindItem(CTreeItem* item);
	virtual bool        FindItemName(const string& name);
	virtual CTreeItem*  GetItem(CTreeItem* item);
};


