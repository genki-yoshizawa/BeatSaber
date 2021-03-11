#pragma once
#include "GameObject/GameObject.h"
#include "model.h"

class MainSceneBackGroundTable : public GameObject
{
private:
	XMFLOAT3 m_Position;		// 位置
	XMVECTOR m_Rotation;		// 回転
	XMFLOAT3 m_Scale;			// 大きさ

	CModel* m_TableModel;		// テーブルモデル

	ID3D11VertexShader*			m_VertexShader = nullptr;	// 頂点シェーダー
	ID3D11PixelShader*			m_PixelShader = nullptr;	// ピクセルシェーダー
	ID3D11InputLayout*			m_VertexLayout = nullptr;	// インプットレイアウト

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};