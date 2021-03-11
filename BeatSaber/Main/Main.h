#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define CREATE_INSTANCE_RETRY 5		// �C���X�^���X�����̃��g���C��

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


// OculusVR�̃C���N���[�h
#include <OVR_CAPI_D3D.h>
#include <Extras/OVR_CAPI_Util.h>

#pragma comment (lib, "LibOVR.lib")

// �G���[�`�F�b�N�̃}�N����`
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


#define SCREEN_WIDTH	(960)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(540)			// �E�C���h�E�̍���

HWND GetWindow();
void MainLoopEnd(bool is_loop_end);	// ���C�����[�v����E�o����֐�

// XMFLOAT3�Ɋւ��鉉�Z�̊֐����Ȃ��̂Ŏ����Ŏ���
// XMFLOAT3�̉��Z
XMFLOAT3 XMFloat3Add(XMFLOAT3 v1, XMFLOAT3 v2);
// XMFLOAT3�̌��Z
XMFLOAT3 XMFloat3Suba(XMFLOAT3 v1, XMFLOAT3 v2);
// XMFLOAT3�̐�Βl
float XMFloat3Length(XMFLOAT3 v);
// XMFLOAT3�̓���
float XMFloat3Dot(XMFLOAT3 v1, XMFLOAT3 v2);
// XMFLOAT3�̃X�J���[�l�Ƃ̐�
XMFLOAT3 XMFloat3Dot(XMFLOAT3 v, float a);
// XMFLOAT3�̒P�ʃx�N�g����
XMFLOAT3 XMFloat3Normalize(XMFLOAT3 v);
// XMFLOAT3�̊O��
XMFLOAT3 XMFloat3Cross(XMFLOAT3 v1, XMFLOAT3 v2);

// �ő�l�̎擾
float FloatMax(float a, float b);
// �ŏ��l�̎擾
float FloatMin(float a, float b);