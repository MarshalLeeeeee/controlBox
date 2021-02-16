/*
* refer to
* https://www.cnblogs.com/X-Jun/p/9638817.html
* https://www.jianshu.com/p/d6607f0abd72
* https://github.com/MKXJun/DirectX11-With-Windows-SDK
*/

#include "GameApp.h"
#include "Util.h"

GameApp::GameApp(HINSTANCE hInstance, int w, int h) : 
	D3dApp(hInstance, w, h),
	worldBuffer(nullptr), world(),
	viewBuffer(nullptr), view(),
	projBuffer(nullptr), proj(),
	vertexLayout(nullptr),
	vertexShader(nullptr),
	pixelShader(nullptr) {
	boxes.push_back(Box());
	boxes.push_back(Box(DirectX::XMFLOAT3(0.0, -4.0, 0.0), DirectX::XMFLOAT3(0.5, 0.5, 0.5)));
	boxes.push_back(Box(DirectX::XMFLOAT3(0.0, -8.0, 0.0), DirectX::XMFLOAT3(0.25, 0.25, 0.25)));

}

GameApp::~GameApp() {}

void GameApp::display() {
	assert(immediateContext);
	assert(swapChain);

	// display background
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	immediateContext->ClearRenderTargetView(renderTargetView.Get(), bgColor);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// display every object
	for (auto& b : boxes) {
		b.display(immediateContext.Get());
	}

	swapChain->Present(0, 0);
}

bool GameApp::init() {
	if (!D3dApp::init()) return false;
	if (!initEffect()) return false;
	if (!initResource()) return false;
	return true;
}

bool GameApp::initEffect() {
	ComPtr<ID3DBlob> blob;

	// compile vertex shader
	if (FAILED(CreateShaderFromFile(L"CubeVertex.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()))) {
		MessageBox(0, L"compile vertex shader failed...", 0, 0);
		return false;
	}

	// create vertex shader
	if (FAILED(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vertexShader.GetAddressOf()))) {
		MessageBox(0, L"create vertex shader failed...", 0, 0);
		return false;
	}

	// set input layout
	if (FAILED(device->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), vertexLayout.GetAddressOf()))) {
		MessageBox(0, L"set input layout failed...", 0, 0);
		return false;
	}

	// compile pixel shader
	if (FAILED(CreateShaderFromFile(L"CubePixelBox.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()))) {
		MessageBox(0, L"compile pixel shader failed...", 0, 0);
		return false;
	}

	// create pixel shader
	if (FAILED(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pixelShader.GetAddressOf()))) {
		MessageBox(0, L"create pixel shader failed...", 0, 0);
		return false;
	}

	return true;
}

bool GameApp::initResource() {
	/*
	// init vertices buffer
	VertexPosColor vertices[] =
	{
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f),  DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f),   DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f),   DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f),   DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initDataV;
	ZeroMemory(&initDataV, sizeof(initDataV));
	initDataV.pSysMem = vertices;
	if (FAILED(device->CreateBuffer(&vbd, &initDataV, vertexBuffer.GetAddressOf()))) {
		MessageBox(0, L"init vertices buffer failed...", 0, 0);
		return false;
	}

	// set vertice buffer
	// UINT stride = sizeof(VertexPosColor);
	// UINT offset = 0;
	// immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// init indices buffer
	DWORD indices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 1,
		1, 0, 4,
		1, 5, 6,
		6, 2, 1,
		7, 6, 5,
		5, 4, 7,
		3, 2, 6,
		6, 7, 3,
		4, 0, 3,
		3, 7, 4
	};

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initDataI;
	ZeroMemory(&initDataI, sizeof(initDataI));
	initDataI.pSysMem = indices;
	if (FAILED(device->CreateBuffer(&ibd, &initDataI, indexBuffer.GetAddressOf()))) {
		MessageBox(0, L"init indices buffer failed...", 0, 0);
		return false;
	}

	// set indice buffer
	// immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	

	// init constant buffer
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(device->CreateBuffer(&cbd, nullptr, constantBuffer.GetAddressOf()))) {
		MessageBox(0, L"init constant buffer failed...", 0, 0);
		return false;
	}

	cBuffer.world = DirectX::XMMatrixIdentity();
	cBuffer.view = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(5.0f, 5.0f, -5.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	);
	cBuffer.proj = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XM_PIDIV2, 
			static_cast<float>(windowWidth) /windowHeight, 
			1.0f, 
			1000.0f)
	);

	// set constant buffer
	// immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	*/

	// init constant buffers
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.ByteWidth = sizeof(WorldBuffer);
	if (FAILED(device->CreateBuffer(&cbd, nullptr, worldBuffer.GetAddressOf()))) {
		MessageBox(0, L"init world buffer failed...", 0, 0);
		return false;
	}
	cbd.ByteWidth = sizeof(ViewBuffer);
	if (FAILED(device->CreateBuffer(&cbd, nullptr, viewBuffer.GetAddressOf()))) {
		MessageBox(0, L"init view buffer failed...", 0, 0);
		return false;
	}
	cbd.ByteWidth = sizeof(ProjBuffer);
	if (FAILED(device->CreateBuffer(&cbd, nullptr, projBuffer.GetAddressOf()))) {
		MessageBox(0, L"init proj buffer failed...", 0, 0);
		return false;
	}

	// set constant buffers
	world.world = DirectX::XMMatrixIdentity();
	view.view = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(5.0f, 5.0f, 5.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	);
	proj.proj = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XM_PIDIV2,
			static_cast<float>(Constant::width) / Constant::height,
			1.0f,
			1000.0f)
	);

	D3D11_MAPPED_SUBRESOURCE mappedDataWorld;
	immediateContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataWorld);
	memcpy_s(mappedDataWorld.pData, sizeof(world), &world, sizeof(world));
	immediateContext->Unmap(worldBuffer.Get(), 0);

	D3D11_MAPPED_SUBRESOURCE mappedDataView;
	immediateContext->Map(viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataView);
	memcpy_s(mappedDataView.pData, sizeof(view), &view, sizeof(view));
	immediateContext->Unmap(viewBuffer.Get(), 0);

	D3D11_MAPPED_SUBRESOURCE mappedDataProj;
	immediateContext->Map(projBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataProj);
	memcpy_s(mappedDataProj.pData, sizeof(proj), &proj, sizeof(proj));
	immediateContext->Unmap(projBuffer.Get(), 0);

	// bind constant buffers to vertex shader
	immediateContext->VSSetConstantBuffers(0, 1, worldBuffer.GetAddressOf());
	immediateContext->VSSetConstantBuffers(1, 1, viewBuffer.GetAddressOf());
	immediateContext->VSSetConstantBuffers(2, 1, projBuffer.GetAddressOf());

	// init every object
	for (auto& b : boxes) {
		if (!b.init(device.Get())) return false;
	}
	
	// set topology structure
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set shaders
	immediateContext->IASetInputLayout(vertexLayout.Get());
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	return true;
}