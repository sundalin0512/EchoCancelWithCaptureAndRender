#pragma once
class CaptureClient
{
public:
	CaptureClient();
	~CaptureClient();

	HRESULT Initialize()
	{

	}
	HRESULT Start();
	HRESULT Stop();
	static HRESULT RunThread();
};

