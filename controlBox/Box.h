#pragma once
#ifndef BOX_H
#define BOX_H

#include "Constant.h"
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

struct VertexPosColor {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

class Box {
public:
	Box();
	Box(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 s);

	bool init(ID3D11Device* device);
	virtual void display(ID3D11DeviceContext* immediateContext);

protected:
	virtual void initVertex(VertexPosColor vertices[]) const;
	DirectX::XMFLOAT3 scaleAndTranslate(const DirectX::XMFLOAT3& f) const;
	DirectX::XMFLOAT3 scaling(const DirectX::XMFLOAT3& f) const;

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	UINT vertexStride;
	UINT indexCount;
	WorldBuffer world;
		
};

class Car: public Box{
public:
	Car();
	Car(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 s);

	void display(ID3D11DeviceContext* immediateContext);

protected:
	void initVertex(VertexPosColor vertices[]) const;
};

#endif