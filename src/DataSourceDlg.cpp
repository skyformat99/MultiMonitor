// DataSourceDlg.cpp : 实现文件
//

#include "stdafx.h"

IMPLEMENT_DYNAMIC(CDataSourceDlg, CDialog)

CDataSourceDlg::CDataSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataSourceDlg::IDD, pParent), m_itemRoot(NULL)
{

}

CDataSourceDlg::~CDataSourceDlg()
{
	for (unsigned int i = 0; i < m_listDSCon.size(); ++i)
	{
		CDSConn* pTemp = m_listDSCon[i];
		pTemp->StopThread();
		delete pTemp;
		pTemp = NULL;
	}

	if (m_itemRoot)
	{
		m_itemRoot->DestroyItems();
		delete m_itemRoot;
		m_itemRoot = NULL;
	}
}
void CDataSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LATESTP, m_lastPriceEC);
	DDX_Control(pDX, IDC_LIST_GRA, m_marketList);
	DDX_Control(pDX, IDC_EDIT_HIGH, m_highPriceEC);
	DDX_Control(pDX, IDC_EDIT_LOW, m_lowPriceEC);
	DDX_Control(pDX, IDC_EDIT_VOL, m_volumeEC);
	DDX_Control(pDX, IDC_EDIT_AMOUNT, m_amountEC);
	DDX_Control(pDX, IDC_EDIT_DATE, m_dateEC);
	DDX_Control(pDX, IDC_EDIT_TIME, m_timeEC);
	DDX_Control(pDX, IDC_EDIT_CODE, m_codeEC);
	DDX_Control(pDX, IDC_EDIT_INVHOLD, m_invstHoldEC);
	DDX_Control(pDX, IDC_EDIT_OPEN, m_openEC);
	DDX_Control(pDX, IDC_EDIT_CLOSE, m_closeEC);
	DDX_Control(pDX, IDC_EDIT_PRESET, m_presettlementEC);
	DDX_Control(pDX, IDC_COMBO_MARKET, m_comMarketlist);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_comboBoxServer);
	DDX_Control(pDX, IDC_STATIC_GRA, m_ctrlStatusGraph);
	
	DDX_Control(pDX, IDC_EDIT_SEL1, m_sel[0]);
	DDX_Control(pDX, IDC_EDIT_SEL2, m_sel[1]);
	DDX_Control(pDX, IDC_EDIT_SEL3, m_sel[2]);
	DDX_Control(pDX, IDC_EDIT_SEL4, m_sel[3]);
	DDX_Control(pDX, IDC_EDIT_SEL5, m_sel[4]);
	
	DDX_Control(pDX, IDC_EDIT_BUY1, m_buy[0]);
	DDX_Control(pDX, IDC_EDIT_BUY2, m_buy[1]);
	DDX_Control(pDX, IDC_EDIT_BUY3, m_buy[2]);
	DDX_Control(pDX, IDC_EDIT_BUY4, m_buy[3]);
	DDX_Control(pDX, IDC_EDIT_BUY5, m_buy[4]);

	DDX_Control(pDX, IDC_EDIT_AVG, m_avgEc);
	DDX_Control(pDX, IDC_EDIT_SETTLE, m_settlementEC);
	DDX_Control(pDX, IDC_EDIT_TRDNUM, m_trdNumEC);
	DDX_Control(pDX, IDC_EDIT_PREINV, m_preInvEC);
}


BEGIN_MESSAGE_MAP(CDataSourceDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_NOTIFYDSLISTUPDATE, OnNotifyMDSListUpdate)
	ON_NOTIFY(NM_CLICK, IDC_LIST_GRA, OnNMClickList)
	ON_CBN_SELCHANGE(IDC_COMBO_MARKET, &CDataSourceDlg::OnCbnSelchangeComboMarket)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVER, &CDataSourceDlg::OnCbnSelchangeComboServer)
	ON_BN_CLICKED(IDCFLASH, &CDataSourceDlg::OnBnClickedFlash)
END_MESSAGE_MAP()

void DataGenerator(LPVOID param)
{
	CStatusGraphCtrl* pCtrl = (CStatusGraphCtrl*)param;
	while(true)
	{
		int nRand = rand();
		while(nRand > pCtrl->GetMaxValue())
			nRand /= 10;
		pCtrl->SetCurrentValue(nRand);
		Sleep(50);
	}
}

int CDataSourceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	m_ctrlStatusGraph.Create("Demo Control",WS_CHILD|WS_VISIBLE,CRect(432,10,910,335),		
		static_cast<CWnd*>(this),IDC_STATIC_GRA);
	m_ctrlStatusGraph.SetMaxValue(100);
	m_ctrlStatusGraph.SetMinValue(0);
	m_ctrlStatusGraph.StartUpdate();;
	m_ctrlStatusGraph.SetGraphMode((StatusGraphType)(1));
	m_init = false;
	AfxBeginThread((AFX_THREADPROC)DataGenerator,(LPVOID)&m_ctrlStatusGraph);
	return CDialog::OnCreate(lpCreateStruct);
}

void CDataSourceDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CDialog::OnClose();
}


