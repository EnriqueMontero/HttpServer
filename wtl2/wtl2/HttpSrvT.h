#pragma once

#include <vector>
using namespace std;

// Macros
#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )    \
    {                                                       \
        RtlZeroMemory( (resp), sizeof(*(resp)) );           \
        (resp)->StatusCode = (status);                      \
        (resp)->pReason = (reason);                         \
        (resp)->ReasonLength = (USHORT) strlen(reason);     \
	}

#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)												\
    {                                                                                               \
        (Response).Headers.KnownHeaders[(HeaderId)].pRawValue = (RawValue);							\
        (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength = (USHORT) strlen(RawValue);     \
    }

// Class
template<class T>
class CHttpSrvT
{
public:
	CHttpSrvT()
		:m_session(NULL)
		,m_requestQueue(NULL)
	{
		m_groups.clear();
	}
	
	bool InitializeHttp() 
	{
		HTTPAPI_VERSION HttpApiVersion = HTTPAPI_VERSION_2;

		// Initialize HTTP Server APIs
		ULONG rc = HttpInitialize(HttpApiVersion,HTTP_INITIALIZE_SERVER,NULL);
		
		// Create Request Queue
		if( rc==NO_ERROR )
			rc = HttpCreateRequestQueue(HttpApiVersion,NULL,NULL,0,&m_requestQueue);

		// Create server session
		if( rc==NO_ERROR )
			rc = HttpCreateServerSession(HttpApiVersion,&m_session,0);

		return rc==NO_ERROR;
	}

	bool CreateUrlGroup(HTTP_URL_GROUP_ID& gID)
	{
	
		// Create a group
		ULONG rc = HttpCreateUrlGroup(m_session,&gID,0);
		
		if( rc==NO_ERROR )
		{
			HTTP_BINDING_INFO bInfo;
			bInfo.Flags.Present=true;
			bInfo.RequestQueueHandle=m_requestQueue;
			
			// bind group to the queue
			rc = HttpSetUrlGroupProperty(gID,HttpServerBindingProperty,&bInfo,sizeof(HTTP_BINDING_INFO));

			// keep it in a vector: m_groups or close if binding error
			if( rc==NO_ERROR )
				m_groups.push_back(gID);
			else 
				HttpCloseUrlGroup(gID);
		}
		
		return rc==NO_ERROR;
	}

	bool AddUrlToUrlGroup(const HTTP_URL_GROUP_ID gID,const wchar_t *url,const HTTP_URL_CONTEXT urlC=0)
	{
		ULONG rc =  HttpAddUrlToUrlGroup(gID,url,urlC,0);
		assert(rc==NO_ERROR);
		return rc==NO_ERROR;
	}


protected:
	virtual ~CHttpSrvT()
	{
		// remove url from groups
		vector<HTTP_URL_GROUP_ID>::iterator it;
		for(it=m_groups.begin();it<m_groups.end();it++)
		{
			HttpRemoveUrlFromUrlGroup(*it,NULL,HTTP_URL_FLAG_REMOVE_ALL);
			HttpCloseUrlGroup(*it);
		}

		m_groups.clear();
		
		HttpCloseServerSession(m_session);
		HttpCloseRequestQueue(m_requestQueue);
		HttpTerminate(HTTP_INITIALIZE_SERVER,NULL);
	}

	DWORD ReceiveRequests()
	{
		// this method must be run in a worker thread
		T *				   pT = static_cast<T*>(this);
		ULONG              result;
		HTTP_REQUEST_ID    requestId;
		DWORD              bytesRead;
		PHTTP_REQUEST      pRequest;
		PCHAR              pRequestBuffer;
		ULONG              requestBufferLength;
		HANDLE			   heap = pT->GetHeapHandle();
		
		requestBufferLength = sizeof(HTTP_REQUEST) + 1024*8;
		pRequestBuffer      = (PCHAR) HeapAlloc(heap, 0, requestBufferLength);

		if( pRequestBuffer==NULL )
		{
			return ERROR_NOT_ENOUGH_MEMORY;
		}

		pRequest = (PHTTP_REQUEST)pRequestBuffer;

		// Null value means wait for a new request
		HTTP_SET_NULL_ID( &requestId );

		for(;;)
		{
			RtlZeroMemory(pRequest, requestBufferLength);

			result = HttpReceiveHttpRequest(m_requestQueue,      // Req Queue
											requestId,           // Req ID
											0,                   // Flags -> 0 = no entity in request
											pRequest,            // HTTP request buffer
											requestBufferLength, // req buffer length
											&bytesRead,          // bytes received
											NULL                 // LPOVERLAPPED
											);
			if( result==NO_ERROR )						// no error
			{
				result = ProcessResponse(pRequest);		// Response	
				if( result!=NO_ERROR )					// some kind of error in ProcessResponse
					break;

				HTTP_SET_NULL_ID( &requestId );			// Reset the Request ID to handle the next request.
			}
			else if( result==ERROR_MORE_DATA )			// small buffer
			{
				requestId = pRequest->RequestId;		// Keep requestId
				requestBufferLength = bytesRead;		// Get new length
				HeapFree(heap, 0, pRequestBuffer);		// Free old buffer

				pRequestBuffer = (PCHAR) HeapAlloc(heap, 0, requestBufferLength);
				if( pRequestBuffer==NULL )				// Not enough memory
				{
					result = ERROR_NOT_ENOUGH_MEMORY;
					break;
				}

				pRequest = (PHTTP_REQUEST)pRequestBuffer;
			}
			else if( result==ERROR_CONNECTION_INVALID && !HTTP_IS_NULL_ID(&requestId) )
			{
				/* 
				The TCP connection was corrupted by the peer when attempting to handle a request with more buffer. 
				Continue to the next request.
				*/

				HTTP_SET_NULL_ID( &requestId );
			}
			else
			{
				break;
			}
		}

	   if( pRequestBuffer )
		   HeapFree(heap, 0, pRequestBuffer);
	   
	   return result;
	}

	DWORD ProcessResponse(PHTTP_REQUEST pRequest)
	{
		DWORD rc = NO_ERROR;

		switch( pRequest->Verb )
		{
		case HttpVerbGET:
			rc = ResponseVerbGET(pRequest);
			break;
		case HttpVerbPOST:
			rc = ResponseVerbPOST(pRequest);
			break;
		case HttpVerbPUT:
			break;
		case HttpVerbDELETE:
			break;
		}

		return rc;
	}

	DWORD ResponseVerbGET(PHTTP_REQUEST pRequest)
	{
		T *				pT = static_cast<T*>(this);
		HTTP_RESPONSE   response;
		HTTP_DATA_CHUNK dataChunk;
		DWORD           bytesSent;
		char *			pEntityString = NULL;
		USHORT			status = pT ? pT->ReponseEntity(pRequest,&pEntityString) :500;
		string			pReason = getReason(status);
		HANDLE			heap = pT->GetHeapHandle();

		INITIALIZE_HTTP_RESPONSE(&response,status,pReason.c_str());
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");
   
		if( pEntityString )
		{
			dataChunk.DataChunkType           = HttpDataChunkFromMemory;
			dataChunk.FromMemory.pBuffer      = pEntityString;
			dataChunk.FromMemory.BufferLength = strlen(pEntityString);

			response.EntityChunkCount         = 1;
			response.pEntityChunks            = &dataChunk;
		}

		DWORD rc = HttpSendHttpResponse(m_requestQueue,      // ReqQueueHandle
										pRequest->RequestId, // Request ID
										0,                   // Flags
										&response,           // HTTP response
										NULL,                // pReserved1
										&bytesSent,          // bytes sent  (OPTIONAL)
										NULL,                // pReserved2  (must be NULL)
										0,                   // Reserved3   (must be 0)
										NULL,                // LPOVERLAPPED(OPTIONAL)
										NULL                 // pReserved4  (must be NULL)
										); 

		if( pEntityString )
			HeapFree(heap,0,pEntityString);

		return rc;
	}

	DWORD ResponseVerbPOST(PHTTP_REQUEST pRequest)
	{
		T *				pT = static_cast<T*>(this);
		char *			pEntityString = NULL;
		HTTP_RESPONSE   response;
		DWORD           bytesSent;

		INITIALIZE_HTTP_RESPONSE(&response, 200, "OK");

		// Get entity Body if exists
		if( pRequest->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS )
		{
			CEntity entity(CEntity::eFile);
			
			PVOID pEntityBuffer = entity.GetBuffer();
			ULONG EntityBufferLength = entity.GetBufferLength();
			ULONG BytesRead = 0, result = 0L;
			BOOL bMoreData = TRUE;
			
			do
			{
				// Read the entity chunk from the request.
				BytesRead = 0; 
				result = HttpReceiveRequestEntityBody(m_requestQueue,
													  pRequest->RequestId,
													  0,
													  pEntityBuffer,
													  EntityBufferLength,
													  &BytesRead,
													  NULL 
													  );
				switch( result )
				{
				case NO_ERROR:
					entity.Add(BytesRead);
					bMoreData = TRUE;
                    break;
                case ERROR_HANDLE_EOF:
					entity.Add(BytesRead);
					bMoreData = FALSE;
					pT->ReponseEntity(entity,&pEntityString);
					if( pEntityString )
					{
						HTTP_DATA_CHUNK dataChunk;

						dataChunk.DataChunkType           = HttpDataChunkFromMemory;
						dataChunk.FromMemory.pBuffer      = pEntityString;
						dataChunk.FromMemory.BufferLength = strlen(pEntityString);
						response.EntityChunkCount         = 1;
						response.pEntityChunks            = &dataChunk;
					}
					break;
				default:
					bMoreData = FALSE;
					break;
				}

			} while( bMoreData );
		}

		return HttpSendHttpResponse(m_requestQueue,      // ReqQueueHandle
									pRequest->RequestId, // Request ID
									0,                   // Flags
									&response,           // HTTP response
									NULL,                // pReserved1
									&bytesSent,          // bytes sent  (OPTIONAL)
									NULL,                // pReserved2  (must be NULL)
									0,                   // Reserved3   (must be 0)
									NULL,                // LPOVERLAPPED(OPTIONAL)
									NULL                 // pReserved4  (must be NULL)
									); 
	}
	
private:
	string getReason(USHORT status) const
	{
		string rc="¿?";
		switch( status )
		{
		case 200:
			rc = "OK";
			break;
		case 400:
			rc = "Bad Request";
			break;
		case 401:
			rc = "Unauthorized";
			break;
		case 404:
			rc = "Not Found";
			break;
		case 500:
			rc = "Internal Server Error";
			break;
		case 501:
			rc = "Not Implemented";
			break;
		case 503:
			rc = "Service Unavailable";
			break;
		}

		return rc;
	}
	
protected:
	HTTP_SERVER_SESSION_ID m_session;
	vector<HTTP_URL_GROUP_ID> m_groups;
	HANDLE m_requestQueue;
};

template<class T>
class CHttpSrvImpl : public CHttpSrvT<T>
{
public:
	CHttpSrvImpl()
		:CHttpSrvT<T>()
	{
	}
};