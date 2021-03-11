#pragma once

class GameObject
{
protected:
	bool m_Destroy = false;		// 破壊フラグ

public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Init() = 0;	// 初期化
	virtual void Uninit() = 0;	// 解放
	virtual void Update() = 0;	// 更新
	virtual void Draw() = 0;	// 描画

	void SetDestroy() { m_Destroy = true; }	// 破壊フラグを立てる
	bool GetDestroy() { return m_Destroy; }	// 破壊フラグの取得
	bool Destroy();							// 破壊処理
};