#pragma once
#include "stdafx.h"
#include "WASAPICapture.h"
#include "WASAPIRenderer.h"

CWASAPIRenderer* InitRenderer()
{
	//初始化render
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	IMMDevice *device = NULL;
	deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	CWASAPIRenderer *renderer = new (std::nothrow) CWASAPIRenderer(device, true, eConsole);
	SafeRelease(&deviceEnumerator);
	return renderer;
}

CWASAPICapture* InitCapturer()
{
	//初始化capturer
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	IMMDevice *device = NULL;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &device);
	CWASAPICapture *capturer = new (std::nothrow) CWASAPICapture(device, true, eConsole);
	SafeRelease(&deviceEnumerator);
	return capturer;
}