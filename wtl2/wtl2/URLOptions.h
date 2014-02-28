#pragma once

#include "atldlgs.h"
#include "atlctrls.h"
#include "resource.h"

#include <vector>
using namespace std;


class CGroupPage :
	public CPropertyPageImpl<CGroupPage>
{
public:
	enum { IDD = IDD_DIALOG1 };
	
	CGroupPage(HTTP_URL_GROUP_ID id);
	~CGroupPage();

	// Maps
	BEGIN_MSG_MAP(CGroup1Page)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnButton)
		CHAIN_MSG_MAP(CPropertyPageImpl<CGroupPage>)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnButton(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnAdd();
	void OnRemove();
 
	// Property page notification handlers
	int OnApply();

private:
	CListBox		m_lb;
	CEdit			m_edit;
	CButton			m_plus, 
					m_minus;

	HTTP_URL_GROUP_ID	m_gID;
	wstring				m_title;
	vector<wstring>		m_vURLs;
};

class CURLOptions :
	public CPropertySheetImpl<CURLOptions>
{
public:
	CURLOptions(_U_STRINGorID title = (LPCTSTR) NULL, UINT uStartPage = 0, HWND hWndParent = NULL);
	~CURLOptions(void);

	// Maps
	BEGIN_MSG_MAP(COptionsSheet)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_COMMAND, OnButton)
		CHAIN_MSG_MAP(CPropertySheetImpl<CURLOptions>)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnButton(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		
private:
	CGroupPage page;
};
