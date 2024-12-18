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
	eye(), drt(), dt(Constant::dt), bound(Constant::bound), 
	relView(), xangle(0.0), yangle(0.0),
	vertexLayout(nullptr),
	vertexShader(nullptr),
	pixelShader(nullptr) {

	// insert objects to the scene
	// dynamic binding
	boxes.push_back(new Car(DirectX::XMFLOAT3(0.0, 0.7, 0.0), DirectX::XMFLOAT3(1.5, 0.7, 0.7)));
	boxes.push_back(new Box(DirectX::XMFLOAT3(0.0, bound, bound), DirectX::XMFLOAT3(bound, bound, 0.01)));
	boxes.push_back(new Box(DirectX::XMFLOAT3(0.0, bound, -bound), DirectX::XMFLOAT3(bound, bound, 0.01)));
	boxes.push_back(new Box(DirectX::XMFLOAT3(bound, bound, 0.0), DirectX::XMFLOAT3(0.01, bound, bound)));
	boxes.push_back(new Box(DirectX::XMFLOAT3(-bound, bound, 0.0), DirectX::XMFLOAT3(0.01, bound, bound)));
	boxes.push_back(new Box(DirectX::XMFLOAT3(0.0, -0.01, 0.0), DirectX::XMFLOAT3(bound, 0.01, bound)));

}

GameApp::~GameApp() {}

void GameApp::update() {
	// get mouse state
	DirectX::Mouse::State mouseState = mouse->GetState();
	DirectX::Mouse::State lastMouseState = mouseTracker.GetLastState();

	// get keyboard state
	DirectX::Keyboard::State keyState = keyboard->GetState();
	keyboardTracker.Update(keyState);

	// store old value in case of collision
	DirectX::XMFLOAT3 oldEye = eye;
	DirectX::XMFLOAT3 oldDrt = drt;;
	DirectX::XMMATRIX oldView = relView;

	// key events
	if (keyState.IsKeyDown(DirectX::Keyboard::W)) forward();
	if (keyState.IsKeyDown(DirectX::Keyboard::A)) turnLeft();
	if (keyState.IsKeyDown(DirectX::Keyboard::S)) backward();
	if (keyState.IsKeyDown(DirectX::Keyboard::D)) turnRight();
	if (keyState.IsKeyDown(DirectX::Keyboard::Escape)) SendMessage(window, WM_DESTROY, 0, 0);

	// mouse events
	if (mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE) {
		xangle += mouseState.y * dt;
		yangle += mouseState.x * dt;
	}

	// update object transformation
	for (auto& b : boxes) {
		// implicitly do collision detection
		// if collide, abandon all update and do not update view buffer
		if (!b->update(relView)) {
			eye = oldEye;
			drt = oldDrt;
			relView = oldView;
			return;
		}
	}

	// update view buffer
	view.view = DirectX::XMMatrixRotationX(xangle) * DirectX::XMMatrixRotationY(yangle) * relView;
	D3D11_MAPPED_SUBRESOURCE mappedDataView;
	immediateContext->Map(viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataView);
	memcpy_s(mappedDataView.pData, sizeof(view), &view, sizeof(view));
	immediateContext->Unmap(viewBuffer.Get(), 0);
}

void GameApp::display() {
	assert(immediateContext);
	assert(swapChain);

	// display background
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	immediateContext->ClearRenderTargetView(renderTargetView.Get(), bgColor);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// display every object
	for (auto& b : boxes) {
		b->display(immediateContext.Get());
	}

	swapChain->Present(0, 0);
}

bool GameApp::init() {
	if (!D3dApp::init()) return false;
	if (!initEffect()) return false;
	if (!initResource()) return false;
	mouse->SetWindow(window);
	mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
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
	eye = DirectX::XMFLOAT3(-2.0f, 2.0f, 0.0f);
	drt = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	world.world = DirectX::XMMatrixIdentity();
	updateView();
	proj.proj = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XM_PIDIV2,
			static_cast<float>(Constant::width) / Constant::height,
			0.1f,
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
	immediateContext->VSSetConstantBuffers(BufferType::WORLD, 1, worldBuffer.GetAddressOf());
	immediateContext->VSSetConstantBuffers(BufferType::VIEW , 1, viewBuffer.GetAddressOf());
	immediateContext->VSSetConstantBuffers(BufferType::PROJ , 1, projBuffer.GetAddressOf());

	// init every object
	for (auto& b : boxes) {
		if (!b->init(device.Get())) return false;
	}
	
	// set topology structure
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set shaders
	immediateContext->IASetInputLayout(vertexLayout.Get());
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	return true;
}

DirectX::XMVECTOR GameApp::toVector(const DirectX::XMFLOAT3& f) const {
	return DirectX::XMVectorSet(f.x, f.y, f.z, 0.0);
}

void GameApp::normalizeDrt() {
	drt.x = DirectX::XMVectorGetX(relView.r[2]);
	drt.y = DirectX::XMVectorGetY(relView.r[2]);
	drt.z = DirectX::XMVectorGetZ(relView.r[2]);
}

void GameApp::forward() {
	eye.x = eye.x + dt * drt.x;
	eye.y = eye.y + dt * drt.y;
	eye.z = eye.z + dt * drt.z;
	updateView();
}

void GameApp::backward() {
	eye.x = eye.x - dt * drt.x;
	eye.y = eye.y - dt * drt.y;
	eye.z = eye.z - dt * drt.z;
	updateView();
}

void GameApp::turnRight() {
	relView = DirectX::XMMatrixRotationY(0.5 * dt) * relView;
	normalizeDrt();
}

void GameApp::turnLeft() {
	relView = DirectX::XMMatrixRotationY(-0.5 * dt) * relView;
	normalizeDrt();
}

void GameApp::updateView() {
	relView = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixLookToLH(
			toVector(eye),
			toVector(drt),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	);
	normalizeDrt();
}