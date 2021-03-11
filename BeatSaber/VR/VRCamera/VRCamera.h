#pragma once

#if _MSC_VER > 1600
#include "Main/Main.h"
#include "Renderer/Renderer.h"
#else
#include "xnamath.h"
#endif //_MSC_VER > 1600

// エラーメッセージのマクロ定義
#ifndef FATALERROR
#define FATALERROR(msg) { MessageBoxA(nullptr, (msg), "OculusRoomTiny", MB_ICONERROR | MB_OK); exit(-1); }
#endif
//-----------------------------------------------------------
class VRCamera
{
private:
	XMVECTOR m_Position;	// 位置
	XMVECTOR m_Rotation;	// 回転

public:
	VRCamera() {}
	VRCamera(XMVECTOR* pos, XMVECTOR* rot) : m_Position(*pos), m_Rotation(*rot) {};
	VRCamera(const XMVECTOR& pos, const XMVECTOR& rot) : m_Position(pos), m_Rotation(rot) {};
	~VRCamera() {}

	XMMATRIX GetViewMatrix();						// ビュー行列の取得

	XMVECTOR GetPosition() { return m_Position; }	// 位置の取得

	XMVECTOR GetRotation() { return m_Rotation; }	// 回転の取得

	void MovePosition(XMVECTOR move) { m_Position = XMVectorAdd(m_Position, move); }	// 引数の分だけカメラを移動する

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(VRCamera), __alignof(VRCamera));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};