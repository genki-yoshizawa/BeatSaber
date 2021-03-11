#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define CREATE_INSTANCE_RETRY 5		// インスタンス生成のリトライ回数

#include <stdio.h>
#include <windows.h>
#include <assert.h>

#include <vector>



#pragma warning(push)
#pragma warning(disable:4005)

#include <windows.h>
#include <io.h>
#include <cstdint>
#include <vector>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <stdio.h>
#include <new>
#include <d3dx9.h>
#include <d3dx11.h>
#include <DirectXMath.h>

using namespace DirectX;

#pragma warning(pop)



#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")


// OculusVRのインクルード
#include <OVR_CAPI_D3D.h>
#include <Extras/OVR_CAPI_Util.h>

#pragma comment (lib, "LibOVR.lib")

// エラーチェックのマクロ定義
#ifndef ERROR_CHECK
#define ERROR_CHECK(result, msg) if(!result){ MessageBox(nullptr, (msg), "OculusVRGame", MB_ICONERROR | MB_OK); }
#endif

// clean up member COM pointers
template<typename T> void Release(T *&obj)
{
	if (!obj) return;
	obj->Release();
	obj = nullptr;
}


#define SCREEN_WIDTH	(960)			// ウインドウの幅
#define SCREEN_HEIGHT	(540)			// ウインドウの高さ

HWND GetWindow();
void MainLoopEnd(bool is_loop_end);	// メインループから脱出する関数

// XMFLOAT3に関する演算の関数がないので自分で実装
// XMFLOAT3の加算
XMFLOAT3 XMFloat3Add(XMFLOAT3 v1, XMFLOAT3 v2);
// XMFLOAT3の減算
XMFLOAT3 XMFloat3Suba(XMFLOAT3 v1, XMFLOAT3 v2);
// XMFLOAT3の絶対値
float XMFloat3Length(XMFLOAT3 v);
// XMFLOAT3の内積
float XMFloat3Dot(XMFLOAT3 v1, XMFLOAT3 v2);
// XMFLOAT3のスカラー値との積
XMFLOAT3 XMFloat3Dot(XMFLOAT3 v, float a);
// XMFLOAT3の単位ベクトル化
XMFLOAT3 XMFloat3Normalize(XMFLOAT3 v);
// XMFLOAT3の外積
XMFLOAT3 XMFloat3Cross(XMFLOAT3 v1, XMFLOAT3 v2);

// 最大値の取得
float FloatMax(float a, float b);
// 最小値の取得
float FloatMin(float a, float b);