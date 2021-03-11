#include "SecondarySceneHand.h"
#include "VR/VRHandle/VRHandle.h"
#include "Renderer/Renderer.h"
#include "GameManager/GameManager.h"
#include "Scene/TitleScene/TitleScene.h"

SecondarySceneHand::SecondarySceneHand(bool isright_hand)
	:m_isRightHand(isright_hand)
{
	if(m_isRightHand)
	{
		m_isEmitRay = true;
	}
}

void SecondarySceneHand::Init()
{
	m_GripModel = Scene::GetGripModel(m_isRightHand);

	m_RayModel = new CModel();
	m_RayModel->Load("asset\\model\\ray.obj");

	m_WhenHitSphere = new CModel();
	m_WhenHitSphere->Load("asset\\model\\ray_sphere.obj");

	m_Scale = XMFLOAT3(0.03f, 0.03f, 0.03f);
}

void SecondarySceneHand::Uninit()
{
	m_WhenHitSphere->Unload();
	delete m_WhenHitSphere;

	m_RayModel->Unload();
	delete m_RayModel;
}

void SecondarySceneHand::Update()
{
	// 手の位置と角度を取得
	ovrPoseStatef hand_state;
	if (m_isRightHand) 
	{
		hand_state = VRHandle::GetInstance()->GetTrackingState()->HandPoses[RIGHT];
	}
	else
	{
		hand_state = VRHandle::GetInstance()->GetTrackingState()->HandPoses[LEFT];
	}

	m_Position = XMFLOAT3(hand_state.ThePose.Position.x, hand_state.ThePose.Position.y, hand_state.ThePose.Position.z);
	m_Rotation = XMVectorSet(hand_state.ThePose.Orientation.x, hand_state.ThePose.Orientation.y, hand_state.ThePose.Orientation.z, hand_state.ThePose.Orientation.w);
	
	// 手の回転量から光線を出す方向を決める
	m_Forward = XMFLOAT3(0.0f, 0.0f, -1.0f);	//デフォルトの方向
	XMVECTOR vec = XMVector3Rotate(XMLoadFloat3(&m_Forward), m_Rotation);	//XMVECTORで計算
	m_Forward = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));	//XMFLOAT3に変換
	m_Forward = XMFloat3Normalize(m_Forward);	//単位ベクトルにする

	if (m_isEmitRay && isHitRay())
	{
		m_isDrawSphere = true;
	}
	else
	{
		m_isDrawSphere = false;
	}
}

void SecondarySceneHand::Draw()
{
	// 柄の描画
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_GripModel->Draw();

	if (m_isEmitRay)
	{
		XMFLOAT3 ray_scale = { 0.01f, 0.01f, 100000000.0f };

		// 球の描画
		if (m_isDrawSphere)
		{
			modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(XMQuaternionIdentity()));
			modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_SpherePosition)));
			Renderer::GetInstance()->SetWorldMatrix(&modelMat);

			m_WhenHitSphere->Draw();

			XMFLOAT3 ray_length = { m_Position.x - m_SpherePosition.x, m_Position.y - m_SpherePosition.y, m_Position.z - m_SpherePosition.z };
			ray_scale = { 0.01f, 0.01f, XMFloat3Length(ray_length) };
		}

		// 光線の描画
		modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&ray_scale)), XMMatrixRotationQuaternion(m_Rotation));
		modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
		Renderer::GetInstance()->SetWorldMatrix(&modelMat);

		m_RayModel->Draw();
	}
}

bool SecondarySceneHand::isHitRay()
{
	Board* board = nullptr;

	// 現在シーンのボードの取得
	if (GameManager::GetInstance()->GetSceneCategory() == SCENE_TITLE) 
	{
		board = TitleScene::GetInstance()->GetGameObject<Board>();
	}
	else if (GameManager::GetInstance()->GetSceneCategory() == SCENE_RESULT)
	{
		board = ResultScene::GetInstance()->GetGameObject<Board>();
	}

	// シーン内にボードがなければfalseを返す
	if(!board)
	{
		return false;
	}

	// ボードとの衝突判定の結果を返す
	return board->isCollisiontoHandRay(&m_Position, &m_Forward, &m_SpherePosition);
}