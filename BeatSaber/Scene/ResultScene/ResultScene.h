#pragma once
#include "Main/Main.h"
#include "Scene/Scene.h"

#include "GameObject/Board/Board.h"
#include "GameObject/SecondarySceneObject/SecondarySceneHand/SecondarySceneHand.h"

class ResultScene : public Scene
{
private:
	// �V���O���g��
	static ResultScene* m_Instance;
	ResultScene() {}
	~ResultScene() {}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new ResultScene();
			if (m_Instance)
			{
				return;
			}
		}
	}

	SecondarySceneHand* m_LeftHand;		// ����
	SecondarySceneHand* m_RightHand;	// �E��
	Board* m_Board;						// �{�[�h

	bool m_GameClear = false;			// �Q�[���N���A�t���O

	// �g���K�[���͂��s��ꂽ��
	void WhenTriggerHand(SecondarySceneHand* hand);

public:
	ResultScene(const ResultScene&) = delete;
	ResultScene operator=(const ResultScene&) = delete;
	ResultScene(ResultScene&&) = delete;
	ResultScene operator=(ResultScene&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static ResultScene* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}

	void Init() override;		// ������
	void Uninit() override;		// ���
	void Update() override;		// �X�V


	void SetGameClear(bool clear) { m_GameClear = clear; }
	bool GetGameClear() { return m_GameClear; }
};