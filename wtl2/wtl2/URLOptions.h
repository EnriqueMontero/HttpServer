#pragma once

#include "atldlgs.h"
#include "atlctrls.h"
#include "resource.h"

#include <vector>
using namespace std;

#define WM_ADD_REMOVE_PAGE			(WM_USER+50)
#define WM_ADD_REMOVE_ALL_PAGES		(WM_USER+51)

class CGroupPage :
	public CPropertyPageImpl<CGroupPage>
{
public:
	enum { IDD = IDD_DIALOG1 };
	
	CGroupPage(HTTP_URL_GROUP_ID id,HWND hwnd);
	~CGroupPage();

	// Maps
	BEGIN_MSG_MAP(CGroupPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButton1)
		COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedButton2)
		COMMAND_HANDLER(IDC_BUTTON3, BN_CLICKED, OnLoadFromFile)
		CHAIN_MSG_MAP(CPropertyPageImpl<CGroupPage>)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLoadFromFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	// Property page notification handlers
	BOOL OnApply();

	// Helper
	vector<wstring>& GetURLs()
	{
		return m_vURLs;
	}
	void FileToList(char *pBuffer);

private:
	CListBox			m_lb;
	CEdit				m_edit;
	CButton				m_plus, 
						m_minus;

	HTTP_URL_GROUP_ID	m_gID;
	wstring				m_title;
	vector<wstring>		m_vURLs;
	HWND				m_hView;
};

class CSettingsPage : 
	public CPropertyPageImpl<CSettingsPage>

{
public:
	enum { IDD = IDD_DIALOG2 };

	CSettingsPage();
	~CSettingsPage();

	// Maps
	BEGIN_MSG_MAP(CSettingsPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_SPIN1, UDN_DELTAPOS, OnDeltaposSpin1)
		COMMAND_HANDLER(IDC_CHECK1, BN_CLICKED, OnBnClickedCheck1)
		CHAIN_MSG_MAP(CPropertyPageImpl<CSettingsPage>)
	END_MSG_MAP()

		// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDeltaposSpin1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheck1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	// Property page notification handlers
	BOOL OnApply();

	// Helper
	wstring GetDefaultURL() const;

private:
	wstring			m_title;

	CStatic			m_szURL;
	CEdit			m_edit;
	CButton			m_check;
	CStatic			m_szGroups;
	CUpDownCtrl		m_spin;
	CEdit			m_buddy;
	wstring			m_defaultURL;
};

class CURLOptions :
	public CPropertySheetImpl<CURLOptions>
{
public:
	CURLOptions(_U_STRINGorID title = (LPCTSTR) NULL, UINT uStartPage = 0, HWND hWndParent = NULL);
	~CURLOptions(void);

	// Maps
	BEGIN_MSG_MAP(CURLOptions)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_ADD_REMOVE_PAGE,OnGroupsChange)
		MESSAGE_HANDLER(WM_ADD_REMOVE_ALL_PAGES,OnRemoveAllPages)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGroupsChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveAllPages(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	// helper
	BOOL HasPages() const;
	wstring GetURLbyDefault() const;
	CGroupPage * operator [](const size_t index) const;
		
private:
	CSettingsPage page;
	vector <CGroupPage *> m_groups;
};
