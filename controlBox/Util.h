#ifndef UTIL_H
#define UTIL_H

//#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

HRESULT CreateShaderFromFile(
	const WCHAR* hlslFileName,
	LPCSTR entryPoint,
	LPCSTR shaderModel,
	ID3DBlob** ppBlobOut);

#endif
