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
	// シングルトン
	static MainScene* m_Instance;
	MainScene() {}
	~MainScene() {}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new MainScene();
			if (m_Instance)
			{
				return;
			}
		}
	}

	MainSceneHand* m_LeftHand;					// 左手
	MainSceneHand* m_RightHand;					// 右手
	MainSceneLife* m_Life;						// ライフ
	Board* m_Board;								// ボード
	std::list<MainSceneNotes*> m_NotesObject;	// ノーツ
	bool m_Pause;								// ポーズフラグ

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

	void Init() override;		// 初期化
	void Uninit() override;		// 解放
	void Update() override;		// 更新


	void AddNotesObject(MainSceneNotes* note, int layer);					// ノーツの追加
	std::list<MainSceneNotes*> GetNotesObject() { return m_NotesObject; }	// ノーツの取得
	MainSceneLife* GetLife() { return m_Life; }								// ライフの取得

};