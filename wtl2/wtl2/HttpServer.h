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
		,m_heap(NULL)
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
				rc = AddUrlToUrlGroup(gID,L"http://+:80/test/");
			}
		}
		return rc;
	}

	DWORD Run()
	{
		// Create own thread heap
		m_heap = HeapCreate(HEAP_NO_SERIALIZE,512*1024,0);

		if( m_heap )
		{
			// Request processing
			ReceiveRequests();

			// delete heap
			HeapDestroy(m_heap);
		}

		return 0;
	}

	USHORT ReponseEntity(PHTTP_REQUEST pRequest,char **pEntityString)
	{
		const char * response ="Vaya Mierda";
		ULONG responseLength = strlen(response);
		*pEntityString = (char *) HeapAlloc(m_heap,0,responseLength+1);

		memcpy(*pEntityString,response,responseLength+1);
		PostMessage(m_hwnd,WM_USER+10,NULL,3);

		return 0;
	}

	USHORT ReponseEntity(CEntity& aEntity,char **pEntityString)
	{
		PostMessage(m_hwnd,WM_USER+10,NULL,6);
		return 0;
	}

	HANDLE GetHeapHandle() const
	{
		return m_heap;
	}

protected:
	HWND m_hwnd;
	HANDLE m_heap;
};