// ProjectManager.h : Declaration of the CProjectManager

#ifndef __PROJECTMANAGER_H_
#define __PROJECTMANAGER_H_

#include <ObjSafe.h>
#include "resource.h"       // main symbols

#include "AxAuthor_Defines.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectManager
class ATL_NO_VTABLE CProjectManager : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CProjectManager, &CLSID_ProjectManager>,
	public IDispatchImpl<IProject, &IID_IProject, &LIBID_IPVSAXAUTHORLib>
{
public:
	CProjectManager();
	~CProjectManager();

DECLARE_REGISTRY_RESOURCEID(IDR_PROJECTMANAGER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CProjectManager)
	COM_INTERFACE_ENTRY(IProject)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CProjectManager)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()

// IProject
public:
	STDMETHOD(BrowseForFolder)(/*[out, retval]*/ BSTR *selectedFolder);	
	STDMETHOD(CopyStream)(INT mediaType, BSTR srcBlobDir, BSTR dstBlobDir, INT startMin, INT endMin, BOOL allMins);
	STDMETHOD(DeleteDir)(BSTR dirName);
	STDMETHOD(get_ProjectDir)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProjectDir)(/*[in]*/ BSTR newVal);

private:
	CComBSTR	m_bstrProjectDir;
};

#endif //__PROJECTMANAGER_H_
