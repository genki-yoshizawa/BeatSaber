#pragma once
#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "MainSceneHandEffect/MainSceneHandEffect.h"
#include "model.h"

class MainSceneHand : public GameObject
{
private:
	XMFLOAT3 m_Position;		// 位置
	XMVECTOR m_Rotation;		// 回転
	XMFLOAT3 m_Scale;			// 大きさ
	XMFLOAT3 m_Forward;			// 正面方向
	CModel* m_GripModel;
	CModel* m_SaberModel;
	float m_SaberLength;		// 剣の長さ
	bool m_isRightHand;			// 右手フラグ

	MainSceneHandEffect* m_Effect;	// エフェクト

	XMFLOAT3 m_NotePosition;	// ヒットしたノーツの位置

	bool isHitNotes(XMFLOAT3 cur_position, XMFLOAT3 cur_forward);							// ノーツに当たっている時
	bool isOnPolygon(XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3, XMFLOAT3 vec);	// 3頂点からの2つのベクトルを用いてvertex1+vecの点がポリゴン上の候補か調べる

public:
	MainSceneHand() = delete;
	MainSceneHand(bool isright_hand);
	~MainSceneHand(){}
	void Init() override;		// 初期化
	void Uninit() override;		// 解放
	void Update() override;		// 更新
	void Draw() override;		// 描画

	MainSceneHandEffect* GetMainSceneHandEffect() { return m_Effect; }

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneHand), __alignof(MainSceneHand));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};