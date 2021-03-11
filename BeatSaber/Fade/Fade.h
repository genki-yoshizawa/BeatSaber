#pragma once
#include "Main/Main.h"
#include "GameManager/GameManager.h"

enum FADE_STATE {
	FADE_STATE_NONE = 0,
	FADE_STATE_IN,
	FADE_STATE_OUT
};

class Fade
{
	// シングルトン
	static Fade* m_Instance;
	Fade(){}
	~Fade(){}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new Fade();
			if (m_Instance)
			{
				return;
			}
		}
	}

	//ID3D11Buffer*				m_VertexBuffer = nullptr;	// 頂点バッファ
	ID3D11Buffer*				m_VertexBuffer[2] = { nullptr, nullptr };	// 頂点バッファ
	ID3D11ShaderResourceView*	m_Texture = nullptr;		// テクスチャセットすることでフェードの時に画像でできる

	FADE_STATE m_Fade = FADE_STATE_NONE;		// フェード状態
	SCENE_CATEGORY m_NextScene = SCENE_NONE;	// 次のシーン
	float m_FadeAlpha = 0.0f;					// フェードアルファ値
	float m_AddAlpha = 1 / 20.0f;				// フェードする際に1fに加える透明度

	D3DXCOLOR m_fadecolor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// フェード色
public:
	Fade(const Fade&) = delete;
	Fade & operator=(const Fade&) = delete;
	Fade(Fade&&) = delete;
	Fade & operator=(Fade&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static Fade* GetInstance()
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

	void SetFade(SCENE_CATEGORY scene);	// フェード開始(引数に次のシーンを入れる)

	FADE_STATE GetFadeState() { return m_Fade; }			//現在のFadeの状態を取得する
};