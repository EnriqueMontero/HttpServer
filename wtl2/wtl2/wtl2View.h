// wtl2View.h : interface of the CWtl2View class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define WM_CODE_INFORMATION		(WM_USER+10)

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
		MESSAGE_HANDLER(WM_CODE_INFORMATION,OnInformation);
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInformation(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
		case 4:
			ws1 = L"Open File Error";
			ws2 = to_wstring (wParam);
			break;
		case 5:
			ws1 = L"Error starting server";
			break;
		case 6:
			ws1 = L"Post Processed";
			break;
		case 7:
			ws1 = L"Resume Thread error";
			ws2 = to_wstring (wParam);
			break;
		case 8:
			ws1 = *((wstring *)wParam);
			ws2 = L"URL Added";
			break;
		}

		if( ws1.length() )
		{
			int nItem = GetItemCount();
			nItem = InsertItem(nItem,ws1.c_str());
			SetItemData(nItem,index);
			if( ws2.length() )
				SetItemText(nItem,1,ws2.c_str());
			if( ws3.length() )
				SetItemText(nItem,2,ws3.c_str());
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

LRESULT OnCustomDraw(LPNMLVCUSTOMDRAW lvcd)
	{
		LRESULT rc = CDRF_DODEFAULT;
		switch( lvcd->nmcd.dwDrawStage )
		{
		case CDDS_PREPAINT:
			rc = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:
			rc = CDRF_NOTIFYSUBITEMDRAW;
			break;
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
			ULONG itemType = (ULONG)lvcd->nmcd.lItemlParam;
			COLORREF bk,fg=RGB(0,0,0);
			switch( itemType )
			{
			case 1:
				bk=RGB(0,255,0);
				break;
			case 2:
				bk=RGB(0,64,0);
				fg=RGB(255,255,255);
				break;
			case 4:
			case 5:
			case 7:
				bk=RGB(255,0,0);
				fg=RGB(255,255,255);
				break;
			case 3:
			case 6:
				bk=bk=RGB(192,255,192);
				break;
			case 8:
				bk=RGB(255,219,0);
				break;
			}
			lvcd->clrText=fg;
			lvcd->clrTextBk=bk;
			rc = CDRF_DODEFAULT;
			break;
		}

		return rc;
	}
};

