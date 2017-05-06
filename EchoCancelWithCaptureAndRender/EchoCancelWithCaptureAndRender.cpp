// EchoCancelWithCaptureAndRender.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../AEC/delayEstimation/delayEstimation.h"
#include "../AEC/delayEstimation/delayEstimation_emxAPI.h"

const int ciFrameSize = 480;
const int REFTIMES_PER_SEC = 10000000;
const int REFTIMES_PER_MILLISEC = 10000;
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

const TCHAR *szLoadRenderDataEvent = L"szLoadRenderDataEvent";
const TCHAR *szSaveCaptureDataEvent = L"szSaveCaptureDataEvent";

CRITICAL_SECTION criticalSection;

class WaveData
{
public:
	float* data;
	UINT size;
	WaveData* next;

	WaveData() :
		data(NULL),
		size(0),
		next(NULL)
	{}

	~WaveData()
	{
		delete[] data;
	}
};

float *tmpRenderData;
UINT tmpRenderFramesNum;
float *tmpCaptureData;
UINT tmpCaptureFramesNum;
WaveData *RenderDataQueue;
WaveData *CurrentRenderData;
WaveData *RenderDataTail;
WaveData *CaptureDataQueue;
WaveData *CaptureDataTail;


HRESULT LoadRenderDataThread()
{
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szLoadRenderDataEvent);
	while (true)
	{
		WaitForSingleObject(hEvent, INFINITE);
		EnterCriticalSection(&criticalSection);
		static int position = 0;
		int renderDataPosition = 0;
		tmpRenderData = new float[tmpRenderFramesNum * 2];
		while (CurrentRenderData->next != nullptr && renderDataPosition < tmpRenderFramesNum)
		{
			if (position == 0)
				CurrentRenderData = CurrentRenderData->next;
			for (; position < CurrentRenderData->size && renderDataPosition < tmpRenderFramesNum; position++, renderDataPosition++)
			{
				tmpRenderData[renderDataPosition * 2] = CurrentRenderData->data[position];
				tmpRenderData[renderDataPosition * 2 + 1] = CurrentRenderData->data[position];
			}
			if (position == CurrentRenderData->size)
				position = 0;

		}
		//tmpRenderFramesNum = 20000;
		LeaveCriticalSection(&criticalSection);
	}
	return S_OK;
}

HRESULT SaveCaptureDataThread()
{
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szSaveCaptureDataEvent);
	while (true)
	{
		WaitForSingleObject(hEvent, INFINITE);
		EnterCriticalSection(&criticalSection);
		CaptureDataTail->data = new float[tmpCaptureFramesNum];
		for (int i = 0; i < tmpCaptureFramesNum; i++)
		{
			CaptureDataTail->data[i] = tmpCaptureData[i * 2];
		}
		CaptureDataTail->size = tmpCaptureFramesNum;
		delete[]tmpCaptureData;
		CaptureDataTail->next = new WaveData();
		CaptureDataTail = CaptureDataTail->next;
		LeaveCriticalSection(&criticalSection);
	}
	return S_OK;
}

