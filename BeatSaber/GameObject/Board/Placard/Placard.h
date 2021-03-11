#pragma once
#include "Main/Main.h"
#include "Renderer/Renderer.h"

class Placard
{
private:
	XMFLOAT2 m_Position;	// 位置
	float m_Width;			// 横幅
	float m_Height;			// 縦幅
	static ID3D11Buffer* m_VertexBuffer;	// 頂点バッファ
	ID3D11ShaderResourceView* m_Texture;	// テクスチャ

public:
	Placard(){}
	~Placard(){}
	void Init(XMFLOAT2 position, float width, float height, const char* texture);	// 初期化
	void Uninit();																	// 解放
	void Update();																	// 更新
	void Draw(XMFLOAT3 board_pos, XMVECTOR board_rotation);							// 描画
};