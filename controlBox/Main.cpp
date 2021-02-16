/*
* refer to
* https://www.cnblogs.com/X-Jun/p/9638817.html
* https://www.jianshu.com/p/d6607f0abd72
* https://github.com/MKXJun/DirectX11-With-Windows-SDK
*/

#include "Constant.h"
#include "GameApp.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int showCmd) {
	int width = Constant::width;
	int height = Constant::height;

	GameApp app(hInstance, width, height);
	if (!app.init()) return 0;

	return app.run();
}