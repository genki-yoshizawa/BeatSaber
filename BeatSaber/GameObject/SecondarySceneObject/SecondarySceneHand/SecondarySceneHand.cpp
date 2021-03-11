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
	// ��̈ʒu�Ɗp�x���擾
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
	
	// ��̉�]�ʂ���������o�����������߂�
	m_Forward = XMFLOAT3(0.0f, 0.0f, -1.0f);	//�f�t�H���g�̕���
	XMVECTOR vec = XMVector3Rotate(XMLoadFloat3(&m_Forward), m_Rotation);	//XMVECTOR�Ōv�Z
	m_Forward = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));	//XMFLOAT3�ɕϊ�
	m_Forward = XMFloat3Normalize(m_Forward);	//�P�ʃx�N�g���ɂ���

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
	// ���̕`��
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_GripModel->Draw();

	if (m_isEmitRay)
	{
		XMFLOAT3 ray_scale = { 0.01f, 0.01f, 100000000.0f };

		// ���̕`��
		if (m_isDrawSphere)
		{
			modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(XMQuaternionIdentity()));
			modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_SpherePosition)));
			Renderer::GetInstance()->SetWorldMatrix(&modelMat);

			m_WhenHitSphere->Draw();

			XMFLOAT3 ray_length = { m_Position.x - m_SpherePosition.x, m_Position.y - m_SpherePosition.y, m_Position.z - m_SpherePosition.z };
			ray_scale = { 0.01f, 0.01f, XMFloat3Length(ray_length) };
		}

		// �����̕`��
		modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&ray_scale)), XMMatrixRotationQuaternion(m_Rotation));
		modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
		Renderer::GetInstance()->SetWorldMatrix(&modelMat);

		m_RayModel->Draw();
	}
}

bool SecondarySceneHand::isHitRay()
{
	Board* board = nullptr;

	// ���݃V�[���̃{�[�h�̎擾
	if (GameManager::GetInstance()->GetSceneCategory() == SCENE_TITLE) 
	{
		board = TitleScene::GetInstance()->GetGameObject<Board>();
	}
	else if (GameManager::GetInstance()->GetSceneCategory() == SCENE_RESULT)
	{
		board = ResultScene::GetInstance()->GetGameObject<Board>();
	}

	// �V�[�����Ƀ{�[�h���Ȃ����false��Ԃ�
	if(!board)
	{
		return false;
	}

	// �{�[�h�Ƃ̏Փ˔���̌��ʂ�Ԃ�
	return board->isCollisiontoHandRay(&m_Position, &m_Forward, &m_SpherePosition);
}