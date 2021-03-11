#include "VR/VRHandle/VRHandle.h"
#include "GameManager.h"
#include "Renderer/Renderer.h"
#include "model.h"
//#include "sound.h"
#include "Bloom/Bloom.h"
#include "Fade/Fade.h"
#include "GameAudio/GameAudio.h"
#include "CreateMusicScore/CreateMusicScore.h"

GameManager* GameManager::m_Instance = nullptr;

static VRCamera*		g_mainCam;

void GameManager::Init()
{
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	// �����_���[������
	Renderer::GetInstance()->Init();

	// FloorLevel will give tracking poses where the floor height is 0
	// VR�f�o�C�X�̏�����
	VRHandle::GetInstance()->Init();

	// ���f���ǂݍ���
	Scene::LoadModel();

	// �I�[�f�B�I������
	GameAudio::GetInstance()->InitSound(GetWindow());

	// �u���[��������
	Bloom::GetInstance()->Init();

	// �t�F�[�h������
	Fade::GetInstance()->Init();

	// �e�C���X�^���X����
	TitleScene::GetInstance();
	MainScene::GetInstance();
	ResultScene::GetInstance();
	
	CreateMusicScore::GetInstance();

	// �V�[�����^�C�g���ɂ���
	GameManager::GetInstance()->SetScene(SCENE_TITLE);

	// Create camera
	g_mainCam = new VRCamera(XMVectorSet(0.0f, 0.0f, 0.0f, 0), XMQuaternionIdentity());

	GameAudio::GetInstance()->StopSound();
	//PlaySound(SOUND_LABEL_BGM_TITLE_BGM);

}

void GameManager::Uninit()
{
	delete g_mainCam;

	GameAudio::GetInstance()->UninitSound();
	GameAudio::Delete();

	m_Scene->Uninit();
	ResultScene::Delete();
	MainScene::Delete();
	TitleScene::Delete();

	Fade::Delete();
	Bloom::Delete();

	Scene::UnloadModel();

	Renderer::Delete();

	VRHandle::GetInstance()->Uninit();
	VRHandle::Delete();
}

void GameManager::Update()
{
	VRHandle::GetInstance()->Update();

	if (VRHandle::GetInstance()->GetState().IsVisible)
	{
		m_Scene->Update();

		XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -1.0f, 0), g_mainCam->GetRotation());
		XMVECTOR right = XMVector3Rotate(XMVectorSet(1.0f, 0, 0, 0), g_mainCam->GetRotation());

		g_mainCam->MovePosition((VRHandle::GetInstance()->GetInputState()->Thumbstick[1].y * forward) + (VRHandle::GetInstance()->GetInputState()->Thumbstick[1].x * right));

	}

	Bloom::GetInstance()->Update();

	Fade::GetInstance()->Update();
}

void GameManager::Draw()
{
	LIGHT light;
	light.Enable = false;
	light.Direction = D3DXVECTOR4(1.0f, -1.0f, 1.0f, 0.0f);
	D3DXVec4Normalize(&light.Direction, &light.Direction);
	light.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::GetInstance()->SetLight(light);

	for(int eye = 0; eye < 2; eye++)
	{
		VRHandle::GetInstance()->PPRenderBegin(eye, g_mainCam);
		Renderer::GetInstance()->SetUsualShader();
		m_Scene->Draw();

		// �P�x���o
		Bloom::GetInstance()->Luminance(eye);

		// �K�E�X�t�B���^�[
		Bloom::GetInstance()->Filter(eye);

		VRHandle::GetInstance()->EyeRenderBegin(eye, g_mainCam);

		Fade::GetInstance()->Draw(eye);
		Bloom::GetInstance()->Draw(eye);


		// �����_�����O���e���X���b�v�`�F�C���ɃR�~�b�g����
		VRHandle::GetInstance()->EyeRenderCommit(eye);
	}

	VRHandle::GetInstance()->EyeRenderEnd();

	// �~���[�`��
	VRHandle::GetInstance()->MirrorRender();
	Renderer::GetInstance()->End();
}

void GameManager::SetScene(SCENE_CATEGORY scene)
{
	if(scene == SCENE_NONE)
	{
		return;
	}

	if (m_Scene != nullptr)
	{
		m_Scene->Uninit();
	}

	switch(scene)
	{
	case  SCENE_TITLE:
		m_Scene = TitleScene::GetInstance();
		m_Scene_Category = SCENE_TITLE;
		break;

	case SCENE_MAIN:
		m_Scene = MainScene::GetInstance();
		m_Scene_Category = SCENE_MAIN;
		break;

	case SCENE_RESULT:
		m_Scene = ResultScene::GetInstance();
		m_Scene_Category = SCENE_RESULT;
		break;
	}
	m_Scene->Init();
}


VRCamera* GetCamera() { return g_mainCam; }