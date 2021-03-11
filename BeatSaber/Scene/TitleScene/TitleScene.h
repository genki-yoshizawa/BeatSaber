#pragma once
#include "Main/Main.h"
#include "Scene/Scene.h"

#include "GameObject/Board/Board.h"
#include "GameObject/SecondarySceneObject/SecondarySceneHand/SecondarySceneHand.h"

class TitleScene : public Scene
{
private:
	// シングルトン
	static TitleScene* m_Instance;
	TitleScene(){}
	~TitleScene(){}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new TitleScene();
			if (m_Instance)
			{
				return;
			}
		}
	}

	SecondarySceneHand* m_LeftHand;		// 左手
	SecondarySceneHand* m_RightHand;	// 右手
	Board* m_Board;						// ボード

	// トリガー入力が行われた時
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

	void Init() override;		// 初期化
	void Uninit() override;		// 解放
	void Update() override;		// 更新
};