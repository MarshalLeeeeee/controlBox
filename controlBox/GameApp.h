/*
* refer to
* https://www.cnblogs.com/X-Jun/p/9638817.html
* https://www.jianshu.com/p/d6607f0abd72
* https://github.com/MKXJun/DirectX11-With-Windows-SDK
*/

#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "Constant.h"
#include "D3dApp.h"
#include "Box.h"
#include <vector>

class GameApp : public D3dApp {
public:
	GameApp(HINSTANCE hInstance, int w, int h);
	~GameApp();

	void display();
	bool init();

private:
	// buffer
	ComPtr<ID3D11Buffer> worldBuffer;
	ComPtr<ID3D11Buffer> viewBuffer;
	ComPtr<ID3D11Buffer> projBuffer;
	WorldBuffer world;
	ViewBuffer view;
	ProjBuffer proj;

	// shader
	ComPtr<ID3D11InputLayout> vertexLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;

	// objects
	std::vector<Box> boxes;

	bool initEffect();
	bool initResource();
};

#endif