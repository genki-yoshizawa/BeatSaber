#pragma once

#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "model.h"

class MainSceneNotes : public GameObject
{
protected:
	float m_AppereTime;	//�o�����ԁijson���Ŕ��莞�ԂŐݒ�ς݂̒l��ϊ������l�j
	float m_EndTime;	//�I�����ԁi�ǂ݂̂Ɏg���l�j
	int m_AppereLane;	//�o�����郌�[��
	int m_JudgeLane;	//���背�[��
	int m_NotesType;	//�E��m�[�c������m�[�c��
	int m_Direction;	//�������
	XMFLOAT3 m_Position;		// �ʒu
	float m_StartPosition_x;	// �o������x���W
	float m_StartPosition_y;	// �o������y���W
	XMVECTOR m_Rotation;		// ��]
	XMFLOAT3 m_Scale;			// �傫��
	bool m_isHitSaber;			// ���̏Փ˃t���O
	bool m_isRightHit;			// �Փ˂��������E�肾�������̃t���O
	CModel* m_NotesModel;		// �m�[�c���f��

	// �������������Ƃ��̏���
	virtual void HitSaber(){}

public:
	MainSceneNotes() =  delete;
	MainSceneNotes(float time, int appere_lane, int judge_lane, int notes_type, int direction)
		:m_AppereTime(time), m_AppereLane(appere_lane), m_JudgeLane(judge_lane), m_NotesType(notes_type), m_Direction(direction),m_EndTime(0.0f) {}
	~MainSceneNotes(){}
	virtual void Init() override;		// ������
	void Uninit() override;				// ���
	virtual void Update() override;		// �X�V
	virtual void Draw() override;		// �`��

	void SetHitSaber(bool is_right) { m_isHitSaber = true; m_isRightHit = is_right; }	// �����q�b�g������

	XMFLOAT3 GetPosition() { return m_Position; }		// �ʒu�̎擾
	XMFLOAT3 GetScale() { return m_Scale; }				// �傫���̎擾

	float GetAppereTime() { return m_AppereTime; }		// �o�����Ԃ̎擾
	int GetNotesType() { return m_NotesType; }			// �m�[�c�^�C�v�̎擾

protected:
	const float MOST_LEFT = -0.75;												//���[���̈�ԍ��̍��W
	const float NOTES_DRAW_START_Z = -166.0f;									//�m�[�c���o�Ă���ŏ���z���W
	const float NOTES_ON_LANE_Z = -16.0f;									 	//�m�[�c�̑��x�������鎞�i�o���G�t�F�N�g���o�����j��z���W
	const float NOTES_JUDGE_Z = -1.0f;											//���z�I�Ȕ����z���W�ijson�Ŏw�肷�鎞�Ԃ̈ʒu�j�i���łɐ؂�Ă���Ή���炷�j
	const float NOTES_FAST_SPEED = NOTES_ON_LANE_Z - NOTES_DRAW_START_Z;		//�m�[�c�̍ŏ���z���x�B���̊�1�b
	const float NOTES_SLOW_SPEED = (NOTES_JUDGE_Z - NOTES_ON_LANE_Z) / 1.25f;	//�m�[�c�̒x���Ȃ�������z���x�B���̊�1.25�b
	const float OFFSET_Y =0.65f;												//�m�[�c��y���ړ���(��ŃI�v�V�����Őݒ�ł���悤�ɂ���)


public:
	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneNotes), __alignof(MainSceneNotes));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}

};