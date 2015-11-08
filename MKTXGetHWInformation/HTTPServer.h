#pragma once

#pragma comment(lib, "httpapi.lib")

class HttpServer
{

public:

	HttpServer(UINT numPort = 8181) : m_numPort(numPort) {}

	inline VOID InitializeHttpResponse(PHTTP_RESPONSE pResponse, USHORT statusCode, LPCSTR reason)
	{
		RtlZeroMemory(pResponse, sizeof(*pResponse));
		pResponse->StatusCode = statusCode;
		pResponse->pReason = reason;
		pResponse->ReasonLength = (USHORT)strlen(reason);
	}

	inline VOID AddKnownHeader(HTTP_RESPONSE response, HTTP_HEADER_ID headerId, LPCSTR pRawValue)
	{
		response.Headers.KnownHeaders[headerId].pRawValue = pRawValue;
		response.Headers.KnownHeaders[headerId].RawValueLength = (USHORT)strlen(pRawValue);
	}

	DWORD DoReceiveRequests(HANDLE hReqQueue);

	DWORD SendHttpResponse(
		IN HANDLE hReqQueue,
		IN PHTTP_REQUEST pRequest,
		IN USHORT StatusCode,
		IN PSTR pReason,
		IN PSTR pEntity
	);
	DWORD SendHttpPostResponse(
		IN HANDLE        hReqQueue,
		IN PHTTP_REQUEST pRequest
	);


private:

	CONST HTTPAPI_VERSION m_version = HTTPAPI_VERSION_1;
	CONST UINT m_numPort;
	HANDLE m_hReqQueue = NULL;
	ULONG m_retCode = NO_ERROR;

	inline LPVOID WINAPI AllocMem(SIZE_T dwBytes) { HeapAlloc(GetProcessHeap(), 0, dwBytes); }
	inline BOOL WINAPI FreeMem(LPVOID lpMem) { HeapFree(GetProcessHeap(), 0, lpMem); }
	inline VOID Initialize() { m_retCode = HttpInitialize(m_version, HTTP_INITIALIZE_SERVER, NULL); }

};