#pragma once
#ifndef CONSTANT_H
#define CONSTAMT_H

#include <DirectXMath.h>

struct WorldBuffer {
	DirectX::XMMATRIX world;
};

struct ViewBuffer {
	DirectX::XMMATRIX view;
};

struct ProjBuffer {
	DirectX::XMMATRIX proj;
};

class Constant {
public:
	static const int width;
	static const int height;
	static const float dt;
	static const float bound;
};

#endif