BOOL CDataSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	///INIT_EASYSIZE;
	SetIcon(m_hIcon, TRUE);         // 设置大图标
	SetIcon(m_hIcon, FALSE);        // 设置小图标
	windows_mini_lib::SetListCtrlStyle(&m_marketList);
	m_marketList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	CRect rc;
	this->GetClientRect(&rc);

	rc.top += 60;
	rc.bottom -= 8;
	rc.left += 8;
	rc.right -= 8;

	m_marketList.MoveWindow(&rc);

	m_marketList.InsertColumn(0, _T("Code"), LVCFMT_CENTER, 70);
	m_marketList.InsertColumn(1, _T("Name"), LVCFMT_CENTER, 150);
	m_marketList.InsertColumn(2, _T("Market"), LVCFMT_CENTER, 80);

	XYCoordinate r(-1,-1,WHITE);
	m_itemRoot = new CMDSTreeItem(MDSROOT,r);

	LoadDSServer("./jgserver.xml");
	InitServers();
	return TRUE;  //return TRUE unless you set the focus to a control
}



void CDataSourceDlg::InitServers()
{
	bool created = false;
	std::map<string,std::vector<string> >::iterator it = m_mapServers.begin();
	for( ; it != m_mapServers.end(); it++)
	{
		string server = (*it).first;
		if(!created)
		{
			std::vector<string> markets = (*it).second;
			std::vector<string>::iterator iv = markets.begin();
			for(; iv != markets.end(); iv++)
			{
				m_comMarketlist.AddString((*iv).c_str());
			}
			m_comMarketlist.SetCurSel(0);
			created = true;
		}
	}
}

// Comment out the following statements cause it does nothing.
// On the other hand, it seems they make the program fail to exit normally.	
LRESULT CDataSourceDlg::OnNotifyMDSListUpdate(WPARAM wp, LPARAM lp)
{
	/*int curSel = m_comMarketlist.GetCurSel();
	CString selmarket ;
	m_comMarketlist.SetCurSel(curSel);
	m_comMarketlist.GetLBText(curSel, selmarket);
	mapMarkets::iterator it = m_marketData.find(selmarket.GetBuffer(0));
	if (it != m_marketData.end())
		return 0 ;*/

	//CAutoLock lock(m_lock);
	//for (unsigned int i = 0; i < m_listDSCon.size(); i++)
	//{
	//	m_marketData = m_listDSCon[0]->GetData();
	//	stringstream ss;
	//	mapMarkets::iterator it = m_marketData.begin();
	//	for ( ; it != m_marketData.end(); it++)
	//	{
	//		string market = (*it).first;
	//		map<string, SStdQt> qt = (*it).second;
	//		
	//		SStdQt q = qt["HSIH6"];
	//	}
	//}
	
	//InitCodeList();
	//UpdateWindow();
	return 0;
}

