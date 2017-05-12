// EchoCancelWithCaptureAndRender.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include "../AEC/delayEstimation/delayEstimation.h"
#include "../AEC/delayEstimation/delayEstimation_emxAPI.h"

using namespace std;

const int ciFrameSize = 480;
const int REFTIMES_PER_SEC = 10000000;
const int REFTIMES_PER_MILLISEC = 10000;
const int audioLength5s = 240000;

int renderProcessDataNum = 0;
int captureProcessDataNum = 0;
int renderProcessPosition = 0;
int captureProcessPosition = 0;

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

const TCHAR *szLoadRenderDataEvent = L"szLoadRenderDataEvent";
const TCHAR *szSaveCaptureDataEvent = L"szSaveCaptureDataEvent";
const TCHAR *szRenderThreadEvent = L"RenderThreadEvent";
//const TCHAR *szStopEvent = L"StopEvent";
bool StopFlag = false;

CRITICAL_SECTION criticalSection;

WAVEFORMATEX *WaveFormat;

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
WaveData *OutputDataQueue;
WaveData *OutputDataTail;

//  Header for a WAV file - we define a structure describing the first few fields in the header for convenience.
//
struct WAVEHEADER
{
	DWORD   dwRiff;                     // "RIFF"
	DWORD   dwSize;                     // Size
	DWORD   dwWave;                     // "WAVE"
	DWORD   dwFmt;                      // "fmt "
	DWORD   dwFmtSize;                  // Wave Format Size
};

//  Static RIFF header, we'll append the format to it.
const BYTE szWaveHeader[] =
{
	'R',   'I',   'F',   'F',  0x00,  0x00,  0x00,  0x00, 'W',   'A',   'V',   'E',   'f',   'm',   't',   ' ', 0x00, 0x00, 0x00, 0x00
};

//  Static wave DATA tag.
const BYTE szWaveData[] = { 'd', 'a', 't', 'a' };

LRESULT SaveToFile()
{
	FILE *fp = fopen("waveabc.bin", "wb");
	if (fp)
	{
		vector<float> Buffer;
		size_t BufferSize = 0;
		while (OutputDataQueue->next != nullptr)
		{
			BufferSize += OutputDataQueue->size;
			Buffer.insert(Buffer.end(), OutputDataQueue->data, OutputDataQueue->data + OutputDataQueue->size);
			OutputDataQueue = OutputDataQueue->next;
		}
		BufferSize *= sizeof(float);
		DWORD waveFileSize = sizeof(WAVEHEADER) + sizeof(WAVEFORMATEX) + WaveFormat->cbSize + sizeof(szWaveData) + sizeof(DWORD) + static_cast<DWORD>(BufferSize);
		BYTE *waveFileData = new (std::nothrow) BYTE[waveFileSize];
		BYTE *waveFilePointer = waveFileData;
		WAVEHEADER *waveHeader = reinterpret_cast<WAVEHEADER *>(waveFileData);

		if (waveFileData == NULL)
		{
			printf("Unable to allocate %d bytes to hold output wave data\n", waveFileSize);
			return false;
		}

		//
		//  Copy in the wave header - we'll fix up the lengths later.
		//
		CopyMemory(waveFilePointer, szWaveHeader, sizeof(szWaveHeader));
		waveFilePointer += sizeof(szWaveHeader);

		//
		//  Update the sizes in the header.
		//
		waveHeader->dwSize = waveFileSize - (2 * sizeof(DWORD));
		waveHeader->dwFmtSize = sizeof(WAVEFORMATEX) + WaveFormat->cbSize;

		//
		//  Next copy in the WaveFormatex structure.
		//
		CopyMemory(waveFilePointer, WaveFormat, sizeof(WAVEFORMATEX) + WaveFormat->cbSize);
		waveFilePointer += sizeof(WAVEFORMATEX) + WaveFormat->cbSize;


		//
		//  Then the data header.
		//
		CopyMemory(waveFilePointer, szWaveData, sizeof(szWaveData));
		waveFilePointer += sizeof(szWaveData);
		*(reinterpret_cast<DWORD *>(waveFilePointer)) = static_cast<DWORD>(BufferSize);
		waveFilePointer += sizeof(DWORD);

		//
		//  And finally copy in the audio data.
		//
		CopyMemory(waveFilePointer, Buffer.data(), BufferSize);

		//
		//  Last but not least, write the data to the file.
		//
		DWORD bytesWritten;
		if (!(bytesWritten = fwrite(waveFileData, sizeof(BYTE), waveFileSize , fp)))
		{
			printf("Unable to write wave file: %d\n", GetLastError());
			delete[]waveFileData;
			return false;
		}

		//if (bytesWritten != waveFileSize)
		//{
		//	printf("Failed to write entire wave file\n");
		//	delete[]waveFileData;
		//	return false;
		//}
		delete[]waveFileData;
		fclose(fp);
	}
	return S_OK;
}

