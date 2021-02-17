/*
* refer to
* https://www.cnblogs.com/X-Jun/p/9638817.html
* https://www.jianshu.com/p/d6607f0abd72
* https://github.com/MKXJun/DirectX11-With-Windows-SDK
*/

#ifndef D3DAPP_H
#define D3DAPP_H

#include <Windows.h>
#include <wrl/client.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include "Mouse.h"
#include "Keyboard.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

class D3dApp {
public:
	D3dApp(HINSTANCE hInstance, int w, int h);
	virtual ~D3dApp();

	int run();
	virtual bool init();
	virtual void display() = 0;
	LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool initWindow();
	bool initD3d();

	// d3d objects
	//// swapChain [canvas-like] -> (back buffer) -> renderTargetView
	//// device [bridge between dx and hardware] -> depthStencilBuffer -> depthStencilView
	//// renderTargetView  |
	//// depthStencilView  | -> immediateContext
	//// (VIEWPORT)        |
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Device> device;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11DeviceContext> immediateContext;

	std::unique_ptr<DirectX::Mouse> mouse;
	DirectX::Mouse::ButtonStateTracker mouseTracker;
	std::unique_ptr<DirectX::Keyboard> keyboard;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;

	HINSTANCE instance;
	HWND window;
	int windowWidth;
	int windowHeight;
};

#endif 