HRESULT RenderThread()
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioRenderClient *pRenderClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	UINT32 numFramesPadding;
	BYTE *pData;
	DWORD flags = 0;

	HANDLE hEvent = CreateEvent(NULL, true, false, szLoadRenderDataEvent);
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LoadRenderDataThread, NULL, 0, 0);

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	EXIT_ON_ERROR(hr);

	// Tell the audio source which format to use.
	//hr = pMySource->SetFormat(pwfx);
	EXIT_ON_ERROR(hr);

	// Get the actual size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->GetService(
		IID_IAudioRenderClient,
		(void**)&pRenderClient);
	EXIT_ON_ERROR(hr);

	// Grab the entire buffer for the initial fill operation.
	hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
	EXIT_ON_ERROR(hr);

	// Load the initial data into the shared buffer.
	EnterCriticalSection(&criticalSection);
	tmpRenderFramesNum = bufferFrameCount;
	LeaveCriticalSection(&criticalSection);
	PulseEvent(hEvent);
	Sleep(1);
	EnterCriticalSection(&criticalSection);
	CopyMemory(pData, tmpRenderData, tmpRenderFramesNum * 8);
	delete[] tmpRenderData;
	bufferFrameCount = tmpRenderFramesNum;
	LeaveCriticalSection(&criticalSection);

	EXIT_ON_ERROR(hr);

	hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
	EXIT_ON_ERROR(hr);

	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start playing.
	EXIT_ON_ERROR(hr);

	// Each loop fills about half of the shared buffer.
	while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
	{
		// Sleep for half the buffer duration.
		//Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		// See how much buffer space is available.
		hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
		EXIT_ON_ERROR(hr);

		numFramesAvailable = bufferFrameCount - numFramesPadding;

		// Grab all the available space in the shared buffer.
		hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
		EXIT_ON_ERROR(hr);

		// Get next 1/2-second of data from the audio source.
		EnterCriticalSection(&criticalSection);
		tmpRenderFramesNum = bufferFrameCount;
		LeaveCriticalSection(&criticalSection);
		PulseEvent(hEvent);
		Sleep(1);
		EnterCriticalSection(&criticalSection);
		CopyMemory(pData, tmpRenderData, tmpRenderFramesNum * 8);
		delete[] tmpRenderData;
		bufferFrameCount = tmpRenderFramesNum;
		LeaveCriticalSection(&criticalSection);
		EXIT_ON_ERROR(hr);

		hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
		Sleep(10);
		EXIT_ON_ERROR(hr);
	}

	// Wait for last data in buffer to play before stopping.
	//Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

RenderStop:
	hr = pAudioClient->Stop();  // Stop playing.
	EXIT_ON_ERROR(hr);

	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pRenderClient)

		return hr;
}

HRESULT CaptureThread()
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioCaptureClient *pCaptureClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	UINT32 packetLength = 0;
	BOOL bDone = FALSE;
	BYTE *pData;
	DWORD flags;

	HANDLE hEvent = CreateEvent(NULL, true, false, szSaveCaptureDataEvent);
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SaveCaptureDataThread, NULL, 0, 0);
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(
		eCapture, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	EXIT_ON_ERROR(hr);

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr);

	// Notify the audio sink which format to use.
	//hr = pMySink->SetFormat(pwfx);
	EXIT_ON_ERROR(hr);

	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr);

	// Each loop fills about half of the shared buffer.
	while (bDone == FALSE)
	{
		// Sleep for half the buffer duration.
		Sleep(10);

		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		EXIT_ON_ERROR(hr);

		while (packetLength != 0)
		{
			// Get the available data in the shared buffer.
			hr = pCaptureClient->GetBuffer(
				&pData,
				&numFramesAvailable,
				&flags, NULL, NULL);
			EXIT_ON_ERROR(hr);

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				pData = NULL;  // Tell CopyData to write silence.
			}

			// Copy the available capture data to the audio sink.
			EnterCriticalSection(&criticalSection);
			tmpCaptureData = new float[numFramesAvailable * 2];
			tmpCaptureFramesNum = numFramesAvailable;
			CopyMemory(tmpCaptureData, pData, numFramesAvailable * 8);
			LeaveCriticalSection(&criticalSection);
			PulseEvent(hEvent);
			EXIT_ON_ERROR(hr);

			hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
			EXIT_ON_ERROR(hr);

			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr);
		}
	}
CaptureStop:
	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr);

	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pCaptureClient)

		return hr;
}

