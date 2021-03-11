#pragma once

#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "model.h"

class MainSceneNotes : public GameObject
{
protected:
	float m_AppereTime;	//出現時間（json内で判定時間で設定済みの値を変換した値）
	float m_EndTime;	//終了時間（壁のみに使う値）
	int m_AppereLane;	//出現するレーン
	int m_JudgeLane;	//判定レーン
	int m_NotesType;	//右手ノーツか左手ノーツか
	int m_Direction;	//判定方向
	XMFLOAT3 m_Position;		// 位置
	float m_StartPosition_x;	// 出現時のx座標
	float m_StartPosition_y;	// 出現時のy座標
	XMVECTOR m_Rotation;		// 回転
	XMFLOAT3 m_Scale;			// 大きさ
	bool m_isHitSaber;			// 剣の衝突フラグ
	bool m_isRightHit;			// 衝突した剣が右手だったかのフラグ
	CModel* m_NotesModel;		// ノーツモデル

	// 剣が当たったときの処理
	virtual void HitSaber(){}

public:
	MainSceneNotes() =  delete;
	MainSceneNotes(float time, int appere_lane, int judge_lane, int notes_type, int direction)
		:m_AppereTime(time), m_AppereLane(appere_lane), m_JudgeLane(judge_lane), m_NotesType(notes_type), m_Direction(direction),m_EndTime(0.0f) {}
	~MainSceneNotes(){}
	virtual void Init() override;		// 初期化
	void Uninit() override;				// 解放
	virtual void Update() override;		// 更新
	virtual void Draw() override;		// 描画

	void SetHitSaber(bool is_right) { m_isHitSaber = true; m_isRightHit = is_right; }	// 剣がヒットした時

	XMFLOAT3 GetPosition() { return m_Position; }		// 位置の取得
	XMFLOAT3 GetScale() { return m_Scale; }				// 大きさの取得

	float GetAppereTime() { return m_AppereTime; }		// 出現時間の取得
	int GetNotesType() { return m_NotesType; }			// ノーツタイプの取得

protected:
	const float MOST_LEFT = -0.75;												//レーンの一番左の座標
	const float NOTES_DRAW_START_Z = -166.0f;									//ノーツが出てくる最初のz座標
	const float NOTES_ON_LANE_Z = -16.0f;									 	//ノーツの速度が落ちる時（出現エフェクトを出す時）のz座標
	const float NOTES_JUDGE_Z = -1.0f;											//理想的な判定のz座標（jsonで指定する時間の位置）（すでに切れていれば音を鳴らす）
	const float NOTES_FAST_SPEED = NOTES_ON_LANE_Z - NOTES_DRAW_START_Z;		//ノーツの最初のz速度。この間1秒
	const float NOTES_SLOW_SPEED = (NOTES_JUDGE_Z - NOTES_ON_LANE_Z) / 1.25f;	//ノーツの遅くなった時のz速度。この間1.25秒
	const float OFFSET_Y =0.65f;												//ノーツのy軸移動量(後でオプションで設定できるようにする)


public:
	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneNotes), __alignof(MainSceneNotes));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}

};