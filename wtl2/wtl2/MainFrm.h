// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CWtl2View m_view;

	CCommandBarCtrl m_CmdBar;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnStart)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnStop)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnClear)
		COMMAND_ID_HANDLER(ID_FILE_PRINT, OnURLSetting)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW,OnCustomDraw)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
		m_TB.Attach(hWndToolBar);
		m_Menu.Attach( m_CmdBar.m_hMenu );

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		CreateSimpleStatusBar();

		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE );
		
		RECT rc;
		GetClientRect(&rc);
		m_view.SetExtendedListViewStyle(m_view.GetExtendedListViewStyle()|LVS_EX_GRIDLINES);
		m_view.FillPlease(rc);

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		// Http Server create
		bServerInitialize=false;
		bThreadSuspended=true;
		m_pServer = new CHttpServer(m_view.m_hWnd);

		// menu enabling items
		m_Menu.EnableMenuItem(ID_FILE_NEW,MF_DISABLED);
		m_Menu.EnableMenuItem(ID_FILE_OPEN,MF_DISABLED);

		// Toolbar actions
		m_TB.EnableButton(ID_FILE_NEW,FALSE);
		m_TB.EnableButton(ID_FILE_OPEN,FALSE);

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		bHandled = FALSE;

		delete m_pServer;
		return 1;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document
		if( !bServerInitialize )
		{
			bServerInitialize = m_pServer->Initialize();
			if( !bServerInitialize )	// error starting server
				m_view.PostMessageW(WM_CODE_INFORMATION,NULL,5);
		}

		if( bServerInitialize )
		{
			if(	bThreadSuspended )
			{
				DWORD error = m_pServer->Resume();
				if( error==-1 )
				{
					error=GetLastError();
					m_view.PostMessage(WM_CODE_INFORMATION,error,7);
				}
				else
				{
					bThreadSuspended = FALSE;
					m_view.PostMessage(WM_CODE_INFORMATION,NULL,1);
				}
			}
		}

		return 0L;
	}

	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if( bServerInitialize && !bThreadSuspended )
		{
			m_pServer->Suspend();
			bThreadSuspended = TRUE;
			m_view.PostMessage(WM_CODE_INFORMATION,NULL,2);
		}

		return 0L;
	}

	LRESULT OnClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return m_view.DeleteAllItems();
	}

	LRESULT OnURLSetting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CURLOptions url(_T("URLs Configuration:"),0,m_hWndClient);
		if( IDOK==url.DoModal(*this) )
		{
			if( m_pServer->InitializeHttp() )
			{
				INT iCount=0;
				if( url.HasPages() )
				{
					iCount = SaveGroups(url);
				}
				else
				{
					HTTP_URL_GROUP_ID gID=0;
					wstring wURL = url.GetURLbyDefault();

					m_pServer->CreateUrlGroup(gID);
					if( m_pServer->AddUrlToUrlGroup(gID,wURL.c_str()) )
					{
						m_view.SendMessage(WM_CODE_INFORMATION,(WPARAM)&wURL,8);
						iCount = 1;
					}
				}

				if( iCount )
				{
					// menu enabling items
					m_Menu.EnableMenuItem(ID_FILE_NEW,MF_ENABLED);
					m_Menu.EnableMenuItem(ID_FILE_OPEN,MF_ENABLED);

					// toolbar enabling buttons
					m_TB.EnableButton(ID_FILE_NEW,TRUE);
					m_TB.EnableButton(ID_FILE_OPEN,TRUE);
				}
			}
		}

		return 0L;
	}

	INT SaveGroups(CURLOptions& url) 
	{
		INT count=0;
		wstring wURL;
		size_t pag = 0;
		CGroupPage * pPage = NULL;
		HTTP_URL_GROUP_ID gID=0;

		while( (pPage = url[pag++])!=NULL )
		{
			vector<wstring>& vURLs = pPage->GetURLs();
			if( vURLs.size()>0 )
			{
				m_pServer->CreateUrlGroup(gID);

				for(size_t i=0;i<vURLs.size();i++)
				{
					wURL = vURLs[i];
					if( m_pServer->AddUrlToUrlGroup(gID,wURL.c_str()) )
					{
						m_view.SendMessage(WM_CODE_INFORMATION,(WPARAM)&wURL,8);
						count ++; 
					}
				}
			}
		}

		return count;
	}
		
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

private:
	CHttpServer *	m_pServer;
	BOOL			bServerInitialize;
	BOOL			bThreadSuspended;
	CToolBarCtrl	m_TB;
	CMenu			m_Menu;

	LRESULT OnCustomDraw(int /*idCtrl*/, LPNMHDR lParam, BOOL& /*bHandled*/)
	{
		if( lParam->hwndFrom==m_hWndClient )
			return m_view.OnCustomDraw((LPNMLVCUSTOMDRAW)lParam);
		return 0;
	}
};