void CDataSourceDlg::OnSize(UINT nType, int cx, int cy)
{
	  CDialog::OnSize(nType, cx, cy);
	static int  m_intialized = 0;

    // TODO: 在此处添加消息处理程序代码
	if (m_intialized >= 1 && (nType == SIZE_RESTORED  || nType == SIZE_MAXIMIZED) )
    {
		CRect rect,rcMarketList,rcStServer,rcStProduct,rcComServer,rcComProduct,rcStHighPrice,rcEdHighPrice,
			rcStLowPrice,rcEdLowPrice,rcStVol,rcEdVol,rcStAmount,rcEdAmout,rcStDate,rcEdDate,rcStTime,rcEdTime,
			rcStCode,rcEdCode,rcStInvstHld,rcEdInvstHld,rcStOpen,rcEdOpen,rcStClose,rcEdClose,rcStPreset,rcEdPreset,
			rcStatusGra,rcStBuy[5],rcEdBuy[5],rcStSell[5],rcEdSell[5],rcStAvg,rcEdAvg,rcStSettle,rcEdSettle,rcStTrdNum,rcEdTrdNum,
			rcStPreInv,rcEdPreInv,rcStPrice,rcEdPrice;
	
		int maxWidth = 0, colSize = 0;
		if (nType == SIZE_MAXIMIZED)
		{
			maxWidth = 150;
			colSize = 50;
		}
		GetWindowRect(&rect);//获取当前控件大小；
		ScreenToClient(&rect);  
		
		rcStServer.top = rect.top + 10;
		rcStServer.left  = rect.left + 30;
		rcStServer.right = rcStServer.left + 70;
		rcStServer.bottom = rcStServer.top + 15;
		//GetDlgItem(IDC_STATIC_SERVER)->MoveWindow(&rcStServer);

		rcComServer.top = rect.top + 10;
		rcComServer.left = rcStServer.right + 20;
		rcComServer.right = rcComServer.left + 130;
		rcComServer.bottom = rcComServer.top + 15;
		//GetDlgItem(IDC_COMBO_SERVER)->MoveWindow(&rcComServer);

		rcStProduct.top = rect.top + 10;
		rcStProduct.left = rcComServer.right + 10;
		rcStProduct.right = rcStProduct.left + 50;
		rcStProduct.bottom = rcStProduct.top + 15;
		//GetDlgItem(IDC_STATIC_Market)->MoveWindow(&rcStProduct);

		rcComProduct.top = rect.top + 10;
		rcComProduct.left = rcStProduct.right + 10;
		rcComProduct.right = rcComProduct.left + 80;
		rcComProduct.bottom = rcComProduct.top + 15;
		//GetDlgItem(IDC_COMBO_MARKET)->MoveWindow(&rcComProduct);

		//rcMarketList.left = rect.left + 30;
		rcMarketList.top = rcStServer.bottom + 15;
		rcMarketList.bottom = rect.bottom - 10;
		// Fix the width of the list ctrl when resizing the window.
		rcMarketList.right = 320;//rect.left + 400;
		m_marketList.MoveWindow(&rcMarketList);

		rcStatusGra.bottom = rect.top + 10;
		rcStatusGra.left = rcMarketList.right + 30;
		rcStatusGra.right = rect.right - 150;
		rcStatusGra.bottom = rect.bottom - 200;
		GetDlgItem(IDC_STATIC_GRA)->MoveWindow(&rcStatusGra);
		//DATE  LINE 1
		rcStDate.top = rcStatusGra.bottom + 10;
		rcStDate.left = rcMarketList.right + 10;
		rcStDate.right = rcStDate.left + 70;
		rcStDate.bottom = rcStDate.top + 20;
		GetDlgItem(IDC_STATIC_DATE)->MoveWindow(&rcStDate);

		rcEdDate.top = rcStatusGra.bottom + 10;
		rcEdDate.left = rcStDate.right + 10 + colSize;
		rcEdDate.right = rcEdDate.left + 70;
		rcEdDate.bottom = rcEdDate.top + 20;
		GetDlgItem(IDC_EDIT_DATE)->MoveWindow(&rcEdDate);
		// TIME 
		rcStTime.top = rcStatusGra.bottom + 10;
		rcStTime.left = rcEdDate.right + 10 + maxWidth;
		rcStTime.right = rcStTime.left + 70;
		rcStTime.bottom = rcStTime.top + 20;
		GetDlgItem(IDC_STATIC_TIME)->MoveWindow(&rcStTime);

		rcEdTime.top = rcStatusGra.bottom + 10;
		rcEdTime.left = rcStTime.right + 10 + colSize;
		rcEdTime.right = rcEdTime.left + 70;
		rcEdTime.bottom = rcEdTime.top + 20;
		GetDlgItem(IDC_EDIT_TIME)->MoveWindow(&rcEdTime);
		// CODE
		rcStCode.top = rcStatusGra.bottom + 10;
		rcStCode.left = rcEdTime.right + 10 + maxWidth;
		rcStCode.right = rcStCode.left + 70;
		rcStCode.bottom = rcStCode.top + 20;
		GetDlgItem(IDC_STATIC_CODE)->MoveWindow(&rcStCode);

		rcEdCode.top = rcStatusGra.bottom + 10;
		rcEdCode.left = rcStCode.right + 10 + colSize;
		rcEdCode.right = rcEdCode.left + 70;
		rcEdCode.bottom = rcEdCode.top + 20;
		GetDlgItem(IDC_EDIT_CODE)->MoveWindow(&rcEdCode);
		//INVEST HOLDING
		rcStInvstHld.top = rcStatusGra.bottom + 10;
		rcStInvstHld.left = rcEdCode.right + 10 + maxWidth;
		rcStInvstHld.right = rcStInvstHld.left + 70;
		rcStInvstHld.bottom = rcStInvstHld.top + 20;
		GetDlgItem(IDC_STATIC_PREINV)->MoveWindow(&rcStInvstHld);

		rcEdInvstHld.top = rcStatusGra.bottom + 10;
		rcEdInvstHld.left = rcStInvstHld.right + 10 + colSize;
		rcEdInvstHld.right = rcEdInvstHld.left + 70;
		rcEdInvstHld.bottom = rcEdInvstHld.top + 20;
		GetDlgItem(IDC_EDIT_PREINV)->MoveWindow(&rcEdInvstHld);
		//LASTEST PRICE LINE 2
		rcStPrice.top = rcStatusGra.bottom + 40;
		rcStPrice.left = rcMarketList.right + 10;
		rcStPrice.right = rcStPrice.left + 70;
		rcStPrice.bottom = rcStPrice.top + 20;
		GetDlgItem(IDC_STATIC_LATESTP)->MoveWindow(&rcStPrice);

		rcEdPrice.top = rcStatusGra.bottom + 40;
		rcEdPrice.left = rcStPrice.right + 10 + colSize;
		rcEdPrice.right = rcEdPrice.left + 70;
		rcEdPrice.bottom = rcEdPrice.top + 20;
		GetDlgItem(IDC_EDIT_LATESTP)->MoveWindow(&rcEdPrice);
		//VOL
		rcStVol.top = rcStatusGra.bottom + 40;
		rcStVol.left = rcEdPrice.right + 10 + maxWidth;
		rcStVol.right = rcStVol.left + 70;
		rcStVol.bottom = rcStVol.top + 20;
		GetDlgItem(IDC_STATIC_VOL)->MoveWindow(&rcStVol);

		rcEdVol.top = rcStatusGra.bottom + 40;
		rcEdVol.left = rcStVol.right + 10 + colSize;
		rcEdVol.right = rcEdVol.left + 70;
		rcEdVol.bottom = rcEdVol.top + 20;
		GetDlgItem(IDC_EDIT_VOL)->MoveWindow(&rcEdVol);
		//AMOUNT
		rcStAmount.top = rcStatusGra.bottom + 40;
		rcStAmount.left = rcEdVol.right + 10 + maxWidth;
		rcStAmount.right = rcStAmount.left + 70;
		rcStAmount.bottom = rcStAmount.top + 20;
		GetDlgItem(IDC_STATIC_AMOUNT)->MoveWindow(&rcStAmount);

		rcEdAmout.top = rcStatusGra.bottom + 40;
		rcEdAmout.left = rcStAmount.right + 10 + colSize;
		rcEdAmout.right = rcEdAmout.left + 70 ;
		rcEdAmout.bottom = rcEdAmout.top + 20;
		GetDlgItem(IDC_EDIT_AMOUNT)->MoveWindow(&rcEdAmout);
		//TRDNUM
		rcStTrdNum.top = rcStatusGra.bottom + 40;
		rcStTrdNum.left = rcEdAmout.right + 10 + maxWidth;
		rcStTrdNum.right = rcStTrdNum.left + 70;
		rcStTrdNum.bottom = rcStTrdNum.top + 20;
		GetDlgItem(IDC_STATIC_TRDNUM)->MoveWindow(&rcStTrdNum);

		rcEdTrdNum.top = rcStatusGra.bottom + 40;
		rcEdTrdNum.left = rcStTrdNum.right + 10 + colSize;
		rcEdTrdNum.right = rcEdTrdNum.left + 70;
		rcEdTrdNum.bottom = rcEdTrdNum.top + 20;
		GetDlgItem(IDC_EDIT_TRDNUM)->MoveWindow(&rcEdTrdNum);
		//HIGH new line
		rcStHighPrice.top = rcStatusGra.bottom + 70;
		rcStHighPrice.left = rcMarketList.right + 10;
		rcStHighPrice.right = rcStHighPrice.left + 70;
		rcStHighPrice.bottom = rcStHighPrice.top + 20;
		GetDlgItem(IDC_STATIC_HIGH)->MoveWindow(&rcStHighPrice);

		rcEdHighPrice.top = rcStatusGra.bottom + 70;
		rcEdHighPrice.left = rcStHighPrice.right + 10 + colSize;
		rcEdHighPrice.right = rcEdHighPrice.left + 70;
		rcEdHighPrice.bottom = rcEdHighPrice.top + 20;
		GetDlgItem(IDC_EDIT_HIGH)->MoveWindow(&rcEdHighPrice);
		//LOW
		rcStLowPrice.top = rcStatusGra.bottom + 70;
		rcStLowPrice.left = rcEdHighPrice.right + 10 + maxWidth;
		rcStLowPrice.right = rcStLowPrice.left + 70;
		rcStLowPrice.bottom = rcStLowPrice.top + 20;
		GetDlgItem(IDC_STATIC_LOW)->MoveWindow(&rcStLowPrice);

		rcEdLowPrice.top = rcStatusGra.bottom + 70;
		rcEdLowPrice.left = rcStLowPrice.right + 10 + colSize;
		rcEdLowPrice.right = rcEdLowPrice.left + 70;
		rcEdLowPrice.bottom = rcEdLowPrice.top + 20;
		GetDlgItem(IDC_EDIT_LOW)->MoveWindow(&rcEdLowPrice);
		//OPEN
		rcStOpen.top = rcStatusGra.bottom + 70;
		rcStOpen.left = rcEdLowPrice.right + 10 + maxWidth;
		rcStOpen.right = rcStOpen.left + 70;
		rcStOpen.bottom = rcStOpen.top + 20;
		GetDlgItem(IDC_STATIC_OPEN)->MoveWindow(&rcStOpen);

		rcEdOpen.top = rcStatusGra.bottom + 70;
		rcEdOpen.left = rcStOpen.right + 10 + colSize;
		rcEdOpen.right = rcEdOpen.left + 70;
		rcEdOpen.bottom = rcEdOpen.top + 20;
		GetDlgItem(IDC_EDIT_OPEN)->MoveWindow(&rcEdOpen);
		//AVG
		rcStAvg.top = rcStatusGra.bottom + 70;
		rcStAvg.left = rcEdOpen.right + 10 + maxWidth;
		rcStAvg.right = rcStAvg.left + 70;
		rcStAvg.bottom = rcStAvg.top + 20;
		GetDlgItem(IDC_STATIC_AVG)->MoveWindow(&rcStAvg);

		rcEdAvg.top = rcStatusGra.bottom + 70;
		rcEdAvg.left = rcStAvg.right + 10 + colSize;
		rcEdAvg.right = rcEdAvg.left + 70;
		rcEdAvg.bottom = rcEdAvg.top + 20;
		GetDlgItem(IDC_EDIT_AVG)->MoveWindow(&rcEdAvg);
		//CLOSE  NEW LINE
		rcStClose.top = rcStatusGra.bottom + 100;
		rcStClose.left = rcMarketList.right + 10;
		rcStClose.right = rcStClose.left + 70;
		rcStClose.bottom = rcStClose.top + 20;
		GetDlgItem(IDC_STATIC_CLOSE)->MoveWindow(&rcStClose);

		rcEdClose.top = rcStatusGra.bottom + 100;
		rcEdClose.left = rcStClose.right + 10 + colSize;
		rcEdClose.right = rcEdClose.left + 70;
		rcEdClose.bottom = rcEdClose.top + 20;
		GetDlgItem(IDC_EDIT_CLOSE)->MoveWindow(&rcEdClose);
		CRect rcStIh, rcEdIh,rcStPrev,rcEdPrev;
	 
		rcStIh.top = rcStatusGra.bottom + 100;
		rcStIh.left = rcEdClose.right + 10 + maxWidth;
		rcStIh.right = rcStIh.left + 70;
		rcStIh.bottom = rcStIh.top + 20;
		GetDlgItem(IDC_STATIC_INVHOLD)->MoveWindow(&rcStIh);

		rcEdIh.top = rcStatusGra.bottom + 100;
		rcEdIh.left = rcStIh.right + 10 + colSize;
		rcEdIh.right = rcEdIh.left + 70;
		rcEdIh.bottom = rcEdIh.top + 20;
		GetDlgItem(IDC_EDIT_INVHOLD)->MoveWindow(&rcEdIh);
		//PRESET
		rcStPrev.top = rcStatusGra.bottom + 100;
		rcStPrev.left = rcEdIh.right + 10 + maxWidth;
		rcStPrev.right = rcStPrev.left + 70;
		rcStPrev.bottom = rcStPrev.top + 20;
		GetDlgItem(IDC_STATIC_PRESET)->MoveWindow(&rcStPrev);

		rcEdPrev.top = rcStatusGra.bottom + 100;
		rcEdPrev.left = rcStPrev.right + 10 + colSize;
		rcEdPrev.right = rcEdPrev.left + 70;
		rcEdPrev.bottom = rcEdPrev.top + 20;
		GetDlgItem(IDC_EDIT_PRESET)->MoveWindow(&rcEdPrev);
		//SETTLE
		rcStSettle.top = rcStatusGra.bottom + 100;
		rcStSettle.left = rcEdPrev.right + 10 + maxWidth;
		rcStSettle.right = rcStSettle.left + 70;
		rcStSettle.bottom = rcStSettle.top + 20;
		GetDlgItem(IDC_STATIC_SETTLE)->MoveWindow(&rcStSettle);

		rcEdSettle.top = rcStatusGra.bottom + 100;
		rcEdSettle.left = rcStSettle.right + 10 + colSize;
		rcEdSettle.right = rcEdSettle.left + 70;
		rcEdSettle.bottom = rcEdSettle.top + 20;
		GetDlgItem(IDC_EDIT_SETTLE)->MoveWindow(&rcEdSettle);

		//rcStBuy[5],rcEdBuy[5],rcStSell[5],rcEdSell[5]
		for(int i = 4; i >= 0; i--)
		{
			if(i == 4)
			{
				rcStSell[i].left = rcStatusGra.right + 10;
				rcStSell[i].right = rcStSell[i].left + 50;
				rcStSell[i].top = rcStatusGra.top + 10;
				rcStSell[i].bottom = rcStSell[i].top + 20;

				rcEdSell[i].left = rcStSell[i].right + 10;
				rcEdSell[i].right = rcEdSell[i].left + 50;
				rcEdSell[i].top = rcStatusGra.top + 10;
				rcEdSell[i].bottom = rcEdSell[i].top + 20;

				GetDlgItem(IDC_STATIC_SEL5)->MoveWindow(&rcStSell[i]);
				GetDlgItem(IDC_EDIT_SEL5)->MoveWindow(&rcEdSell[i]);
			}
			else
			{
				rcStSell[i].left = rcStatusGra.right + 10;
				rcStSell[i].right = rcStSell[i].left + 50;
				rcStSell[i].top = rcStSell[i+1].bottom + 10;
				rcStSell[i].bottom = rcStSell[i].top + 20;

				rcEdSell[i].left = rcStSell[i].right + 10;
				rcEdSell[i].right = rcEdSell[i].left + 50;
				rcEdSell[i].top = rcStSell[i+1].bottom + 10;
				rcEdSell[i].bottom = rcEdSell[i].top + 20;

				if(i == 3)
				{
					GetDlgItem( IDC_STATIC_SEL4)->MoveWindow(&rcStSell[i]);
					GetDlgItem(IDC_EDIT_SEL4)->MoveWindow(&rcEdSell[i]);
				}
				else if( i == 2)
				{
					GetDlgItem( IDC_STATIC_SEL3)->MoveWindow(&rcStSell[i]);
					GetDlgItem(IDC_EDIT_SEL3)->MoveWindow(&rcEdSell[i]);
				}
				else if(i == 1)
				{
					GetDlgItem( IDC_STATIC_SEL2)->MoveWindow(&rcStSell[i]);
					GetDlgItem(IDC_EDIT_SEL2)->MoveWindow(&rcEdSell[i]);
				}
				else 
				{
					GetDlgItem( IDC_STATIC_SEL1)->MoveWindow(&rcStSell[i]);
					GetDlgItem(IDC_EDIT_SEL1)->MoveWindow(&rcEdSell[i]);
				
				}
			}
		}

		for(int i = 0; i <= 4; i++)
		{
			if(i == 0)
			{
				rcStBuy[i].left = rcStatusGra.right + 10;
				rcStBuy[i].right = rcStBuy[i].left + 50;
				rcStBuy[i].top = rcStSell[0].bottom + 10;
				rcStBuy[i].bottom = rcStBuy[i].top + 20;

				rcEdBuy[i].left = rcStBuy[i].right + 10;
				rcEdBuy[i].right = rcEdBuy[i].left + 50;
				rcEdBuy[i].top = rcEdSell[0].bottom + 10;
				rcEdBuy[i].bottom = rcEdBuy[i].top + 20;

				GetDlgItem(IDC_STATIC_BUY1)->MoveWindow(&rcStBuy[i]);
				GetDlgItem(IDC_EDIT_BUY1)->MoveWindow(&rcEdBuy[i]);
			}
			else
			{
				rcStBuy[i].left = rcStatusGra.right + 10;
				rcStBuy[i].right = rcStBuy[i].left + 50;
				rcStBuy[i].top = rcStBuy[i-1].bottom + 10;
				rcStBuy[i].bottom = rcStBuy[i].top + 20;

				rcEdBuy[i].left = rcStBuy[i].right + 10;
				rcEdBuy[i].right = rcEdBuy[i].left + 50;
				rcEdBuy[i].top = rcEdBuy[i-1].bottom + 10;
				rcEdBuy[i].bottom = rcEdBuy[i].top + 20;

				if(i == 1)
				{
					GetDlgItem( IDC_STATIC_BUY2)->MoveWindow(&rcStBuy[i]);
					GetDlgItem(IDC_EDIT_BUY2)->MoveWindow(&rcEdBuy[i]);
				}
				else if( i == 2)
				{
					GetDlgItem( IDC_STATIC_BUY3)->MoveWindow(&rcStBuy[i]);
					GetDlgItem(IDC_EDIT_BUY3)->MoveWindow(&rcEdBuy[i]);
				}
				else if(i == 3)
				{
					GetDlgItem( IDC_STATIC_BUY4)->MoveWindow(&rcStBuy[i]);
					GetDlgItem(IDC_EDIT_BUY4)->MoveWindow(&rcEdBuy[i]);
				}
				else 
				{
					GetDlgItem( IDC_STATIC_BUY5)->MoveWindow(&rcStBuy[i]);
					GetDlgItem(IDC_EDIT_BUY5)->MoveWindow(&rcEdBuy[i]);
				
				}
			}
		}

		CRect rcCancel;
		rcCancel.left = rcStatusGra.right + 10;
		rcCancel.right = rcCancel.left + 70;
		rcCancel.top  = rect.bottom - 30;
		rcCancel.bottom = rcCancel.top + 30;
		GetDlgItem(IDCFLASH)->MoveWindow(&rcCancel);
	}

	m_intialized++;
}

void CDataSourceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


void  CDataSourceDlg::DestroyListItems()
{
	CAutoLock lock(m_lock);
	m_marketList.DeleteAllItems();
	if (m_itemRoot != NULL)
	{
		m_itemRoot->DestroyItems();
		delete m_itemRoot;
	}

	XYCoordinate r(-1,-1,WHITE);
	m_itemRoot = new CMDSTreeItem(MDSROOT,r);
}

void  CDataSourceDlg::InitCodeList()
{
	CAutoLock lock(m_lock);
	DestroyListItems();	
	InitMarketData();

	int curSel = m_comMarketlist.GetCurSel();
	CString selmarket ;
	m_comMarketlist.SetCurSel(curSel);
	m_comMarketlist.GetLBText(curSel,selmarket);
	mapMarkets::iterator it = m_marketData.find(selmarket.GetBuffer(0));
	if( it != m_marketData.end())
	{
		int k = 0;
		map<string, SStdQt> mq = (*it).second;
		map<string, SStdQt>::iterator itq = mq.begin();
		for(; itq != mq.end(); itq++)
		{
			int m = 0;
			SStdQt q = (*itq).second;
			XYCoordinate c,xport,xstatus;
			c.x = k;
			c.y = m++;
			CMDSTreeItem codeItem(string(q.m_pchCode),c);
			CMDSTreeItem* row = NULL;
			if(!m_itemRoot->FindItem(&codeItem))
			{
				row  = new CMDSTreeItem(string(q.m_pchCode),c);
				m_itemRoot->AppendSubObjects(row);
			}
			xport.x = k;
			xport.y = m++;
			CMDSTreeItem nameItem(string(q.m_pcName),xport);
			if(!m_itemRoot->FindItem(&nameItem))
			{
				CMDSTreeItem* itemPort = new CMDSTreeItem(string(q.m_pcName),xport);
				row->AppendSubObjects(itemPort);
			}
			xstatus.x = k;
			xstatus.y = m++;
			CMDSTreeItem marketItem(string(q.m_pchMarket),xstatus);
			if(!m_itemRoot->FindItem(&marketItem))
			{
				CMDSTreeItem* itemStatus = new CMDSTreeItem(string(q.m_pchMarket),xstatus);
				row->AppendSubObjects(itemStatus);
			}
			if(!m_init)
			{
				m_init = true;
				string price,high,low,date,time,invst,open,close,preset,amount,volume,code,avg,preInv,settle,trdNum;
				vector<string> buyPrice;
				vector<string> selPrice;
				
				buyPrice.resize(5);
				selPrice.resize(5);
				CString name = m_marketList.GetItemText(0,0);
				string  _name = name.GetBuffer();
				map<string, SStdQt>::iterator t = mq.find(_name);
				if(t != mq.end())
				{
					price=convert<string>(q.m_dblPrice);
					m_lastPriceEC.SetWindowText(price.c_str());
					high=convert<string>(q.m_dblHigh);
					m_highPriceEC.SetWindowText(high.c_str());
					date=convert<string>(q.m_dwDate);
					m_dateEC.SetWindowText(date.c_str());
					time=convert<string>(q.m_dwTime);
					m_timeEC.SetWindowText(time.c_str());
					low = convert<string>(q.m_dblLow);
					m_lowPriceEC.SetWindowText(low.c_str());
					open = convert<string>(q.m_dblOpen);
					m_openEC.SetWindowText(open.c_str());
					close = convert<string>(q.m_dblClose);
					m_closeEC.SetWindowText(close.c_str());
					/*preset = convert<string>(q.m_dblPreAvgPrice);
					m_presettlementEC.SetWindowText(preset.c_str());*/
					amount = convert<string>(q.m_dblAmount);
					m_amountEC.SetWindowText(amount.c_str());
					volume =  convert<string>(q.m_xVolume);
					m_volumeEC.SetWindowText(volume.c_str());
					code = convert<string>(q.m_pchCode);
					m_codeEC.SetWindowText(code.c_str());
					/*avg = convert<string>(q.m_dblAvgPrice);
					m_avgEc.SetWindowText(avg.c_str());*/
					preInv = convert<string>(q.m_xPreOpenInterest);
					m_preInvEC.SetWindowText(preInv.c_str());
					trdNum = convert<string>(q.m_xTradeNum);
					/*m_trdNumEC.SetWindowText(trdNum.c_str());
					settle = convert<string>(q.m_dblAvgPrice);*/
					m_settlementEC.SetWindowText(settle.c_str());
					invst = convert<string>(q.m_xOpenInterest);
					m_invstHoldEC.SetWindowText(invst.c_str());

					buyPrice[0] = convert<string>(q.m_pdwMMP[5]);
					m_buy[0].SetWindowText(	buyPrice[0].c_str());
					selPrice[0] = convert<string>(q.m_pdwMMP[4]);
					m_sel[0].SetWindowText(	selPrice[0].c_str());
					buyPrice[1] = convert<string>(q.m_pdwMMP[6]);
					m_buy[1].SetWindowText(	buyPrice[1].c_str());
					selPrice[1] = convert<string>(q.m_pdwMMP[3]);
					m_sel[1].SetWindowText(	selPrice[1].c_str());

					buyPrice[2] = convert<string>(q.m_pdwMMP[7]);
					m_buy[2].SetWindowText(	buyPrice[2].c_str());
					selPrice[2] = convert<string>(q.m_pdwMMP[2]);
					m_sel[2].SetWindowText(	selPrice[2].c_str());

					buyPrice[3] = convert<string>(q.m_pdwMMP[8]);
					m_buy[3].SetWindowText(	buyPrice[3].c_str());
					selPrice[3] = convert<string>(q.m_pdwMMP[1]);
					m_sel[3].SetWindowText(	selPrice[3].c_str());

					buyPrice[4] = convert<string>(q.m_pdwMMP[9]);
					m_buy[4].SetWindowText(	buyPrice[4].c_str());
					selPrice[4] = convert<string>(q.m_pdwMMP[0]);
					m_sel[4].SetWindowText(	selPrice[4].c_str());
				}	 
			}
			k++;
		}
	}

	m_itemRoot->DisplayItem(m_marketList,m_itemRoot);
}

