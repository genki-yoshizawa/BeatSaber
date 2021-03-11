#include "MainSceneWall.h"

#include "Scene/MainScene/MainScene.h"
#include "VR/VRHandle/VRHandle.h"
#include "GameAudio/GameAudio.h"
#include "CreateMusicScore/CreateMusicScore.h"

void MainSceneWall::Init()
{
	m_NotesModel = Scene::GetWall();

	/* レーンの位置番号の早見表
		8  9 10 11
		4  5  6  7
		0  1  2  3
	*/
	// 壁はレーンの移動をしないためAppereLaneを基準とする

	// 初期値を設定
	// 存在する時間を求める
	float exist_time = m_EndTime - m_AppereTime;

	m_Position = XMFLOAT3(MOST_LEFT + (m_AppereLane % 4) * 0.5f, OFFSET_Y + 0.7f + (m_AppereLane / 4) * 0.25f, NOTES_DRAW_START_Z - m_Scale.z * 0.5f);
	m_Rotation = XMQuaternionIdentity();
	m_Scale = XMFLOAT3(0.4f, 1.4f - (m_AppereLane / 4) * 0.5f, exist_time * NOTES_SLOW_SPEED );

	// レーンを超えた壁の幅をm_Directionを方向として設定する
	switch(m_Direction)
	{
	case 1/*左方向*/:
		m_Position.x -= m_AppereLane % 4 * 0.25f;
		m_Scale.x += m_AppereLane % 4 * 0.5f;
		break;

	case 2/*右方向*/:
		m_Position.x += (3 - (m_AppereLane % 4)) * 0.25f;
		m_Scale.x += (3 - (m_AppereLane % 4)) * 0.5f;
		break;

	case 0/*方向なし*/:
	default:
		break;
	}


}

void MainSceneWall::Update()
{
	// 判定位置z = -1.0(ここを基準にjsonファイルを作る)
	// ノーツが出現してからの経過時間を計算
	float passed_time = GameAudio::GetInstance()->GetPlaybackSecond(CreateMusicScore::GetInstance()->GetMusicScore()) - m_AppereTime;

	if (passed_time < 1.0f)
	{// 高速で動いている間
		m_Position.z = NOTES_DRAW_START_Z - m_Scale.z * 0.5f + NOTES_FAST_SPEED * passed_time;
	}
	else
	{// 低速での動き
		m_Position.z = NOTES_ON_LANE_Z - m_Scale.z * 0.5f + NOTES_SLOW_SPEED * (passed_time - 1.0f);
	}
	
	// 頭位置取得
	XMFLOAT3 head_pos = XMFLOAT3(VRHandle::GetInstance()->GetTrackingState()->HeadPose.ThePose.Position.x, VRHandle::GetInstance()->GetTrackingState()->HeadPose.ThePose.Position.y, VRHandle::GetInstance()->GetTrackingState()->HeadPose.ThePose.Position.z);

	if(head_pos.x > m_Position.x - m_Scale.x * 0.5f && head_pos.x < m_Position.x + m_Scale.x * 0.5f
		&&head_pos.y > m_Position.y - m_Scale.y * 0.5f && head_pos.y < m_Position.y + m_Scale.y * 0.5f
		&&head_pos.z > m_Position.z - m_Scale.z * 0.5f && head_pos.z < m_Position.z + m_Scale.z * 0.5f)
	{// 頭が壁の中にあれば
		MainScene::GetInstance()->GetLife()->DecreaseLife(2);
	}

	if (m_Position.z > 5.0f && passed_time > m_EndTime)
	{
		m_Destroy = true;
	}


}
void MainSceneWall::Draw()
{
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	// 両面カリングに設定
	Renderer::GetInstance()->SetCulling(D3D11_CULL_NONE);

	m_NotesModel->Draw();

	// カリング設定を戻す
	Renderer::GetInstance()->SetCulling(D3D11_CULL_BACK);
}