int main()
{
	RenderDataTail = new WaveData();
	RenderDataQueue = RenderDataTail;
	CurrentRenderData = RenderDataTail;
	RenderDataTail->next = RenderDataTail;
	RenderDataTail->data = new float[2000];
	for (int i = 0; i < 2000; i++)
	{
		RenderDataTail->data[i] = (rand()%100) / (float)500;
	}
	RenderDataTail->size = 2000;
	CaptureDataTail = new WaveData();
	CaptureDataQueue = CaptureDataTail;
	CurrentRenderData = CaptureDataQueue;
	InitializeCriticalSection(&criticalSection);

	HANDLE hRenderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, NULL, 0, 0);
	HANDLE hCaptureThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CaptureThread, NULL, 0, 0);

	WaitForSingleObject(hRenderThread, INFINITE);
	//远端语音数据
	//short *sFarEnd = new short[ciFrameSize];
	//FILE *fpFarWave = fopen("farEnd.wav", "rb");
	//int iFarWaveEnd = fseek(fpFarWave, 0, SEEK_END);
	//int iFarWaveBegin = fseek(fpFarWave, 44, SEEK_SET);
	//int iFarWaveLength = (iFarWaveEnd - iFarWaveBegin) / sizeof(short);

	//fread(sFarEnd, sizeof(short), ciFrameSize, fpFarWave);

	//FILE *fp_far = fopen("farEndSound.wav", "rb");
	//FILE *fp_near = fopen("mix.wav", "rb");
	//FILE *fp_out = fopen("out.wav", "wb");

	//short *far_frame;
	//short *near_frame;
	//short *out_frame;

	//far_frame = new short[44];
	//near_frame = new short[44];
	//out_frame = new short[44];

	//char* data;
	//data = new char[44];
	//fread(far_frame, sizeof(char), 44, fp_far);
	//long fileBegin = ftell(fp_far);
	//fwrite(far_frame, sizeof(char), 44, fp_out);
	//fread(near_frame, sizeof(char), 44, fp_near);

	//delete[]far_frame;
	//delete[]near_frame;
	//delete[]out_frame;

	//fseek(fp_far, 0, SEEK_END);
	//long fileEnd = ftell(fp_far);


	//fseek(fp_far, 44, SEEK_SET);
	//fseek(fp_near, 44, SEEK_SET);
	//int fileLength = (fileEnd - fileBegin) / 2;
	//int audioLength1s = fileLength;
	//far_frame = new short[audioLength1s];
	//near_frame = new short[audioLength1s];
	//out_frame = new short[audioLength1s];
	//for (int i = 0; i < 5; i++)
	//{
	//	fread(far_frame, sizeof(short), audioLength1s, fp_far);
	//	fread(near_frame, sizeof(short), audioLength1s, fp_near);

	//	float *farEnd_f = new float[audioLength1s];
	//	for (int i = 0; i < audioLength1s; i++)
	//	{
	//		farEnd_f[i] = far_frame[i] / float(65536 / 2);
	//	}

	//	emxArray_real32_T  *farEnd;
	//	farEnd = emxCreateWrapper_real32_T(farEnd_f, audioLength1s, 1);


	//	float *nearEnd_f = new float[audioLength1s];
	//	for (int i = 0; i < audioLength1s; i++)
	//	{
	//		nearEnd_f[i] = near_frame[i] / float(65536 / 2);
	//	}

	//	emxArray_real32_T  *nearEnd;
	//	nearEnd = emxCreateWrapper_real32_T(nearEnd_f, audioLength1s, 1);

	//	int delay = delayEstimation(farEnd, nearEnd);


	//	float *echo_f = new float[audioLength1s];

	//	emxArray_real32_T *echo, *m, *en;
	//	echo = emxCreateWrapper_real32_T(echo_f, audioLength1s, 1);
	//	m = emxCreateWrapper_real32_T(echo_f, 20, 1);
	//	en = emxCreateWrapper_real32_T(echo_f, audioLength1s, 1);

	//	NLMS(nearEnd, farEnd, 20, 1, 0, echo, m, en);

	//	int a = 0;
	//}
	DeleteCriticalSection(&criticalSection);
	return 0;
}
