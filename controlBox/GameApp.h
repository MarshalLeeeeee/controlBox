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

enum BufferType {WORLD, VIEW, PROJ};

class GameApp : public D3dApp {
public:
	GameApp(HINSTANCE hInstance, int w, int h);
	~GameApp();

	bool init();
	void update();
	void display();

private:
	// buffer
	ComPtr<ID3D11Buffer> worldBuffer; // local to world
	ComPtr<ID3D11Buffer> viewBuffer; // world to view
	ComPtr<ID3D11Buffer> projBuffer; // view to proj
	WorldBuffer world;
	ViewBuffer view;
	ProjBuffer proj;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 drt;

	// shader
	ComPtr<ID3D11InputLayout> vertexLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;

	// objects
	float bound;
	std::vector<Box*> boxes;

	// update
	float dt;
	void forward();
	void backward();
	void turnRight();
	void turnLeft();
	void updateView();

	// init method
	bool initEffect();
	bool initResource();

	// math operation
	DirectX::XMVECTOR toVector(const DirectX::XMFLOAT3& f) const;
	void normalizeDrt();
};

#endif