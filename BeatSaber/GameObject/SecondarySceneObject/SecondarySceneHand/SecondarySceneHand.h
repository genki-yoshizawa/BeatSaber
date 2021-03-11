#pragma once
#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "model.h"

class SecondarySceneHand : public GameObject
{
private:
	XMFLOAT3 m_Position;			// �ʒu
	XMVECTOR m_Rotation;			// ��]
	XMFLOAT3 m_Scale;				// �傫��
	XMFLOAT3 m_Forward;				// ���ʕ���
	CModel* m_GripModel;			// �O���b�v���f��
	CModel* m_RayModel;				// �������f��
	CModel* m_WhenHitSphere;		// �����f��
	bool m_isDrawSphere = false;	// �����f���̕`��t���O
	XMFLOAT3 m_SpherePosition;		// ���̈ʒu
	bool m_isRightHand = false;		// �E��ł��邩�t���O
	bool m_isEmitRay = false;		// �����o�̓t���O

	// �������Ȃɂ��ɏՓ˂��Ă��邩�̔���
	bool isHitRay();

public:
	SecondarySceneHand() = delete;
	SecondarySceneHand(bool isright_hand);
	~SecondarySceneHand() {}
	void Init() override;		// ������
	void Uninit() override;		// ���
	void Update() override;		// �X�V
	void Draw() override;		// �`��
	void SetEmitRay(bool emit) { m_isEmitRay = emit; }	// �����o�̓t���O�̃Z�b�g
	bool GetEmitRay() { return m_isEmitRay; }			// �����o�̓t���O�̎擾


	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(SecondarySceneHand), __alignof(SecondarySceneHand));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};