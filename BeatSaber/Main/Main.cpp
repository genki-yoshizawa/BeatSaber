#include "VR/VRCamera/VRCamera.h"
#include "VR/VRHandle/VRHandle.h"

#include "Renderer/Renderer.h"
#include "GameManager/GameManager.h"
#include "model.h"
#include "cube.h"

//------------------------------------------------------------
// ovrSwapTextureSet wrapper class that also maintains the render target views
// needed for D3D11 rendering.

LPCWSTR CLASS_NAME = L"AppClass";
LPCWSTR WINDOW_NAME = L"OculusVRGame";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitWindow(HINSTANCE hinst);

bool HandleMessages(void);

HWND g_Window;
bool g_isLoopEnd = false;

HWND GetWindow()
{
	return g_Window;
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	InitWindow(hinst);

	VRHandle::GetInstance()->Start();

	GameManager::GetInstance()->Init();

	while (1)
	{
		if (!HandleMessages())
		{
			break;
		}
	}

	GameManager::GetInstance()->Uninit();

	DestroyWindow(g_Window);
	g_Window = nullptr;
	UnregisterClassW(CLASS_NAME, hinst);

	ovr_Shutdown();
	return(0);
}

void InitWindow(HINSTANCE hinst)
{
	WNDCLASSW wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbWndExtra = 0;
	RegisterClassW(&wc);
	// adjust the window size and show at InitDevice time
	g_Window = CreateWindowW(CLASS_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, hinst, 0);
	if (!g_Window) 
	{
		MessageBox(nullptr, "windowの初期化に失敗しました。", "OculusVRGame", MB_ICONERROR | MB_OK);
		exit(-1);
	}
}

bool HandleMessages(void)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{// PostQuitMessage()が呼ばれたらループ終了
			return false;
		}
		else
		{
			// メッセージの翻訳とディスパッチ
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
	{
		GameManager::GetInstance()->Update();
		if(g_isLoopEnd)
		{
			return false;
		}

		GameManager::GetInstance()->Draw();
	}

	return true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void MainLoopEnd(bool is_loop_end)
{
	g_isLoopEnd = is_loop_end;
}

XMFLOAT3 XMFloat3Add(XMFLOAT3 v1, XMFLOAT3 v2)
{
	return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

XMFLOAT3 XMFloat3Suba(XMFLOAT3 v1, XMFLOAT3 v2)
{
	return XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

float XMFloat3Dot(XMFLOAT3 v1, XMFLOAT3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

XMFLOAT3 XMFloat3Dot(XMFLOAT3 v, float a)
{
	return XMFLOAT3(v.x * a, v.y * a, v.z * a);
}

float XMFloat3Length(XMFLOAT3 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

XMFLOAT3 XMFloat3Normalize(XMFLOAT3 v)
{
	float len = XMFloat3Length(v);

	return XMFLOAT3(v.x / len, v.y / len, v.z / len);
}

XMFLOAT3 XMFloat3Cross(XMFLOAT3 v1, XMFLOAT3 v2)
{
	XMFLOAT3 cross;
	cross.x = v1.y * v2.z - v1.z * v2.y;
	cross.y = v1.z * v2.x - v1.x * v2.z;
	cross.z = v1.x * v2.y - v1.y * v2.x;

	return cross;
}

float FloatMax(float a, float b)
{
	return a > b ? a : b;
}

float FloatMin(float a, float b)
{
	return a > b ? b : a;
}