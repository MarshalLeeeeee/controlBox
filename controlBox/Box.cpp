#include "Box.h"

const D3D11_INPUT_ELEMENT_DESC VertexPosColor::inputLayout[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

Box::Box() :
	vertexBuffer(nullptr),
	indexBuffer(nullptr),
	vertexStride(),
	indexCount(),
	world(),
	pos(0.0, 0.0, 0.0),
	scale(1.0, 1.0, 1.0) {}

Box::Box(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 s) :
	vertexBuffer(nullptr),
	indexBuffer(nullptr),
	vertexStride(),
	indexCount(),
	world(),
	pos(p),
	scale(s) {}

bool Box::init(ID3D11Device* device) {
	// init vertices buffer
	VertexPosColor vertices[] =
	{
		{ scaleAndTranslate(DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f)), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)),  DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)),   DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f)),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f)),  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f)),   DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)),    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ scaleAndTranslate(DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f)),   DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
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

	vertexStride = sizeof(VertexPosColor);

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
	ibd.ByteWidth = sizeof indices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initDataI;
	ZeroMemory(&initDataI, sizeof(initDataI));
	initDataI.pSysMem = indices;
	if (FAILED(device->CreateBuffer(&ibd, &initDataI, indexBuffer.GetAddressOf()))) {
		MessageBox(0, L"init indices buffer failed...", 0, 0);
		return false;
	}

	indexCount = 36;

	// init rigid transform
	world.world = DirectX::XMMatrixIdentity();

	return true;
}

void Box::display(ID3D11DeviceContext* immediateContext) {
	// set vertice buffer
	UINT stride = vertexStride;
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// set indice buffer
	immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// get constant buffer
	ComPtr<ID3D11Buffer> wBuffer = nullptr;
	immediateContext->VSGetConstantBuffers(0, 1, wBuffer.GetAddressOf());

	// update world buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	immediateContext->Map(wBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(world), &world, sizeof(world));
	immediateContext->Unmap(wBuffer.Get(), 0); 

	// draw index
	immediateContext->DrawIndexed(indexCount, 0, 0);
}

DirectX::XMFLOAT3 Box::scaleAndTranslate(const DirectX::XMFLOAT3& f) const {
	return DirectX::XMFLOAT3(f.x * scale.x + pos.x,
							 f.y * scale.y + pos.y,
							 f.z * scale.z + pos.z);
}

DirectX::XMFLOAT3 Box::scaling(const DirectX::XMFLOAT3& f) const {
	return DirectX::XMFLOAT3(f.x * scale.x,
							 f.y * scale.y,
							 f.z * scale.z);
}