#pragma once
#include "Main/Main.h"
#include "Scene/Scene.h"

#include "GameObject/Board/Board.h"
#include "GameObject/MainSceneObject/MainSceneHand/MainSceneHand.h"
#include "GameObject/MainSceneObject/MainSceneNotes/MainSceneNotes.h"
#include "GameObject/MainSceneObject/MainSceneLife/MainSceneLife.h"

class MainScene : public Scene
{
private:
	// �V���O���g��
	static MainScene* m_Instance;
	MainScene() {}
	~MainScene() {}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new MainScene();
			if (m_Instance)
			{
				return;
			}
		}
	}

	MainSceneHand* m_LeftHand;					// ����
	MainSceneHand* m_RightHand;					// �E��
	MainSceneLife* m_Life;						// ���C�t
	Board* m_Board;								// �{�[�h
	std::list<MainSceneNotes*> m_NotesObject;	// �m�[�c
	bool m_Pause;								// �|�[�Y�t���O

public:
	MainScene(const MainScene&) = delete;
	MainScene operator=(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene operator=(MainScene&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static MainScene* GetInstance()
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


	void AddNotesObject(MainSceneNotes* note, int layer);					// �m�[�c�̒ǉ�
	std::list<MainSceneNotes*> GetNotesObject() { return m_NotesObject; }	// �m�[�c�̎擾
	MainSceneLife* GetLife() { return m_Life; }								// ���C�t�̎擾

};