#include "stdafx.h"
#include "Entity.h"

CEntity::CEntity(EntityMode mod/*=eFile*/)
	:m_mode(mod)
	,m_readLength(4*1024)
	,m_pBuffer(NULL)
	,m_pBody(NULL)
	,m_hTempFile(INVALID_HANDLE_VALUE)
	,m_fileName(L"")
	,m_bodySize(0L)
{
	m_pBuffer = HeapAlloc(GetProcessHeap(), 0, m_readLength);

	if( m_mode==eFile )
		CreateEntityFile();
	else if( m_mode==eMemory )
		CreateEntityMemory();
}

CEntity::~CEntity(void)
{
	if( m_pBody )
		HeapFree(GetProcessHeap(), 0, m_pBody);
	
	if( m_hTempFile!=INVALID_HANDLE_VALUE )
	{
        CloseHandle(m_hTempFile);
		if( m_fileName.length() )
			DeleteFile(m_fileName.c_str());
	}

	HeapFree(GetProcessHeap(), 0, m_pBuffer);
}

BOOL CEntity::CreateEntityFile()
{
	m_fileName = GetFileName();
	if( m_fileName.length() )
	{
		m_hTempFile = CreateFile(m_fileName.c_str(),
								 GENERIC_READ | GENERIC_WRITE, 
								 0,								// Do not share.
								 NULL,							// No security descriptor.
								 CREATE_ALWAYS,					// Overrwrite existing.
								 FILE_ATTRIBUTE_NORMAL,			// Normal file.
								 NULL
								 );
	}

	return m_hTempFile!=INVALID_HANDLE_VALUE;
}

BOOL CEntity::CreateEntityMemory()
{
	m_bodySize = 32*1024;
	m_pBody = HeapAlloc(GetProcessHeap(), 0, m_bodySize);
	return m_pBody!=NULL;
}

void * CEntity::GetBuffer() const
{
	return m_pBuffer;
}

ULONG CEntity::GetBufferLength() const
{
	return m_bufferLength;
}

ULONG CEntity::GetLength() const
{
	return m_readLength;
}

void CEntity::Add(ULONG br)
{
	if( br!=0 )
	{
		if( m_mode==eFile )
		{
			ULONG written;
			WriteFile(m_hTempFile, m_pBuffer, br, &written, NULL);
		}
		else if( m_mode==eMemory )
		{
			if( m_bodySize<=(m_readLength+br) )
			{
				m_bodySize += 32*1024;
				void * reallocBuffer = HeapReAlloc(GetProcessHeap(), 0, m_pBody, m_bodySize);
				if( reallocBuffer!=NULL )
				{
					m_pBody = reallocBuffer;
					memcpy((char *)m_pBody + m_readLength, m_pBuffer, br);
				}
			}
			else
				memcpy((char *)m_pBody + m_readLength, m_pBuffer, br);
		}

		m_readLength += br;
	}
}

wstring CEntity::GetFileName() const
{
	wchar_t fl[MAX_PATH];
	UINT rc = GetTempFileName(L".",L"tmp",0,fl);
	return rc ? fl : L"";
}
