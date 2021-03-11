#include "MainSceneBomb.h"
#include "Scene/MainScene/MainScene.h"
#include "GameAudio/GameAudio.h"
#include "CreateMusicScore/CreateMusicScore.h"


void MainSceneBomb::Init()
{
	m_NotesModel = Scene::GetBomb();

	/* レーンの位置番号の早見表
		8  9 10 11
		4  5  6  7
		0  1  2  3
	*/
	// ボムはレーンの移動をしないためAppereLaneを基準とする

	// 初期値を設定
	m_Position = XMFLOAT3(MOST_LEFT + (m_AppereLane % 4) * 0.5f, OFFSET_Y + (m_AppereLane / 4) * 0.5f, NOTES_DRAW_START_Z);
	m_Rotation = XMQuaternionIdentity();
	m_Scale = XMFLOAT3(0.4f, 0.4f, 0.4f);

}

void MainSceneBomb::Update()
{
	// 判定位置z = -1.0(ここを基準にjsonファイルを作る)
	// ノーツが出現してからの経過時間を計算
	float passed_time = GameAudio::GetInstance()->GetPlaybackSecond(CreateMusicScore::GetInstance()->GetMusicScore()) - m_AppereTime;

	if (passed_time < 1.0f)
	{// 高速で動いている間
		m_Position.z = NOTES_DRAW_START_Z + NOTES_FAST_SPEED * passed_time;
	}
	else
	{// 低速での動き
		m_Position.z = NOTES_ON_LANE_Z + NOTES_SLOW_SPEED * (passed_time - 1.0f);
	}

	if (m_isHitSaber)
	{// 剣が当たっていれば
		m_Destroy = true;
		MainScene::GetInstance()->GetLife()->DecreaseLife(-15);
		GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_NOTES_MISS);
	}
	if (m_Position.z > 5.0f)
	{// 見逃した場合
		m_Destroy = true;
	}
}