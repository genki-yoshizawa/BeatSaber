#pragma once

#include "Main/Main.h"
#include "GameObject/GameObject.h"

class MainSceneLife : public GameObject
{
private:
	int m_Life;					// 体力

	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;		// テクスチャセットすることでフェードの時に画像でできる

	XMFLOAT3 m_Position;		// 位置
	XMVECTOR m_Rotation;		// 回転
	XMFLOAT3 m_Scale;			// 大きさ

public:
	void Init() override;		// 初期化
	void Uninit() override;		// 解放
	void Update() override {}	// 更新
	void Draw() override;		// 描画

	// 引数分体力に加算する
	void AddLife(int add);
	// 引数分体力に減算する
	void DecreaseLife(int decrease);
	int GetLife() { return m_Life; }	// 体力の取得

private:
	const XMFLOAT3 POSITION = { -2.5f, -1.5f, -10.0f };		// 描画する位置
	const int START_LIFE = 50;								// ゲーム始まりの体力
	const int MAX_LIFE = 100;								// 体力の最大値

public:
	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneLife), __alignof(MainSceneLife));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};