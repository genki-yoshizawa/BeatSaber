#include "ResultScene.h"
#include "VR/VRHandle/VRHandle.h"
#include "GameManager/GameManager.h"
#include "Fade/Fade.h"

ResultScene* ResultScene::m_Instance = nullptr;

void ResultScene::Init() 
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

		Button* button1 = new Button();
		button1->Init(ACTION_SCENE_CHANGE_TO_MAIN, XMFLOAT2(-1.0f, 0.0f), 1.0f, 0.375f, "asset\\texture\\retry1.png", "asset\\texture\\retry2.png");
		m_Board->AddButton(button1);

		Button* button2 = new Button();
		button2->Init(ACTION_SCENE_CHANGE_TO_TITLE, XMFLOAT2(1.0f, 0.0f), 1.0f, 0.375f, "asset\\texture\\back_title1.png", "asset\\texture\\back_title2.png");
		m_Board->AddButton(button2);
	}
}

void ResultScene::Uninit()
{
	m_GameClear = false;
	Scene::Uninit();
}

void ResultScene::Update() 
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

	cur_trigger_left = VRHandle::GetInstance()->GetInputState()->IndexTriggerRaw[LEFT];
	cur_trigger_right = VRHandle::GetInstance()->GetInputState()->IndexTriggerRaw[RIGHT];
}

void ResultScene::WhenTriggerHand(SecondarySceneHand* hand)
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