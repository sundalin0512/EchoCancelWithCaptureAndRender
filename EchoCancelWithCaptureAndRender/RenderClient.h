#pragma once
struct RenderBuffer
{
	RenderBuffer *  _Next;
	UINT32          _BufferLength;
	BYTE *          _Buffer;

	RenderBuffer() :
		_Next(NULL),
		_BufferLength(0),
		_Buffer(NULL)
	{
	}

	~RenderBuffer()
	{
		delete[] _Buffer;
	}
};

class RenderClient
{
private:
	const int ns_10ms = 10000;
	IAudioClient *pAudioClient;
	RenderBuffer *RenderBufferQueue;
public:
	RenderClient();
	~RenderClient();

	HRESULT Initialize()
	{
		HRESULT hr;
		REFERENCE_TIME hnsRequestedDuration = ns_10ms;
		UINT32 bufferFrameCount;
		IMMDeviceEnumerator *pEnumerator = NULL;
		IMMDevice *pDevice = NULL;
		pAudioClient = NULL;
		IAudioCaptureClient *pCaptureClient = NULL;
		WAVEFORMATEX *pwfx = NULL;

		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator);
		EXIT_ON_ERROR(hr)

			hr = pEnumerator->GetDefaultAudioEndpoint(
				eCapture, eConsole, &pDevice);
		EXIT_ON_ERROR(hr)

			hr = pDevice->Activate(
				IID_IAudioClient, CLSCTX_ALL,
				NULL, (void**)&pAudioClient);
		EXIT_ON_ERROR(hr)

			hr = pAudioClient->GetMixFormat(&pwfx);
		EXIT_ON_ERROR(hr)

			hr = pAudioClient->Initialize(
				AUDCLNT_SHAREMODE_SHARED,
				0,
				hnsRequestedDuration,
				0,
				pwfx,
				NULL);
		EXIT_ON_ERROR(hr)

			// Get the size of the allocated buffer.
			hr = pAudioClient->GetBufferSize(&bufferFrameCount);
		EXIT_ON_ERROR(hr)

			hr = pAudioClient->GetService(
				IID_IAudioCaptureClient,
				(void**)&pCaptureClient);
		EXIT_ON_ERROR(hr)
			return hr;
	}
	HRESULT Start();
	HRESULT Stop();
	static HRESULT RunThread();
};

