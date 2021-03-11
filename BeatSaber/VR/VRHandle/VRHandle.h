#pragma once
#define LEFT 0
#define RIGHT 1

#include "Main/Main.h"
#include "VR/VRCamera/VRCamera.h"

#ifndef OVR_ERROR_CHECK
#define OVR_ERROR_CHECK(result, msg) if (OVR_FAILURE(result)) { MessageBox(nullptr, (msg), "OculusVRGame", MB_ICONERROR | MB_OK); }
#endif

class VRHandle {
private:
	// シングルトン
	static VRHandle* m_Instance;
	VRHandle() {}
	~VRHandle() {}

	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new VRHandle();
			if (m_Instance)
			{
				return;
			}
		}
	}

	ovrSession								m_OVRSession = {};
	ovrGraphicsLuid							m_OVRLuId = {};
	ovrHmdDesc								m_OVRHmd = {};

	ovrTextureSwapChain						m_TextureChain[2] = { nullptr,nullptr };
	ovrTextureSwapChain						m_DepthTextureChain[2] = { nullptr,nullptr };
	std::vector<ID3D11RenderTargetView*>	m_TexRtv[2] = { {},{} };
	std::vector<ID3D11DepthStencilView*>	m_TexDsv[2] = { {},{} };

	ID3D11RenderTargetView*					m_PPRenderTargetView[2] = { {},{} };
	ID3D11ShaderResourceView*				m_PPShaderResourceView[2] = { {},{} };

	ovrRecti								m_EyeRenderViewport[2] = { {},{} };

	ovrMirrorTexture						m_MirrorTexture = nullptr;
	ovrMirrorTextureDesc					m_MirrorDesc = {};


	ovrSessionStatus						m_SessionStatus = {};


	ovrEyeRenderDesc						m_EyeRenderDesc[2] = { {}, {} };
	ovrPosef								m_EyeRenderPose[2] = { {}, {} };
	double									m_SensorSampleTime = 0;;// sensorSampleTime is fed into the layer later
	ovrTimewarpProjectionDesc				m_PosTimewarpProjectionDesc = {};
	long long								m_FrameIndex = 0;

	ovrLayerEyeFovDepth						m_Layer = {};

	D3D11_VIEWPORT							m_ViewPort[2] = { {}, {} };
	XMMATRIX								m_View[2] = { {}, {} };
	//XMMATRIX								m_Projection[2] = { {}, {} };
	ovrMatrix4f								m_Projection[2] = { {}, {} };

	ovrTrackingState						m_TrackingState = {};
	ovrInputState							m_InputState = {};

public:
	VRHandle(const VRHandle&) = delete;
	VRHandle operator=(const VRHandle&) = delete;
	VRHandle(VRHandle&&) = delete;
	VRHandle operator=(VRHandle&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static VRHandle* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}

	void							Start();	// libOVRとRiftの初期化。一番最初に呼び出す
	void							Init();		// 初期化
	void							Uninit();	// 解放
	void							Update();	// 更新

	void							EyeRenderBegin(int eye, VRCamera* mainCam);	// 描画始め
	void							PPRenderBegin(int eye, VRCamera* g_mainCam); // ブルーム描画始め
	void							EyeRenderCommit(int eye);					// 描画の適用
	void							EyeRenderEnd();								// 描画終了
	void							MirrorRender();								// ウィンドウズ画面への描画

	ovrSession*						GetOVRSession(void) { return &m_OVRSession; };	// Sessionの取得
	ovrGraphicsLuid*				GetOVRLuId(void) { return &m_OVRLuId; };		// LuIdの取得
	ovrHmdDesc*						GetOVRHmd(void) { return &m_OVRHmd; }			// HMDの取得
	ovrTextureSwapChain				GetTextureSwapChain(int eye) { return m_TextureChain[eye]; }

	ovrSessionStatus				GetState() { return m_SessionStatus; }			// Statusの取得
	ovrTrackingState*				GetTrackingState() { return &m_TrackingState; }	// トラッキング状態の取得
	ovrInputState*					GetInputState() { return &m_InputState; }		// 入力状態の取得

	XMMATRIX						GetViewMatrix(int eye) { return m_View[eye]; }	// ビューマトリックスの取得(引数は右目(0)か左目(0)か)
	//XMMATRIX						GetProjectionMatrix(int eye) { return m_Projection[eye]; }	// プロジェクションマトリックスの取得(引数は右目(0)か左目(0)か)
	ovrMatrix4f						GetProjectionMatrix(int eye) { return m_Projection[eye]; }	// プロジェクションマトリックスの取得(引数は右目(0)か左目(0)か)

	ID3D11ShaderResourceView*		GetPPTexture(int eye) { return m_PPShaderResourceView[eye]; }

	ID3D11RenderTargetView*			GetRTV(int eye);
	ID3D11DepthStencilView*			GetDSV(int eye);

	D3D11_VIEWPORT					GetViewPort(int eye) { return m_ViewPort[eye]; }

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(VRHandle), __alignof(VRHandle));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};