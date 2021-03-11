#pragma once

#if _MSC_VER > 1600
#include "Main/Main.h"
#include "Renderer/Renderer.h"
#else
#include "xnamath.h"
#endif //_MSC_VER > 1600

// �G���[���b�Z�[�W�̃}�N����`
#ifndef FATALERROR
#define FATALERROR(msg) { MessageBoxA(nullptr, (msg), "OculusRoomTiny", MB_ICONERROR | MB_OK); exit(-1); }
#endif
//-----------------------------------------------------------
class VRCamera
{
private:
	XMVECTOR m_Position;	// �ʒu
	XMVECTOR m_Rotation;	// ��]

public:
	VRCamera() {}
	VRCamera(XMVECTOR* pos, XMVECTOR* rot) : m_Position(*pos), m_Rotation(*rot) {};
	VRCamera(const XMVECTOR& pos, const XMVECTOR& rot) : m_Position(pos), m_Rotation(rot) {};
	~VRCamera() {}

	XMMATRIX GetViewMatrix();						// �r���[�s��̎擾

	XMVECTOR GetPosition() { return m_Position; }	// �ʒu�̎擾

	XMVECTOR GetRotation() { return m_Rotation; }	// ��]�̎擾

	void MovePosition(XMVECTOR move) { m_Position = XMVectorAdd(m_Position, move); }	// �����̕������J�������ړ�����

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