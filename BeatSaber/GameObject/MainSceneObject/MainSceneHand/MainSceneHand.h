#pragma once
#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "MainSceneHandEffect/MainSceneHandEffect.h"
#include "model.h"

class MainSceneHand : public GameObject
{
private:
	XMFLOAT3 m_Position;		// �ʒu
	XMVECTOR m_Rotation;		// ��]
	XMFLOAT3 m_Scale;			// �傫��
	XMFLOAT3 m_Forward;			// ���ʕ���
	CModel* m_GripModel;
	CModel* m_SaberModel;
	float m_SaberLength;		// ���̒���
	bool m_isRightHand;			// �E��t���O

	MainSceneHandEffect* m_Effect;	// �G�t�F�N�g

	XMFLOAT3 m_NotePosition;	// �q�b�g�����m�[�c�̈ʒu

	bool isHitNotes(XMFLOAT3 cur_position, XMFLOAT3 cur_forward);							// �m�[�c�ɓ������Ă��鎞
	bool isOnPolygon(XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3, XMFLOAT3 vec);	// 3���_�����2�̃x�N�g����p����vertex1+vec�̓_���|���S����̌�₩���ׂ�

public:
	MainSceneHand() = delete;
	MainSceneHand(bool isright_hand);
	~MainSceneHand(){}
	void Init() override;		// ������
	void Uninit() override;		// ���
	void Update() override;		// �X�V
	void Draw() override;		// �`��

	MainSceneHandEffect* GetMainSceneHandEffect() { return m_Effect; }

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneHand), __alignof(MainSceneHand));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};