#include "stdafx.h"
#include "URLOptions.h"

#include <string>

#define MAX_URL_LENGTH		300
#define SPIN_BOTTON			0
#define SPIN_TOP			4

CURLOptions::CURLOptions(_U_STRINGorID title/* = (LPCTSTR) NULL*/, UINT uStartPage/* = 0*/, HWND hWndParent/* = NULL*/)
	:CPropertySheetImpl<CURLOptions>(title, uStartPage, hWndParent)
{
	m_psh.dwFlags |= (PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP);
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

LRESULT CURLOptions::OnGroupsChange(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	BOOL bInsert = (BOOL)wParam;
	CGroupPage *pPage = NULL;
	if( bInsert )
	{
		int n = m_groups.size() + 1;
		pPage = new CGroupPage(n);
		if( AddPage(*pPage) )
		{
			m_groups.push_back(pPage);
		}
		else 
		{
			delete pPage;
		}
	}
	else
	{
		int n = m_groups.size();
		if( n>0 && RemovePage(n) )
		{
			pPage = m_groups.at(n-1);
			m_groups.pop_back();
			if( pPage!=NULL )
			{
				delete pPage;
			}
		}
	}

	return 0;
}

LRESULT CURLOptions::OnRemoveAllPages(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CGroupPage *pPage = NULL;
	int n = m_groups.size();
	while( n>0 && RemovePage(n) )
	{
		pPage = m_groups.at(n-1);
		m_groups.pop_back();
		if( pPage!=NULL )
		{
			delete pPage;
		}
		n--;		
	}

	return 0;
}

BOOL CURLOptions::HasPages() const
{
	return m_groups.size()>0;
}

wstring CURLOptions::GetURLbyDefault() const
{
	return page.GetDefaultURL();
}

CGroupPage * CURLOptions::operator [] (const size_t index) const
{
	if( index>=0 && index<m_groups.size() ) 
		return m_groups.at(index);
	return NULL;
}

/*
 *	CSettingsPage
 */

CSettingsPage::CSettingsPage()
	:m_defaultURL(L"http://+:80/In/")
	,m_title(L"Unique/Multiple URL(s)")
{
	SetTitle( m_title.c_str() );
}

CSettingsPage::~CSettingsPage()
{
}

LRESULT CSettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_szURL.Attach( GetDlgItem(IDC_STATIC1) );
	m_edit.Attach( GetDlgItem(IDC_EDIT1) );
	m_check.Attach( GetDlgItem(IDC_CHECK1) );
	m_szGroups.Attach( GetDlgItem(IDC_STATIC2) );
	m_spin.Attach( GetDlgItem(IDC_SPIN1) );
	m_buddy.Attach( GetDlgItem(IDC_EDIT2) );

	m_edit.SetLimitText(MAX_URL_LENGTH);
	m_edit.SetWindowText( m_defaultURL.c_str() );
	m_spin.SetRange(SPIN_BOTTON,SPIN_TOP);
	m_spin.SetBuddy(m_buddy);

	return 0L;
}

LRESULT CSettingsPage::OnBnClickedCheck1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	bool bEnable = m_check.GetCheck()==BST_CHECKED ? TRUE : FALSE;
	m_szURL.EnableWindow(!bEnable);
	m_edit.EnableWindow(!bEnable);
	m_szGroups.EnableWindow(bEnable);
	m_buddy.EnableWindow(bEnable);
	m_spin.EnableWindow(bEnable);

	if( !bEnable )
	{
		GetPropertySheet().SendMessage(WM_ADD_REMOVE_ALL_PAGES);
		m_buddy.SetWindowText(L"");
	}

	return 0;
}

LRESULT CSettingsPage::OnDeltaposSpin1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	int pos = pNMUpDown->iPos + pNMUpDown->iDelta;
	if( pos>=SPIN_BOTTON && pos<=SPIN_TOP )
	{
		GetPropertySheet().SendMessage(WM_ADD_REMOVE_PAGE,pNMUpDown->iDelta>0);
	}
	return 0;
}

wstring CSettingsPage::GetDefaultURL() const
{
	return m_defaultURL;
}

BOOL CSettingsPage::OnApply()
{
	wchar_t cad[MAX_URL_LENGTH+1];
	m_edit.GetWindowText(cad,MAX_URL_LENGTH);
	m_defaultURL = cad;
	return TRUE;
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

LRESULT CGroupPage::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	wchar_t cad[MAX_URL_LENGTH+1];
	m_edit.GetWindowText(cad,MAX_URL_LENGTH);
	if( wcslen(cad) )
	{
		m_lb.SetCurSel( m_lb.AddString(cad) );
		m_edit.SetWindowText(L"");
	}

	return 0;
}

LRESULT CGroupPage::OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	int sel = m_lb.GetCurSel();
	if( sel!=LB_ERR )
	{
		m_lb.DeleteString(sel);
	}

	return 0;
}
 
// Property page notification handlers
BOOL CGroupPage::OnApply()
{
	m_vURLs.clear();
	wchar_t cad[MAX_URL_LENGTH+1];
		
	int nItems = m_lb.GetCount();
	for(int i=0;i<nItems;i++)
	{
		if( m_lb.GetText(i,cad)!=LB_ERR && wcslen(cad) ) 
		{
			m_vURLs.push_back(cad);
		}
	}

	return TRUE;
}
