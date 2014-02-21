#pragma once

#include <string>
using namespace std;

class CEntity
{
public:
	enum EntityMode { eFile,eMemory };

	CEntity(EntityMode mod=eFile);
	virtual ~CEntity(void);
	
	void * GetBuffer() const;
	ULONG GetBufferLength() const;
	ULONG GetLength() const;
	void Add(ULONG br);

private:
	BOOL CreateEntityFile();
	BOOL CreateEntityMemory();

	wstring GetFileName() const;

private:

	EntityMode m_mode;

	PVOID m_pBuffer;
	ULONG m_bufferLength;		// defalut value 4K
	ULONG m_readLength;

	HANDLE m_hTempFile;
	wstring m_fileName;

	PVOID m_pBody;
	ULONG m_bodySize;
};

