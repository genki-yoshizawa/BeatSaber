#pragma once
#include "list"
#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "Placard/Placard.h"
#include "Button/Button.h"

class Board : public GameObject
{
private:
	XMFLOAT3 m_Position;			// 位置
	XMVECTOR m_Rotation;			// 回転
	float m_Width;					// 横幅
	float m_Height;					// 縦幅
	XMFLOAT2 m_RayHitPosition;		// 光線がヒットしている位置
	std::list<Button*> m_Button;	// ボタンオブジェクト
	std::list<Placard*> m_Placard;	// 張り紙オブジェクト

public:
	Board(){}
	~Board(){}
	void Init() override;	
	void Init(XMFLOAT3 position, XMVECTOR rotation , float width, float height);	// 初期化
	void Uninit() override;		// 解放
	void Update() override;		// 更新
	void Draw() override;		// 描画
	void WhenTriggerInput();	// トリガー入力された時
	bool isCollisiontoHandRay(const XMFLOAT3* InStartPosition,const XMFLOAT3* InVector , XMFLOAT3* OutPosition);	// 手からの光線とヒットしているか調べる

	void AddButton(Button* button){ m_Button.push_back(button); }	// ボタンの追加
	void AddPlacard(Button* button) { m_Button.push_back(button); }	// 張り紙の追加

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(Board), __alignof(Board));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};