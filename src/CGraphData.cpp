#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGraphData::CGraphData(int size)
{
	int i;
	struct _data *temp=NULL,*tleft;
	AllOk=false;
	left=(struct _data *)malloc(sizeof(struct _data));
	left->Next=NULL;
	tleft=left;
	for(i=1;i<size;i++) {
		temp=(struct _data *)malloc(sizeof(struct _data));
		temp->Value=0;
		temp->Next=NULL;
		left->Next=temp;
		left=temp;
	}
	right=left;
	left=tleft;
	AllOk=true;
}

CGraphData::~CGraphData()
{
	_data* temp;
	while(left)
	{
		temp = left->Next;
		delete left;
		left = temp;
	}
}

void CGraphData::NewValue(long int value)
{
	_data *temp;
	AllOk=false;
	// insertion of new node at the right
	temp=new _data;
	temp->Value=value;
	temp->Next=NULL;
	right->Next=temp;
	right=temp;
	
	// deletion of left most node
	temp=left;
	left=left->Next;
	delete temp;
	AllOk=true;
}

bool CGraphData::IsAllOk()
{
	return AllOk;
}