#pragma once
#include "Main/Main.h"
#include "Scene/Scene.h"

#include "GameObject/Board/Board.h"
#include "GameObject/SecondarySceneObject/SecondarySceneHand/SecondarySceneHand.h"

class ResultScene : public Scene
{
private:
	// シングルトン
	static ResultScene* m_Instance;
	ResultScene() {}
	~ResultScene() {}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new ResultScene();
			if (m_Instance)
			{
				return;
			}
		}
	}

	SecondarySceneHand* m_LeftHand;		// 左手
	SecondarySceneHand* m_RightHand;	// 右手
	Board* m_Board;						// ボード

	bool m_GameClear = false;			// ゲームクリアフラグ

	// トリガー入力が行われた時
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

	void Init() override;		// 初期化
	void Uninit() override;		// 解放
	void Update() override;		// 更新


	void SetGameClear(bool clear) { m_GameClear = clear; }
	bool GetGameClear() { return m_GameClear; }
};