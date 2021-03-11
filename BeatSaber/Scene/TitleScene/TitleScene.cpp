#include "TitleScene.h"
#include "VR/VRHandle/VRHandle.h"
#include "GameManager/GameManager.h"
#include "Fade/Fade.h"

TitleScene* TitleScene::m_Instance = nullptr;

void TitleScene::Init()
{
	m_LeftHand = new SecondarySceneHand(/* isright_hand =*/false);
	m_LeftHand->Init();
	AddGameObject(m_LeftHand, 1);

	m_RightHand = new SecondarySceneHand(/* isright_hand =*/true);
	m_RightHand->Init();
	AddGameObject(m_RightHand, 1);

	{// ボードオブジェクト
		m_Board = new Board();
		m_Board->Init(XMFLOAT3(0.0f, 0.0f, -2.0f), XMQuaternionIdentity(), 8.0f, 5.0f);
		AddGameObject(m_Board, 1);

		Button* button = new Button();
		button->Init(ACTION_SCENE_CHANGE_TO_MAIN, XMFLOAT2(1.0f, 0.0f), 1.0f, 0.375f, "asset\\texture\\start_icon1.png", "asset\\texture\\start_icon2.png");
		m_Board->AddButton(button);
	}
}

void TitleScene::Uninit()
{
	Scene::Uninit();
}

void TitleScene::Update()
{
	static float cur_trigger_left;
	static float cur_trigger_right;

	Scene::Update();

	// この部分はProxyパターン?
	// トリガーチェック
	if (VRHandle::GetInstance()->GetInputState()->IndexTriggerRaw[LEFT] > 0.5f && cur_trigger_left <= 0.5f)
	{
		WhenTriggerHand(m_LeftHand);
		m_RightHand->SetEmitRay(false);

	}
	if (VRHandle::GetInstance()->GetInputState()->IndexTriggerRaw[RIGHT] > 0.5f && cur_trigger_right <= 0.5f)
	{
		WhenTriggerHand(m_RightHand);
		m_LeftHand->SetEmitRay(false);
	}

	cur_trigger_left= VRHandle::GetInstance()->GetInputState()->IndexTriggerRaw[LEFT];
	cur_trigger_right = VRHandle::GetInstance()->GetInputState()->IndexTriggerRaw[RIGHT];
}

void TitleScene::WhenTriggerHand(SecondarySceneHand* hand)
{
	if (!hand->GetEmitRay())
	{
		hand->SetEmitRay(true);
	}
	else
	{
		m_Board->WhenTriggerInput();
	}
}