void CDataSourceDlg::OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FlashData();
	UpdateWindow();
}

void CDataSourceDlg::InitMarketData()
{
	CAutoLock lock(m_lock);
	int curSel = m_comboBoxServer.GetCurSel();
	CString selectedServer;
	m_comboBoxServer.GetLBText(curSel, selectedServer);
	string server = selectedServer.GetBuffer();
	for (auto ic : m_listDSCon) {
		if (ic->GetServerName() == server) {
			m_marketData = ic->GetData();
			break;
		}
	}
}
void CDataSourceDlg::PrintStdQtToScreen()
{
	CAutoLock lock(m_lock);
	int nIndex = m_marketList.GetSelectionMark();
	if (nIndex >= 0)
	{
		int curSel = m_comMarketlist.GetCurSel();
		CString selmarket ;
		m_comMarketlist.GetWindowTextA(selmarket);
		mapMarkets::iterator it = m_marketData.find(selmarket.GetBuffer(0));
		if( it != m_marketData.end())
		{
			map<string, SStdQt> mq = (*it).second;
			string price,high,low,date,time,invst,open,close,preset,amount,volume,code,avg,preInv,settle,trdNum;
			vector<string> buyPrice;
			vector<string> selPrice;

			buyPrice.resize(5);
			selPrice.resize(5);
			CString name = m_marketList.GetItemText(nIndex,0);
			string  _name = name.GetBuffer();
			map<string, SStdQt>::iterator t = mq.find(_name);
			if(t != mq.end())
			{
				SStdQt q = (*t).second;
				price = convert<string>(q.m_dblPrice);
				m_lastPriceEC.SetWindowText(price.c_str());
				high=convert<string>(q.m_dblHigh);
				m_highPriceEC.SetWindowText(high.c_str());
				date=convert<string>(q.m_dwDate);
				m_dateEC.SetWindowText(date.c_str());
				time=convert<string>(q.m_dwTime);
				m_timeEC.SetWindowText(time.c_str());
				low = convert<string>(q.m_dblLow);
				m_lowPriceEC.SetWindowText(low.c_str());
				open = convert<string>(q.m_dblOpen);
				m_openEC.SetWindowText(open.c_str());
				close = convert<string>(q.m_dblClose);
				m_closeEC.SetWindowText(close.c_str());
				/*preset = convert<string>(q.m_dblPreAvgPrice);
				m_presettlementEC.SetWindowText(preset.c_str());*/
				amount = convert<string>(q.m_dblAmount);
				m_amountEC.SetWindowText(amount.c_str());
				volume =  convert<string>(q.m_xVolume);
				m_volumeEC.SetWindowText(volume.c_str());
				code = convert<string>(q.m_pchCode);
				m_codeEC.SetWindowText(code.c_str());
				/*avg = convert<string>(q.m_dblAvgPrice);
				m_avgEc.SetWindowText(avg.c_str());*/
				preInv = convert<string>(q.m_xPreOpenInterest);
				m_preInvEC.SetWindowText(preInv.c_str());
				trdNum = convert<string>(q.m_xTradeNum);
				m_trdNumEC.SetWindowText(trdNum.c_str());
				/*settle = convert<string>(q.m_dblAvgPrice);
				m_settlementEC.SetWindowText(settle.c_str());*/
				invst = convert<string>(q.m_xOpenInterest);
				m_invstHoldEC.SetWindowText(invst.c_str());

				buyPrice[0] = convert<string>(q.m_pdwMMP[5]);
				m_buy[0].SetWindowText(	buyPrice[0].c_str());
				selPrice[0] = convert<string>(q.m_pdwMMP[4]);
				m_sel[0].SetWindowText(	selPrice[0].c_str());

				buyPrice[1] = convert<string>(q.m_pdwMMP[6]);
				m_buy[1].SetWindowText(	buyPrice[1].c_str());
				selPrice[1] = convert<string>(q.m_pdwMMP[3]);
				m_sel[1].SetWindowText(	selPrice[1].c_str());

				buyPrice[2] = convert<string>(q.m_pdwMMP[7]);
				m_buy[2].SetWindowText(	buyPrice[2].c_str());
				selPrice[2] = convert<string>(q.m_pdwMMP[2]);
				m_sel[2].SetWindowText(	selPrice[2].c_str());

				buyPrice[3] = convert<string>(q.m_pdwMMP[8]);
				m_buy[3].SetWindowText(	buyPrice[3].c_str());
				selPrice[3] = convert<string>(q.m_pdwMMP[1]);
				m_sel[3].SetWindowText(	selPrice[3].c_str());

				buyPrice[4] = convert<string>(q.m_pdwMMP[9]);
				m_buy[4].SetWindowText(	buyPrice[4].c_str());
				selPrice[4] = convert<string>(q.m_pdwMMP[0]);
				m_sel[4].SetWindowText(	selPrice[4].c_str());
			}	
		}
	}
}

