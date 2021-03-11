#pragma once
#include "GameObject/GameObject.h"
#include "model.h"

class MainSceneBackGroundLight : public GameObject
{
private:
	XMFLOAT3 m_PositionLeftLight;	// 左側表示モデル位置
	XMFLOAT3 m_PositionRightLight;	// 右側表示モデル位置
	XMVECTOR m_RotationLeftLight;	// 左側表示モデルの回転
	XMVECTOR m_RotationRightLight;	// 右側表示モデルの回転
	XMFLOAT3 m_Scale;			// 大きさ

	CModel* m_LightModel;		// ライトモデル

	ID3D11VertexShader*			m_VertexShader = nullptr;	// 頂点シェーダー
	ID3D11PixelShader*			m_PixelShader = nullptr;	// ピクセルシェーダー
	ID3D11InputLayout*			m_VertexLayout = nullptr;	// インプットレイアウト

	ID3D11Buffer*				m_PositionBuffer;	// ストラクチャードバッファ
	ID3D11ShaderResourceView*	m_PositionSRV;		// シェーダーリソースビュー

	ID3D11ShaderResourceView*	m_BlackTexture = nullptr;		// 光らないライトテクスチャ
	ID3D11Buffer*				m_isLightBuffer;				// 光るかのストラクチャードバッファ
	ID3D11ShaderResourceView*	m_isLightSRV;					// 光るかのbool型シェーダーリソースビュー

	BOOL						m_isLight[100];					// 光らせるフラグ
	int							m_FrameCount;					// フレームカウンタ
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};