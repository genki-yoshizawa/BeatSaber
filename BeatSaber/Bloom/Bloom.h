#pragma once
#include "Main/Main.h"

class Bloom
{
	// シングルトン
	static Bloom* m_Instance;
	Bloom() {}
	~Bloom() {}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new Bloom();
			if (m_Instance)
			{
				return;
			}
		}
	}

	ID3D11Buffer*				m_VertexBuffer[2] = { nullptr, nullptr };	// 頂点バッファ
	ID3D11DepthStencilView*		m_DepthStencilView = nullptr; // デプスステンシルビュー
	//ID3D11ShaderResourceView*	m_BloomTexture = nullptr;		// マルチパスレンダリングによる描画結果のテクスチャ

	ID3D11VertexShader*     m_VertexShader = nullptr;					// 頂点シェーダ
	ID3D11PixelShader*      m_PixelShader = nullptr;					// ピクセルシェーダー
	ID3D11InputLayout*      m_VertexLayout = nullptr;					// インプットレイアウト
public:
	Bloom(const Bloom&) = delete;
	Bloom & operator=(const Bloom&) = delete;
	Bloom(Bloom&&) = delete;
	Bloom & operator=(Bloom&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static Bloom* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}

	void Init(void);		// 初期化
	void Uninit(void);		// 解放
	void Update(void);		// 更新
	void Draw(int eye);		// 描画
private:
	// 輝度抽出
	ID3D11RenderTargetView*					m_LuminanceRenderTargetView[2] = { {},{} };		// レンダーターゲット
	ID3D11ShaderResourceView*				m_LuminanceShaderResourceView[2] = { {},{} };	// テクスチャー
	ID3D11VertexShader*						m_LuminanceVertexShader = nullptr;				// 頂点シェーダ
	ID3D11PixelShader*						m_LuminancePixelShader = nullptr;				// ピクセルシェーダー
	ID3D11InputLayout*						m_LuminanceVertexLayout = nullptr;				// インプットレイアウト

	// X方向フィルター
	ID3D11RenderTargetView*					m_FilterXRenderTargetView[2] = { {},{} };		// レンダーターゲット
	ID3D11ShaderResourceView*				m_FilterXShaderResourceView[2] = { {},{} };		// テクスチャー
	ID3D11VertexShader*						m_FilterXVertexShader = nullptr;				// 頂点シェーダ
	ID3D11PixelShader*						m_FilterXPixelShader = nullptr;					// ピクセルシェーダー
	ID3D11InputLayout*						m_FilterXVertexLayout = nullptr;				// インプットレイアウト

	// Y方向フィルター
	ID3D11RenderTargetView*					m_FilterYRenderTargetView[2] = { {},{} };		// レンダーターゲット
	ID3D11ShaderResourceView*				m_FilterYShaderResourceView[2] = { {},{} };		// テクスチャー
	ID3D11VertexShader*						m_FilterYVertexShader = nullptr;				// 頂点シェーダ
	ID3D11PixelShader*						m_FilterYPixelShader = nullptr;					// ピクセルシェーダー
	ID3D11InputLayout*						m_FilterYVertexLayout = nullptr;				// インプットレイアウト

public:
	void Luminance(int eye);
	void Filter(int eye);
};