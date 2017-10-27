#if !defined(AFX_GRAPHDATA_H__98FCBEFD_6C06_4605_9DF9_33EAAE04A4FF__INCLUDED_)
#define AFX_GRAPHDATA_H__98FCBEFD_6C06_4605_9DF9_33EAAE04A4FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct _data
{
	long int Value;
	struct _data* Next;
};

class CGraphData  
{
private:
	struct _data *right;  
	bool AllOk;
public:
	bool IsAllOk();
	struct _data *left;
	void NewValue(long int value);
	CGraphData(int size);
	virtual ~CGraphData();
};

#endif  
