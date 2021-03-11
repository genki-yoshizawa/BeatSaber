#pragma once
#include "Main/Main.h"
#include "Renderer/Renderer.h"

enum BUTTON_ACTION
{
	ACTION_NONE,

	ACTION_TEST,
	ACTION_SCENE_CHANGE_TO_MAIN,
	ACTION_SCENE_CHANGE_TO_TITLE,
};

class Button
{
private:
	XMFLOAT2 m_Position;	// 位置
	float m_Width;			// 横幅
	float m_Height;			// 縦幅
	static ID3D11Buffer* m_VertexBuffer;			// 頂点バッファ
	ID3D11ShaderResourceView* m_Texture;			// テクスチャ
	ID3D11ShaderResourceView* m_HandRayHitTexture;	// 光線が当てられているときのテクスチャ
	bool m_isHandRayHit;	// 光線が当てられているかフラグ

public:
	Button() {}
	~Button() {}
	void Init(BUTTON_ACTION action, XMFLOAT2 position, float width, float height, const char* texture, const char* texture_ray_hit);	// 初期化
	void Uninit();												// 解放
	void Update();												// 更新
	void Draw(XMFLOAT3 board_pos, XMVECTOR board_rotation);		// 描画
	void WhenTriggerInput();									// トリガー入力があった時
	bool isCollisionHandRay(XMFLOAT2 ray_hit_position);			// 光線と衝突しているか調べる

	// トリガー入力された時のボタンの動作をStateパターンで実装
	// 今回関数が1つなので関数ポインタで実装(可読性がどうなるかの検証の意図あり)
private:
	// BUTTON_ACTIONに対応した関数を記述
	static void ActionTest(Button* const this_button);
	static void ActionSceneChangeToMain(Button* const this_button);
	static void ActionSceneChangeToTitle(Button* const this_button);

public:
	// 実行する関数ポインタ
	void (*ButtonAction)(Button* const this_button);
};