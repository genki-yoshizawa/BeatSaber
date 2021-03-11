#pragma once
#include "Main/Main.h"
#include "VR/VRCamera/VRCamera.h"
#include "Scene/Scene.h"
#include "Scene/TitleScene/TitleScene.h"
#include "Scene/MainScene/MainScene.h"
#include "Scene/ResultScene/ResultScene.h"

enum SCENE_CATEGORY
{
	SCENE_NONE,
	SCENE_TITLE,
	SCENE_MAIN,
	SCENE_RESULT,
};


class GameManager
{
private:
	// �V���O���g��
	static GameManager* m_Instance;
	GameManager(){}
	~GameManager(){}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new GameManager();
			if (m_Instance)
			{
				return;
			}
		}
	}

	Scene* m_Scene = nullptr;							// ���݂̃V�[��

	SCENE_CATEGORY m_Scene_Category = SCENE_NONE;		// ���݂̃V�[���̎��

public:	
	GameManager(const GameManager&) = delete;
	GameManager & operator=(const GameManager&) = delete;
	GameManager(GameManager&&) = delete;
	GameManager & operator=(GameManager&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static GameManager* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}

	void Init();	// ������
	void Uninit();	// ���
	void Update();	// �X�V
	void Draw();	// �`��

	void SetScene(SCENE_CATEGORY scene);	// �V�[���̃Z�b�g

	Scene* GetScene() { return m_Scene; }	// ���݃V�[���̎擾

	SCENE_CATEGORY GetSceneCategory() { return m_Scene_Category; }	// ���݂̃V�[���̎�ނ��擾
};

VRCamera* GetCamera();