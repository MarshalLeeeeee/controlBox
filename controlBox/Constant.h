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
	static const int width = 1280;
	static const int height = 960;
};

#endif