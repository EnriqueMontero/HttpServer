#pragma once

class CHttpServer : 
	public CHttpSrvImpl<CHttpServer>,
	public CThreadImpl<CHttpServer>
{
public:
	CHttpServer(HWND hwnd)
		:CHttpSrvImpl<CHttpServer>()
		,CThreadImpl<CHttpServer>()
		,m_hwnd(hwnd)
	{
	}

	BOOL Initialize()
	{
		// Perform initialization.
		BOOL rc = InitializeHttp() && InitializeThread(this,CREATE_SUSPENDED);
		if( rc )
		{
			HTTP_URL_GROUP_ID gID=0;
			if( CreateUrlGroup(gID) )
			{
				AddUrlToUrlGroup(gID,L"http://+:80/");
			}
		}
		return rc;
	}

	DWORD Run()
	{
		ReceiveRequests();

		// Do something useful...
		return 0;
	}

	USHORT ReponseEntity(PHTTP_REQUEST pRequest,char **pEntityString)
	{
		const char * response ="Vaya Mierda";
		ULONG responseLength = strlen(response);
		*pEntityString = (char *) ALLOC_MEM( responseLength+1 );

		memcpy(*pEntityString,response,responseLength+1);
		PostMessage(m_hwnd,WM_USER+10,NULL,3);

		return 0;
	}

protected:
	HWND m_hwnd;
};