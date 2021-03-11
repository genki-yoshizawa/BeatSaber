#pragma once

#include "Main/Main.h"
#include "GameObject/GameObject.h"

class MainSceneLife : public GameObject
{
private:
	int m_Life;					// �̗�

	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;		// �e�N�X�`���Z�b�g���邱�ƂŃt�F�[�h�̎��ɉ摜�łł���

	XMFLOAT3 m_Position;		// �ʒu
	XMVECTOR m_Rotation;		// ��]
	XMFLOAT3 m_Scale;			// �傫��

public:
	void Init() override;		// ������
	void Uninit() override;		// ���
	void Update() override {}	// �X�V
	void Draw() override;		// �`��

	// �������̗͂ɉ��Z����
	void AddLife(int add);
	// �������̗͂Ɍ��Z����
	void DecreaseLife(int decrease);
	int GetLife() { return m_Life; }	// �̗͂̎擾

private:
	const XMFLOAT3 POSITION = { -2.5f, -1.5f, -10.0f };		// �`�悷��ʒu
	const int START_LIFE = 50;								// �Q�[���n�܂�̗̑�
	const int MAX_LIFE = 100;								// �̗͂̍ő�l

public:
	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneLife), __alignof(MainSceneLife));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};