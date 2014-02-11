// wtl2View.h : interface of the CWtl2View class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CWtl2View : public CWindowImpl<CWtl2View, CListViewCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CWtl2View)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_USER+10,OnInformation);
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInformation(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		ULONG index = (ULONG)lParam;
		wstring ws1(L""), ws2(L""), ws3(L"");
		
		switch( index )
		{
		case 1:
			ws1 = L"Served Started";
			break;
		case 2:
			ws1 = L"Served Stoped";
			break;
		case 3:
			ws1 = L"Get Processed";
			break;
		}

		if( ws1.length() )
		{
			int nItem = GetItemCount();
			InsertItem(nItem,ws1.c_str());
		}
		return 0L;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int wUnit = (LOWORD(lParam))/6;
		SetColumnWidth(0,wUnit*4);
		SetColumnWidth(1,wUnit);
		SetColumnWidth(2,wUnit);

		return 1l;
	}

	LRESULT FillPlease(RECT& rc)
	{
		int wUnit = (rc.right-rc.left)/6;

		InsertColumn(1,L"Primera",LVCFMT_CENTER,wUnit*4);
		InsertColumn(2,L"Segunda",LVCFMT_CENTER,wUnit);
		InsertColumn(3,L"Tercera",LVCFMT_CENTER,wUnit);

		return 0L;
	}
};

