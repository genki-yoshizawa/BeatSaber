#include "MainSceneNotes.h"
#include "VR/VRHandle/VRHandle.h"
#include "Scene/MainScene/MainScene.h"
#include "GameAudio/GameAudio.h"
#include "CreateMusicScore/CreateMusicScore.h"

void MainSceneNotes::Init()
{
	// �m�[�c�^�C�v�ɍ��킹�ă��f�����Z�b�g����
	if (m_NotesType == 0)
	{
		m_NotesModel = Scene::GetNotesModelRed();
	}
	else if(m_NotesType == 1)
	{
		m_NotesModel = Scene::GetNotesModelBlue();
	}

	/* ���[���̈ʒu�ԍ��̑����\
		8  9 10 11
		4  5  6  7
		0  1  2  3
	*/

	// �����l��ݒ�
	m_Position = XMFLOAT3(MOST_LEFT + (m_AppereLane % 4) * 0.5f, OFFSET_Y + (m_AppereLane / 4) * 0.5f, NOTES_DRAW_START_Z);
	m_Rotation = XMQuaternionIdentity();
	m_Scale = XMFLOAT3(0.4f, 0.4f, 0.4f);

	m_StartPosition_x = m_Position.x;
	m_StartPosition_y = m_Position.y;
}
void MainSceneNotes::Uninit()
{
}
void MainSceneNotes::Update()
{
	// ����ʒuz = -1.0(���������json�t�@�C�������)
	// �m�[�c���o�����Ă���̌o�ߎ��Ԃ��v�Z
	float passed_time = GameAudio::GetInstance()->GetPlaybackSecond(CreateMusicScore::GetInstance()->GetMusicScore()) - m_AppereTime;

	if(passed_time < 1.0f)
	{// �����œ����Ă����
		m_Position.z = NOTES_DRAW_START_Z + NOTES_FAST_SPEED * passed_time;
	}
	else
	{// �ᑬ�ł̓���
		m_Position.z = NOTES_ON_LANE_Z + NOTES_SLOW_SPEED * (passed_time - 1.0f);

		// ��]����
		float move_rate = (passed_time - 1.0f) * 10.0f;	// ��]�ʋy�шړ��ʂ̔䗦�i0.0~1.0�j
		if(move_rate > 1.0f)
		{
			move_rate = 1.0f;
		}
		XMFLOAT3 axis = XMFLOAT3(0.0f, 0.0f, 1.0f);		//��]��
		m_Rotation = XMQuaternionRotationAxis(XMLoadFloat3(&axis), XM_PI * 0.25f * (m_Direction - 4) * sinf(move_rate * XM_PI * 0.5f));
		
		// xy���ʏ�ł̈ړ�
		m_Position.x = m_StartPosition_x + ((MOST_LEFT + (m_JudgeLane % 4) * 0.5f) - m_StartPosition_x) * sinf(move_rate * XM_PI * 0.5f);
		m_Position.y = m_StartPosition_y + ((OFFSET_Y + (m_JudgeLane / 4) * 0.5f) - m_StartPosition_y) * sinf(move_rate * XM_PI * 0.5f);
	}

	if(m_isHitSaber)
	{// ���Ɠ������Ă����
		m_Destroy = true;

		// �؂����ԈႦ�Ă�����
		if ((m_NotesType == 0 && m_isRightHit) || (m_NotesType == 1 && !m_isRightHit))
		{
			MainScene::GetInstance()->GetLife()->DecreaseLife(10);
			GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			return;
		}
		
		ovrVector3f hand_velocity = VRHandle::GetInstance()->GetTrackingState()->HandPoses[(int)m_isRightHit].LinearVelocity;

		//�����������B�Ȃ�Ƃ�������
		switch(m_Direction)
		{
		case 0:
			if(hand_velocity.y > 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 1:
			if (hand_velocity.y > 0.0f && hand_velocity.x < 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 2:
			if (hand_velocity.x < 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 3:
			if (hand_velocity.y < 0.0f && hand_velocity.x < 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 4:
			if (hand_velocity.y < 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 5:
			if (hand_velocity.y < 0.0f && hand_velocity.x > 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 6:
			if (hand_velocity.x > 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		case 7:
			if (hand_velocity.y > 0.0f && hand_velocity.x > 0.0f)
			{
				MainScene::GetInstance()->GetLife()->AddLife(1);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_HIT);
			}
			else
			{
				MainScene::GetInstance()->GetLife()->DecreaseLife(10);
				GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
			}
			break;

		default:
			break;
		}
	}

	if(m_Position.z > 5.0f)
	{
		m_Destroy = true; 
		MainScene::GetInstance()->GetLife()->DecreaseLife(15);
		GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
	}

}
void MainSceneNotes::Draw()
{
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);


	m_NotesModel->Draw();
}