void CDataSourceDlg::FlashData()
{
	InitMarketData();
	PrintStdQtToScreen();
}


bool CDataSourceDlg::LoadDSServer(const string & strFileName)
{
	using boost::property_tree::ptree;
	try
	{
		ptree pt;
		read_xml(strFileName, pt);
		{
			ptree serverlist = pt.get_child("serverlist");
			for (ptree::iterator itr = serverlist.begin(); itr!=serverlist.end(); itr++)
			{
				std::vector<string> vm;
				ptree svr = itr->second;
				string strServer = svr.get<string>("<xmlattr>.ip");
				int intPort = svr.get<int>("<xmlattr>.port");
				string strNote = svr.get<string>("<xmlattr>.note");
				bool blnSubTrans = svr.get<bool>("<xmlattr>.subtrans", false);
				//ptree server = svr.get_child("serverlist.server");
				int index = 0;
				for(BOOST_AUTO(pos,svr.begin());pos != svr.end();++pos)
				{
					if(index > 0)
					{
						string key =  pos->first;
						string val =  pos->second.data();
						vm.push_back(val);
					}
					index++;
				}
			
				string strport = convert<string>(intPort);
				string fullServer = strServer + ":" + strport;
				m_mapServers[fullServer] = vm;
				m_comboBoxServer.AddString(fullServer.c_str());
				m_comboBoxServer.SetCurSel(0);
				CDSConn* pCon = new CDSConn;
				pCon->InitConnect(strServer, intPort,m_hWnd,vm,fullServer);
				pCon->StartThread();
				m_listDSCon.push_back(pCon);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}


// CDataSourceDlg 消息处理程序


void CDataSourceDlg::OnCbnSelchangeComboMarket()
{
	// TODO: 在此添加控件通知处理程序代码
	InitCodeList();
	FlashData();
	UpdateWindow();
}


void CDataSourceDlg::OnCbnSelchangeComboServer()
{
	int curSel = m_comboBoxServer.GetCurSel();
	CString selectedServer ;

	m_comboBoxServer.GetLBText(curSel,selectedServer);
	m_comMarketlist.ResetContent();
	std::map<string,std::vector<string> >::iterator it = m_mapServers.find(selectedServer.GetBuffer());
	if(it != m_mapServers.end())
	{
		string server = (*it).first;
		std::vector<string> markets = (*it).second;
		std::vector<string>::iterator iv = markets.begin();
		for(; iv != markets.end(); iv++)
		{
			m_comMarketlist.AddString((*iv).c_str());
		}
		m_comMarketlist.SetCurSel(0);
	}

	InitCodeList();
	FlashData();
	UpdateWindow();
}


void CDataSourceDlg::OnBnClickedFlash()
{
	// TODO: 在此添加控件通知处理程序代码
	FlashData();
	UpdateWindow();
}


