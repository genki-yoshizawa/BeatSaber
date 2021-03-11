#pragma once
#include "Main/Main.h"
#include "Scene/Scene.h"

#include "GameObject/Board/Board.h"
#include "GameObject/SecondarySceneObject/SecondarySceneHand/SecondarySceneHand.h"

class TitleScene : public Scene
{
private:
	// �V���O���g��
	static TitleScene* m_Instance;
	TitleScene(){}
	~TitleScene(){}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new TitleScene();
			if (m_Instance)
			{
				return;
			}
		}
	}

	SecondarySceneHand* m_LeftHand;		// ����
	SecondarySceneHand* m_RightHand;	// �E��
	Board* m_Board;						// �{�[�h

	// �g���K�[���͂��s��ꂽ��
	void WhenTriggerHand(SecondarySceneHand* hand);

public:
	TitleScene(const TitleScene&) = delete;
	TitleScene operator=(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene operator=(TitleScene&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static TitleScene* GetInstance()
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
};