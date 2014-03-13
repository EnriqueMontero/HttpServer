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
	~CHttpServer()
	{
	}

	BOOL Initialize()
	{
		return  InitializeThread(this,CREATE_SUSPENDED);
	}

	DWORD Run()
	{
		// Thread function
		// Perform initialization.
		HANDLE h = HeapCreate(HEAP_NO_SERIALIZE,512*1024,0);  
		if( h )
		{
			 m_heap=h;   // El destructor de clase se ejecuta antes de finalizar el thread ... m_heap queda fuera de escope.

			// Request processing
			ReceiveRequests();
			
			// Destroy heap
			HeapDestroy(h);
		}

		return 0;
	}

	USHORT ReponseEntity(PHTTP_REQUEST pRequest,char **pEntityString)
	{
		const char * response ="Vaya Mierda";
		ULONG responseLength = strlen(response);
		*pEntityString = (char *) HeapAlloc(m_heap,0,responseLength+1);

		memcpy(*pEntityString,response,responseLength+1);
		PostMessage(m_hwnd,WM_CODE_INFORMATION,NULL,3);

		return 0;
	}

	USHORT ReponseEntity(CEntity& aEntity,char **pEntityString)
	{
		PostMessage(m_hwnd,WM_CODE_INFORMATION,NULL,6);
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