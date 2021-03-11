#include "MainScene.h"
#include "GameObject/MainSceneObject/MainSceneNotes/MainSceneNotes.h"
#include "GameObject/MainSceneObject/MainSceneBackGroundTable/MainSceneBackGroundTable.h"
#include "GameObject/MainSceneObject/MainSceneBackGroundLight/MainSceneBackGroundLight.h"
#include "CreateMusicScore/CreateMusicScore.h"
#include "Scene/Scene.h"
#include "Scene/ResultScene/ResultScene.h"
#include "Fade/Fade.h"

MainScene* MainScene::m_Instance = nullptr;

void MainScene::Init() 
{
	m_LeftHand = new MainSceneHand(/* isright_hand =*/false);
	m_LeftHand->Init();
	AddGameObject(m_LeftHand, 1);

	m_RightHand = new MainSceneHand(/* isright_hand =*/true);
	m_RightHand->Init();
	AddGameObject(m_RightHand, 1);

	m_Life = new MainSceneLife();
	m_Life->Init();
	AddGameObject(m_Life, 1);

	AddGameObject<MainSceneBackGroundTable>(1);
	AddGameObject<MainSceneBackGroundLight>(1);

	AddGameObject(m_LeftHand->GetMainSceneHandEffect(), 2);
	AddGameObject(m_RightHand->GetMainSceneHandEffect(), 2);

	//m_Board = new Board();
	//m_Board->Init(XMFLOAT3(0.0f, 0.0f, -2.0f), XMQuaternionIdentity(), 8.0f, 5.0f);
	//AddGameObject(m_Board, 1);

	CreateMusicScore::GetInstance()->Init();
}

void MainScene::Uninit() 
{
	Scene::Uninit();

	CreateMusicScore::GetInstance()->Uninit();
}

void MainScene::Update() 
{
	Scene::Update();

	// Destroyフラグの立っているノーツオブジェクトを除去する
	m_NotesObject.remove_if([](MainSceneNotes* notes) { return notes->GetDestroy(); });

	CreateMusicScore::GetInstance()->Update();

	if(GameAudio::GetInstance()->isFinishSound(CreateMusicScore::GetInstance()->GetMusicScore()))
	{// 現在再生されている曲が終わりに達していれば
		GameAudio::GetInstance()->StopSound();
		ResultScene::GetInstance()->SetGameClear(true);
		Fade::GetInstance()->SetFade(SCENE_RESULT);
	}

	if(m_Life->GetLife() <= 0)
	{//ライフが尽きれば
		ResultScene::GetInstance()->SetGameClear(false);
		//Fade::GetInstance()->SetFade(SCENE_RESULT);
	}
}

void MainScene::AddNotesObject(MainSceneNotes* note, int layer)
{
	AddGameObject(note, layer);
	m_NotesObject.push_back(note);
}