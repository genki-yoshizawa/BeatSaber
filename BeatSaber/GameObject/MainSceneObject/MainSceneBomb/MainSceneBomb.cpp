#include "MainSceneBomb.h"
#include "Scene/MainScene/MainScene.h"
#include "GameAudio/GameAudio.h"
#include "CreateMusicScore/CreateMusicScore.h"


void MainSceneBomb::Init()
{
	m_NotesModel = Scene::GetBomb();

	/* ���[���̈ʒu�ԍ��̑����\
		8  9 10 11
		4  5  6  7
		0  1  2  3
	*/
	// �{���̓��[���̈ړ������Ȃ�����AppereLane����Ƃ���

	// �����l��ݒ�
	m_Position = XMFLOAT3(MOST_LEFT + (m_AppereLane % 4) * 0.5f, OFFSET_Y + (m_AppereLane / 4) * 0.5f, NOTES_DRAW_START_Z);
	m_Rotation = XMQuaternionIdentity();
	m_Scale = XMFLOAT3(0.4f, 0.4f, 0.4f);

}

void MainSceneBomb::Update()
{
	// ����ʒuz = -1.0(���������json�t�@�C�������)
	// �m�[�c���o�����Ă���̌o�ߎ��Ԃ��v�Z
	float passed_time = GameAudio::GetInstance()->GetPlaybackSecond(CreateMusicScore::GetInstance()->GetMusicScore()) - m_AppereTime;

	if (passed_time < 1.0f)
	{// �����œ����Ă����
		m_Position.z = NOTES_DRAW_START_Z + NOTES_FAST_SPEED * passed_time;
	}
	else
	{// �ᑬ�ł̓���
		m_Position.z = NOTES_ON_LANE_Z + NOTES_SLOW_SPEED * (passed_time - 1.0f);
	}

	if (m_isHitSaber)
	{// �����������Ă����
		m_Destroy = true;
		MainScene::GetInstance()->GetLife()->DecreaseLife(-15);
		GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
	}
	if (m_Position.z > 5.0f)
	{// ���������ꍇ
		m_Destroy = true;
	}
}