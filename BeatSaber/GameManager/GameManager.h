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
	// シングルトン
	static GameManager* m_Instance;
	GameManager(){}
	~GameManager(){}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new GameManager();
			if (m_Instance)
			{
				return;
			}
		}
	}

	Scene* m_Scene = nullptr;							// 現在のシーン

	SCENE_CATEGORY m_Scene_Category = SCENE_NONE;		// 現在のシーンの種類

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

	void Init();	// 初期化
	void Uninit();	// 解放
	void Update();	// 更新
	void Draw();	// 描画

	void SetScene(SCENE_CATEGORY scene);	// シーンのセット

	Scene* GetScene() { return m_Scene; }	// 現在シーンの取得

	SCENE_CATEGORY GetSceneCategory() { return m_Scene_Category; }	// 現在のシーンの種類を取得
};

VRCamera* GetCamera();