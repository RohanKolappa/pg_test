// linkcreator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "linkcreator.h"

#include "Shortcut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

void createShortCut();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		createShortCut();
	}

	return nRetCode;
}

void createShortCut()
{

	CShortcut *m_pShortcut = new CShortcut;

	//if(m_pShortcut->CreateShortCut("C:\\IP Video Systems\\IPVSAuthTool\\Authoring_Home.html",
	//	"C:\\Documents and Settings\\anoop\\Desktop\\tt",
	//	CSIDL_DESKTOP,
	//	"Desktop ShellLink",
	//	"",
	//	0) == TRUE)


	if(m_pShortcut->CreateShortCut("C:\\IP Video Systems\\IPVSAuthTool\\Authoring_Home.html",
									"Authoring_Home.html",
									CSIDL_COMMON_DESKTOPDIRECTORY,
									"Home page for IP Video Systems Authoring Tool",
									"",
									0) == TRUE)
		cout << " ShortCut Created on the Desktop " << endl;
	else
		cout << " Failed to create shortcut on Desktop " << endl;


	delete m_pShortcut;

}
