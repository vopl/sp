////////////////////////////////////////////////////////////////////////////
// This file is automatically generated by VC++ 6.0 MFC Wizard
////////////////////////////////////////////////////////////////////////////

// zoomscrolldemo2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "zoomscrolldemo2.h"
#include "zoomscrolldemo2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZoomscrolldemo2App

BEGIN_MESSAGE_MAP(CZoomscrolldemo2App, CWinApp)
	//{{AFX_MSG_MAP(CZoomscrolldemo2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomscrolldemo2App construction

CZoomscrolldemo2App::CZoomscrolldemo2App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CZoomscrolldemo2App object

CZoomscrolldemo2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CZoomscrolldemo2App initialization

BOOL CZoomscrolldemo2App::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

// The Enable3dControls is required in VC++ 6 (in fact, it is automatically 
// generated by the VC++ 6 MFC Wizard), but will cause a deprecated warning 
// in VC++ 7. So we disable the warning message for a clean compilation.
#pragma warning(push)
#pragma warning(disable:4996)
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#pragma warning(pop)

	CZoomscrolldemo2Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
