// MultiMonitor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif


// CMultiMonitorApp:
// �йش����ʵ�֣������ MultiMonitor.cpp
//

class CMultiMonitorApp : public CWinApp
{
public:
    CMultiMonitorApp();

    // ��д
public:
    virtual BOOL InitInstance();

    // ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CMultiMonitorApp theApp;