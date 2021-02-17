/*
* refer to 
* https://www.cnblogs.com/X-Jun/p/9638817.html
* https://www.jianshu.com/p/d6607f0abd72
* https://github.com/MKXJun/DirectX11-With-Windows-SDK
*/

#include "D3dApp.h"

namespace {
	D3dApp* thisptr = nullptr;
}

D3dApp::D3dApp(HINSTANCE hInstance, int w, int h) :
	instance(hInstance),
	window(nullptr),
	windowWidth(w),
	windowHeight(h),
	device(nullptr),
	swapChain(nullptr),
	immediateContext(nullptr),
	renderTargetView(nullptr),
	depthStencilView(nullptr),
	depthStencilBuffer(nullptr) {
	thisptr = this; 
}

D3dApp::~D3dApp() {}

bool D3dApp::init() {
	mouse = std::make_unique<DirectX::Mouse>();
	keyboard = std::make_unique<DirectX::Keyboard>();
	if (!initWindow()) return false;
	if (!initD3d()) return false;
	return true;
}

int D3dApp::run() {
	MSG msg; 
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			update();
			display();
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return thisptr->wndProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK D3dApp::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	/*case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) DestroyWindow(hwnd);
		break;*/
	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		mouse->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_ACTIVATEAPP:
		mouse->ProcessMessage(msg, wParam, lParam);
		keyboard->ProcessMessage(msg, wParam, lParam);
		return 0;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3dApp::initWindow() {

	// create window class and register
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = mainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Direct3D11App";
	if (!RegisterClass(&wc)) {
		MessageBox(0, L"Register window class failed...", 0, 0);
		return false;
	}

	// create window
	window = CreateWindow(L"Direct3D11App",
		                  L"Direct3D11App",
		                  WS_OVERLAPPEDWINDOW, 
						  CW_USEDEFAULT, 
		                  CW_USEDEFAULT, 
						  windowWidth, 
						  windowHeight, 
						  0, 
						  0, 
						  instance, 
						  0);
	if (!window) { 
		MessageBox(0, L"CreateWindow failed...", 0, 0);
		return false;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	return true;
}

bool D3dApp::initD3d() {

	// fill swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = windowWidth;
	sd.BufferDesc.Height = windowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = window;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// feature level
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// create device and swap chain
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&sd,
		swapChain.GetAddressOf(),
		device.GetAddressOf(),
		NULL,
		immediateContext.GetAddressOf()))) {
		MessageBox(0, L"CreateDeviceAndSwapChain failed...", 0, 0);
		return false;
	}

	HRESULT hr = 0;

	// swapChain to back buffer
	ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	hr = swapChain->GetBuffer(0,  __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"swapChain to back buffer failed...", 0, 0);
		return false;
	}

	// back buffer to renderTargetView
	hr = device->CreateRenderTargetView(pBackBuffer.Get(), NULL, renderTargetView.GetAddressOf());
	pBackBuffer->Release();
	if (FAILED(hr)) {
		MessageBox(0, L"back buffer to renderTargetView failed...", 0, 0);
		return false;
	}

	// device to stencil buffer
	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.Width = windowWidth;
	dsDesc.Height = windowHeight;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = device->CreateTexture2D(&dsDesc, 0, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"device to stencil buffer failed...", 0, 0);
		return false;
	}

	// stencil buffer to depthStencilView
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), 0, depthStencilView.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"stencil buffer to depthStencilView failed...", 0, 0);
		return false;
	}

	// renderTargetView and depthStencilView to deviceContent
	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	// viewPort to deviceContent
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(windowWidth);
	vp.Height = static_cast<float>(windowHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports(1, &vp);

	return true;
}