HRESULT LoadRenderDataThread()
{
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szLoadRenderDataEvent);
	HANDLE hRenderEvent = OpenEvent(EVENT_ALL_ACCESS, false, szRenderThreadEvent);
	while (true)
	{
		if (StopFlag)
			return S_OK;
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
		PulseEvent(hRenderEvent);
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
		if (StopFlag)
			return S_OK;
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
		//尾节点不包含数据
		CaptureDataTail = CaptureDataTail->next;
		LeaveCriticalSection(&criticalSection);

	}
	return S_OK;
}

HRESULT RenderThread()
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
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
	HANDLE hRenderEvent = CreateEvent(NULL, true, false, szRenderThreadEvent);
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
	WaitForSingleObject(hRenderEvent, 10);
	EnterCriticalSection(&criticalSection);
	CopyMemory(pData, tmpRenderData, tmpRenderFramesNum * 8);
	delete[] tmpRenderData;
	LeaveCriticalSection(&criticalSection);

	EXIT_ON_ERROR(hr);

	hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->Start();  // Start playing.
	EXIT_ON_ERROR(hr);

	// Each loop fills about half of the shared buffer.
	while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
	{

		// See how much buffer space is available.
		hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
		EXIT_ON_ERROR(hr);

		numFramesAvailable = bufferFrameCount - numFramesPadding;

		// Grab all the available space in the shared buffer.
		hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
		EXIT_ON_ERROR(hr);

		// Get next 1/2-second of data from the audio source.
		EnterCriticalSection(&criticalSection);
		tmpRenderFramesNum = numFramesAvailable;
		LeaveCriticalSection(&criticalSection);
		PulseEvent(hEvent);
		WaitForSingleObject(hRenderEvent, 10);
		EnterCriticalSection(&criticalSection);
		CopyMemory(pData, tmpRenderData, tmpRenderFramesNum * 8);
		delete[] tmpRenderData;
		LeaveCriticalSection(&criticalSection);
		EXIT_ON_ERROR(hr);

		hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
		if (StopFlag)
		{
			goto RenderStop;
		}
		Sleep(500);
		EXIT_ON_ERROR(hr);
	}

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
	WaveFormat = new WAVEFORMATEX(*pwfx);
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

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr);

	// Each loop fills about half of the shared buffer.
	while (bDone == FALSE)
	{
		// Sleep for half the buffer duration.
		if (StopFlag)
		{
			goto CaptureStop;
		}
		Sleep(1);

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

LRESULT ProcessThread()
{
BeginProcess:
	int iter = 20;
	float *nearEnd_f = new float[audioLength5s + iter];
	float *farEnd_f = new float[audioLength5s + iter];
	for (int i = 0; i < iter; i++)
	{
		nearEnd_f[i] = 0;
		farEnd_f[i] = 0;
	}
	emxArray_real32_T  *farEnd;
	emxArray_real32_T  *nearEnd;
	emxArray_real32_T *echo, *m, *en;
ContinueProcess:
	if (StopFlag)
		goto StopProcess;
	EnterCriticalSection(&criticalSection);
	if (RenderDataQueue->next != nullptr && CaptureDataQueue->next != nullptr)
	{

		//TODO 更新指针
		while (captureProcessDataNum < audioLength5s)
		{
			int copySize = 0;
			memcpy(nearEnd_f + iter + captureProcessDataNum, CaptureDataQueue->data + captureProcessPosition, copySize = sizeof(float) * min(CaptureDataQueue->size, audioLength5s - captureProcessDataNum));
			if (copySize < CaptureDataQueue->size)
				captureProcessPosition = copySize;
			else
				captureProcessPosition = 0;

			captureProcessDataNum += copySize;
			if (CaptureDataQueue->next != nullptr)
				CaptureDataQueue = CaptureDataQueue->next;
			else
			{
				LeaveCriticalSection(&criticalSection);
				goto ContinueProcess;
			}
		}

		while (renderProcessDataNum < audioLength5s)
		{
			int copySize = 0;
			memcpy(farEnd_f + iter + renderProcessDataNum, RenderDataQueue->data + renderProcessPosition, copySize = sizeof(float)*min(RenderDataQueue->size, audioLength5s - renderProcessDataNum));
			if (copySize < RenderDataQueue->size)
				renderProcessPosition = copySize;
			else
				renderProcessPosition = 0;

			renderProcessDataNum += copySize;
			if (RenderDataQueue->next != nullptr)
				RenderDataQueue = RenderDataQueue->next;
			else
			{
				LeaveCriticalSection(&criticalSection);
				goto ContinueProcess;
			}
		}
		LeaveCriticalSection(&criticalSection);
	}
	else
	{
		LeaveCriticalSection(&criticalSection);
		Sleep(100);
		goto ContinueProcess;
	}
	captureProcessDataNum = 0;
	renderProcessDataNum = 0;
	farEnd = emxCreateWrapper_real32_T(farEnd_f, audioLength5s + iter, 1);
	nearEnd = emxCreateWrapper_real32_T(nearEnd_f, audioLength5s + iter, 1);
	int delay = delayEstimation(farEnd, nearEnd);
	emxDestroyArray_real32_T(farEnd);
	emxDestroyArray_real32_T(nearEnd);
	farEnd = emxCreateWrapper_real32_T(farEnd_f, 1, audioLength5s + iter);
	nearEnd = emxCreateWrapper_real32_T(nearEnd_f, 1, audioLength5s + iter);
	float *echo_f = new float[audioLength5s + iter];
	echo = emxCreateWrapper_real32_T(echo_f, 1, audioLength5s + iter);
	m = emxCreateWrapper_real32_T(echo_f, 1, iter);
	en = emxCreateWrapper_real32_T(echo_f, audioLength5s + iter, 1);

	NLMS(nearEnd, farEnd, iter, 1, 0, echo, m, en);

	EnterCriticalSection(&criticalSection);
	OutputDataTail->size = audioLength5s;
	OutputDataTail->data = new float[audioLength5s*2];
	for (int i = 0; i < audioLength5s; i++)
	{
		OutputDataTail->data[i * 2] = nearEnd_f[i + iter] - echo_f[i + iter];
		OutputDataTail->data[i * 2 + 1] = OutputDataTail->data[i * 2];
	}
	OutputDataTail->next = new WaveData();
	OutputDataTail = OutputDataTail->next;
	LeaveCriticalSection(&criticalSection);
StopProcess:
	delete[]farEnd_f;
	delete[]nearEnd_f;
	if (StopFlag)
	{
		//SaveToFile();
		return S_OK;
	}
	goto BeginProcess;
	return S_OK;
}

void Stop()
{
	char ch;
	scanf("%c", &ch);
	StopFlag = true;
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
		RenderDataTail->data[i] = (rand() % 100) / (float)500;
	}
	RenderDataTail->size = 2000;
	CaptureDataTail = new WaveData();
	CaptureDataQueue = CaptureDataTail;
	CurrentRenderData = CaptureDataQueue;

	OutputDataTail = new WaveData();
	OutputDataQueue = OutputDataTail;
	InitializeCriticalSection(&criticalSection);

	HANDLE hRenderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, NULL, 0, 0);
	HANDLE hCaptureThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CaptureThread, NULL, 0, 0);
	HANDLE hProcessThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessThread, NULL, 0, 0);
	HANDLE hStopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Stop, NULL, 0, 0);

	WaitForSingleObject(hProcessThread, INFINITE);
	SaveToFile();
	Sleep(100);
	DeleteCriticalSection(&criticalSection);

	
	return 0;
}
