#pragma once
#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "model.h"

class SecondarySceneHand : public GameObject
{
private:
	XMFLOAT3 m_Position;			// 位置
	XMVECTOR m_Rotation;			// 回転
	XMFLOAT3 m_Scale;				// 大きさ
	XMFLOAT3 m_Forward;				// 正面方向
	CModel* m_GripModel;			// グリップモデル
	CModel* m_RayModel;				// 光線モデル
	CModel* m_WhenHitSphere;		// 球モデル
	bool m_isDrawSphere = false;	// 球モデルの描画フラグ
	XMFLOAT3 m_SpherePosition;		// 球の位置
	bool m_isRightHand = false;		// 右手であるかフラグ
	bool m_isEmitRay = false;		// 光線出力フラグ

	// 光線がなにかに衝突しているかの判定
	bool isHitRay();

public:
	SecondarySceneHand() = delete;
	SecondarySceneHand(bool isright_hand);
	~SecondarySceneHand() {}
	void Init() override;		// 初期化
	void Uninit() override;		// 解放
	void Update() override;		// 更新
	void Draw() override;		// 描画
	void SetEmitRay(bool emit) { m_isEmitRay = emit; }	// 光線出力フラグのセット
	bool GetEmitRay() { return m_isEmitRay; }			// 光線出力フラグの取得


	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(SecondarySceneHand), __alignof(SecondarySceneHand));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};