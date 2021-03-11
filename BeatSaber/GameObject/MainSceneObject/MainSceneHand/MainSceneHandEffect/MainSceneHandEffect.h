#pragma once

#include "Main/Main.h"
#include "GameObject/GameObject.h"

class MainSceneHandEffect : public GameObject
{
private:
	static const int EFFECT_POLYGON_NUM = 10;	// エフェクトに使うポリゴン枚数
	XMFLOAT3 m_PointLog[2 * EFFECT_POLYGON_NUM + 2];		// 手の位置と剣の先端の記録(奇数が先端偶数が手の位置)

	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11Buffer*				m_IndexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;		
public:

	void Init(){}
	void Init(bool is_right, float length);
	void Uninit();
	void Update(){}
	void Update(XMFLOAT3 position, XMFLOAT3 forward, float length);
	void Draw();
};