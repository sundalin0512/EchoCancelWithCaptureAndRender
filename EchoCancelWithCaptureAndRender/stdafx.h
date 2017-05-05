// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1
#include <new>
#include <windows.h>
#include <strsafe.h>
#include <objbase.h>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#pragma warning(pop)

static bool DisableMMCSS;

#define EXIT_ON_ERROR(hres)  \
			  if (FAILED(hres)) { exit(-1); }
#define SAFE_RELEASE(punk)  \
			  if ((punk) != NULL)  \
				{ (punk)->Release(); (punk) = NULL; }


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
