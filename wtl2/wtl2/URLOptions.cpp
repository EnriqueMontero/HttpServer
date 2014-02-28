#include "stdafx.h"
#include "URLOptions.h"

#define MAX_URL_LENGTH		300

CURLOptions::CURLOptions(_U_STRINGorID title/* = (LPCTSTR) NULL*/, UINT uStartPage/* = 0*/, HWND hWndParent/* = NULL*/)
	:CPropertySheetImpl<CURLOptions>(title, uStartPage, hWndParent)
	,page(1)
{
	m_psh.dwFlags |= PSH_NOCONTEXTHELP;
	AddPage(page);
}

CURLOptions::~CURLOptions(void)
{
}

LRESULT CURLOptions::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( wParam==TRUE )
		CenterWindow(m_psh.hwndParent);
	return 0L;
}

LRESULT CURLOptions::OnButton(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SHORT idNotification = HIWORD(wParam);

	if( idNotification==BN_CLICKED )
	{
		SHORT idControl = LOWORD(wParam);
		if( idControl==IDOK )
		{
			Apply();
			EndDialog(*this,IDOK);
		}
	}

	return 0L;
}

/*
 *	CGroupPage
 */

CGroupPage::CGroupPage(HTTP_URL_GROUP_ID id)
	:m_gID(id)
	,m_title(L"")
{
	wchar_t cad[100];
	swprintf_s(cad,100,L"URLs for group %d",id);
	m_title=cad;
	
	SetTitle( m_title.c_str() );
}
	
CGroupPage::~CGroupPage()
{
}

// Message handlers
LRESULT CGroupPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_lb.Attach( GetDlgItem(IDC_LIST3) );
	m_edit.Attach( GetDlgItem(IDC_EDIT1) );
	m_plus.Attach( GetDlgItem(IDC_BUTTON1) );
	m_minus.Attach( GetDlgItem(IDC_BUTTON2) );

	m_edit.SetLimitText(MAX_URL_LENGTH);
	return 0L;
}

LRESULT CGroupPage::OnButton(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SHORT idNotification = HIWORD(wParam);

	if( idNotification==BN_CLICKED )
	{
		SHORT idControl = LOWORD(wParam);

		switch( idControl )
		{
		case IDC_BUTTON1:
			OnAdd();
			break;
		case IDC_BUTTON2:
			OnRemove();
			break;
		}
	}

	return 0L;
}

void CGroupPage::OnAdd()
{
	wchar_t cad[MAX_URL_LENGTH+1];
	m_edit.GetWindowText(cad,MAX_URL_LENGTH+1);
	if( wcslen(cad) )
	{
		m_lb.SetCurSel( m_lb.AddString(cad) );
		m_edit.SetWindowText(L"");
		SetModified(TRUE);
	}
}

void CGroupPage::OnRemove()
{
	int sel = m_lb.GetCurSel();
	if( sel!=LB_ERR )
	{
		m_lb.DeleteString(sel);
		SetModified(TRUE);
	}
}
 
// Property page notification handlers
int CGroupPage::OnApply()
{
	m_vURLs.clear();
	wchar_t cad[MAX_URL_LENGTH+1];
		
	int nItems = m_lb.GetCount();
	for(int i=0;i<nItems;i++)
	{
		if( m_lb.GetText(i,cad)!=LB_ERR )
		{
			if( wcslen(cad) )
				m_vURLs.push_back(cad);
		}
	}

	return 0L;
}
