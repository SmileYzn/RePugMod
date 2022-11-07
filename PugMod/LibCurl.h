#pragma once

struct P_CURL_MOD_MEMORY
{
	char* Memory;
	size_t Size;
	void* Callback;
};

class CLibCurl
{
public:
	void Init();
	void Stop();

	void Frame();

	void Get(const char* url, void* FunctionCallback);

	static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

private:
	CURLM* m_MultiHandle = NULL;

	long m_RequestIndex = 0;

	std::map<int, P_CURL_MOD_MEMORY> m_Data;
};

extern CLibCurl gLibCurl;
