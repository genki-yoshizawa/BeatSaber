#include "Main/Main.h"
#include "Renderer/Renderer.h"
#include "VRHandle.h"

VRHandle* VRHandle::m_Instance = nullptr;

void VRHandle::Start()
{
	ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	OVR_ERROR_CHECK(result, "libOVR�̏������Ɏ��s���܂����B");

	result = ovr_Create(&m_OVRSession, &m_OVRLuId);
	OVR_ERROR_CHECK(result, "libOVR�̐����Ɏ��s���܂����B");

	m_OVRHmd = ovr_GetHmdDesc(m_OVRSession);
}

void VRHandle::Init()
{
	// �����Ȃ񂩂ɂ���Ƃ悢
	int sampleCount = 4;
	bool createDepth = true;

	for (int eye = 0; eye < 2; ++eye)
	{
		// �r���[�|�[�g�T�C�Y�̌v�Z
		ovrSizei idealSize = ovr_GetFovTextureSize(m_OVRSession, (ovrEyeType)eye, m_OVRHmd.DefaultEyeFov[eye], 1.0f);
		{
			ovrTextureSwapChainDesc desc = {};
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.Width = idealSize.w;
			desc.Height = idealSize.h;
			desc.MipLevels = 1;
			desc.SampleCount = sampleCount;
			desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.MiscFlags = ovrTextureMisc_DX_Typeless | ovrTextureMisc_AutoGenerateMips;
			desc.BindFlags = ovrTextureBind_DX_RenderTarget;
			desc.StaticImage = ovrFalse;

			ovrResult result = ovr_CreateTextureSwapChainDX(m_OVRSession, Renderer::GetInstance()->GetDevice(), &desc, &m_TextureChain[eye]);
			OVR_ERROR_CHECK(result, "VR�f�o�C�X�̃X���b�v�`�F�C���쐬�Ɏ��s���܂����B");

			// �e�ڂ̃����_�[�^�[�Q�b�g�r���[�쐬
			int textureCount = 0;
			ovr_GetTextureSwapChainLength(m_OVRSession, m_TextureChain[eye], &textureCount);
			for (int i = 0; i < textureCount; ++i)
			{
				{
					// �e�N�X�`���[�쐬
					ID3D11Texture2D* tex = nullptr;
					ovr_GetTextureSwapChainBufferDX(m_OVRSession, m_TextureChain[eye], i, IID_PPV_ARGS(&tex));

					// �����_�[�^�[�Q�b�g�r���[�쐬
					D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
					rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					rtvd.ViewDimension = (sampleCount > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
					ID3D11RenderTargetView* rtv;
					HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateRenderTargetView(tex, &rtvd, &rtv);
					ERROR_CHECK((hr == ERROR_SUCCESS), "VR�f�o�C�X�̃����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂����B");
					m_TexRtv[eye].push_back(rtv);

					tex->Release();
				}
			}
		}
		{// �|�X�g�v���Z�X�p
			// �e�N�X�`���[�쐬
			ID3D11Texture2D* ppTexture = nullptr;
			ovr_GetTextureSwapChainBufferDX(m_OVRSession, m_TextureChain[eye], 0, IID_PPV_ARGS(&ppTexture));

			//�|�X�g�v���Z�X�e�N�X�`���[�쐬
			D3D11_TEXTURE2D_DESC td;
			ZeroMemory(&td, sizeof(td));
			ppTexture->GetDesc(&td);
			td.BindFlags = td.BindFlags | D3D11_BIND_SHADER_RESOURCE;
			ppTexture = nullptr;
			Renderer::GetInstance()->GetDevice()->CreateTexture2D(&td, NULL, &ppTexture);

			//ID3D11Texture2D* ppTexture = nullptr;
			//D3D11_TEXTURE2D_DESC td;
			//ZeroMemory(&td, sizeof(td));
			//td.Width = idealSize.w;
			//td.Height = idealSize.h;
			//td.MipLevels = 1;
			//td.ArraySize = 1;
			//td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //DXGI_FORMAT_R8G8B8A8_TYPELESS; 
			//td.SampleDesc.Count = 1;//sd.SampleDesc;
			//td.Usage = D3D11_USAGE_DEFAULT;
			//td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			//td.CPUAccessFlags = 0;
			//td.MiscFlags = 0;
			//Renderer::GetInstance()->GetDevice()->CreateTexture2D(&td, NULL, &ppTexture);

			// �����_�[�^�[�Q�b�g�r���[�쐬
			D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
			memset(&rtvd, 0, sizeof(rtvd));
			rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R32G32B32A32_FLOAT;
			rtvd.ViewDimension = (sampleCount > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateRenderTargetView(ppTexture, &rtvd, &m_PPRenderTargetView[eye]);
			ERROR_CHECK((hr == ERROR_SUCCESS), "VR�f�o�C�X�̃����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂����B");

			// �|�X�g�v���Z�X�p�V�F�[�_�[���\�[�X�r���[�쐬
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = rtvd.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			SRVDesc.Texture2D.MipLevels = 1;
			Renderer::GetInstance()->GetDevice()->CreateShaderResourceView(ppTexture, &SRVDesc, &m_PPShaderResourceView[eye]);

			ppTexture->Release();
		}

		if (createDepth)
		{
			ovrTextureSwapChainDesc desc = {};
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.Width = idealSize.w;
			desc.Height = idealSize.h;
			desc.MipLevels = 1;
			desc.SampleCount = sampleCount;
			desc.Format = OVR_FORMAT_D32_FLOAT;
			desc.MiscFlags = ovrTextureMisc_None;
			desc.BindFlags = ovrTextureBind_DX_DepthStencil;
			desc.StaticImage = ovrFalse;

			ovrResult result = ovr_CreateTextureSwapChainDX(m_OVRSession, Renderer::GetInstance()->GetDevice(), &desc, &m_DepthTextureChain[eye]);
			if (!OVR_SUCCESS(result))
			{
				MessageBox(nullptr, "DepthTextureChain�̍쐬�Ɏ��s���܂����B", "OculusVRGame", MB_ICONERROR | MB_OK);
				break;
			}

			int textureCount = 0;
			ovr_GetTextureSwapChainLength(m_OVRSession, m_DepthTextureChain[eye], &textureCount);
			for (int i = 0; i < textureCount; ++i)
			{
				ID3D11Texture2D* tex = nullptr;
				ovr_GetTextureSwapChainBufferDX(m_OVRSession, m_DepthTextureChain[eye], i, IID_PPV_ARGS(&tex));

				D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				dsvDesc.ViewDimension = (sampleCount > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;

				ID3D11DepthStencilView* dsv;
				HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateDepthStencilView(tex, &dsvDesc, &dsv);
				ERROR_CHECK((hr == ERROR_SUCCESS), "VR�f�o�C�X�̃f�v�X�X�e���V���r���[�̍쐬�Ɏ��s���܂����B");
				m_TexDsv[eye].push_back(dsv);
				tex->Release();
			}

			m_EyeRenderViewport[eye].Pos.x = 0;
			m_EyeRenderViewport[eye].Pos.y = 0;
			m_EyeRenderViewport[eye].Size = idealSize;
			ERROR_CHECK(m_TextureChain[eye] && m_DepthTextureChain[eye], "�e�N�X�`���쐬�Ɏ��s���܂����B");
		}

		// �r���[�|�[�g�ݒ�
		m_ViewPort[eye].Width = (FLOAT)m_EyeRenderViewport[eye].Size.w;
		m_ViewPort[eye].Height = (FLOAT)m_EyeRenderViewport[eye].Size.h;
		m_ViewPort[eye].MinDepth = 0.0f;
		m_ViewPort[eye].MaxDepth = 1.0f;
		m_ViewPort[eye].TopLeftX = (FLOAT)m_EyeRenderViewport[eye].Pos.x;
		m_ViewPort[eye].TopLeftY = (FLOAT)m_EyeRenderViewport[eye].Pos.y;

	}

	// �~���[���j�^�[�i���j�^�[���HMD�ɕ`�悳��Ă�����e���\�������j�쐬
	m_MirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_MirrorDesc.Width = Renderer::GetInstance()->GetWinsizeW();
	m_MirrorDesc.Height = Renderer::GetInstance()->GetWinsizeH();
	m_MirrorDesc.MirrorOptions = ovrMirrorOption_Default;
	ovrResult result = ovr_CreateMirrorTextureWithOptionsDX(m_OVRSession, Renderer::GetInstance()->GetDevice(), &m_MirrorDesc, &m_MirrorTexture);
	OVR_ERROR_CHECK(result, "�~���[�e�N�X�`���̍쐬�Ɏ��s");

	// Fov���C���[�̏����ݒ�
	m_Layer.Header.Type = ovrLayerType_EyeFovDepth;
	m_Layer.Header.Flags = 0;

	// ���̍��������_�Ƃ���
	ovr_SetTrackingOriginType(m_OVRSession, ovrTrackingOrigin_FloorLevel);
}

void VRHandle::Uninit()
{
	if (m_MirrorTexture)
		ovr_DestroyMirrorTexture(m_OVRSession, m_MirrorTexture);

	for (int eye = 0; eye < 2; eye++) {
		for (int i = 0; i < (int)m_TexRtv[eye].size(); ++i)
		{
			Release(m_TexRtv[eye][i]);
		}
		for (int i = 0; i < (int)m_TexDsv[eye].size(); ++i)
		{
			Release(m_TexDsv[eye][i]);
		}
		Release(m_PPRenderTargetView[eye]);
		Release(m_PPShaderResourceView[eye]);
		if (m_TextureChain[eye])
		{
			ovr_DestroyTextureSwapChain(m_OVRSession, m_TextureChain[eye]);
		}
		if (m_DepthTextureChain[eye])
		{
			ovr_DestroyTextureSwapChain(m_OVRSession, m_DepthTextureChain[eye]);
		}
	}


	ovr_Destroy(m_OVRSession);
}

void VRHandle::Update()
{
	ovrResult result = ovr_GetSessionStatus(m_OVRSession, &m_SessionStatus);
	OVR_ERROR_CHECK(result, "VR�f�o�C�X�Ƃ̐ڑ����؂�܂����B");

	if (m_SessionStatus.ShouldQuit)
	{
		MainLoopEnd(true);
		return;
	}
	if (m_SessionStatus.ShouldRecenter) 
	{
		ovr_RecenterTrackingOrigin(m_OVRSession);
	}

	if (m_SessionStatus.IsVisible)
	{
		// �R���g���[���[�̓��͂̍X�V(��������)
		m_TrackingState = ovr_GetTrackingState(m_OVRSession, ovr_GetTimeInSeconds(), ovrTrue);
		ovrResult result = ovr_GetInputState(m_OVRSession, ovrControllerType_Touch, &m_InputState);
		OVR_ERROR_CHECK(result, "�R���g���[���[�̓��͂̍X�V�Ɏ��s���܂����A");

	}
}

void VRHandle::EyeRenderBegin(int eye, VRCamera* mainCam)
{
	if (!m_SessionStatus.IsVisible)
	{
		return;
	}
	ovr_WaitToBeginFrame(m_OVRSession, m_FrameIndex);

	////�|�[�Y����XMVECTOR�ϐ��ɃZ�b�g
	//XMVECTOR eyeQuat = XMVectorSet(m_EyeRenderPose[eye].Orientation.x, m_EyeRenderPose[eye].Orientation.y, m_EyeRenderPose[eye].Orientation.z, m_EyeRenderPose[eye].Orientation.w);
	//XMVECTOR eyePos = XMVectorSet(m_EyeRenderPose[eye].Position.x, m_EyeRenderPose[eye].Position.y, m_EyeRenderPose[eye].Position.z, 0);
	//
	//// ���C���J�����̈ʒu�Ɩڂ̈ʒu�����Z
	//XMVECTOR CombinedPos = XMVectorAdd(mainCam->GetPosition(), XMVector3Rotate(eyePos, mainCam->GetRotation()));
	//// ���C���J�����̉�]�Ɩڂ̉�]�����킹��
	//VRCamera finalCam(CombinedPos, XMQuaternionMultiply(eyeQuat, mainCam->GetRotation()));
	//Renderer::GetInstance()->SetCameraPosition(finalCam.GetPosition());
	//// �r���[�}�g���b�N�X�̎Z�o
	//XMMATRIX view = finalCam.GetViewMatrix();
	//Renderer::GetInstance()->SetViewMatrix(&view);
	//
	//// �v���W�F�N�V�����}�g���b�N�X�Z�o
	//m_Projection[eye] = ovrMatrix4f_Projection(m_EyeRenderDesc[eye].Fov, 0.02f, 200.0f, ovrProjection_None);
	//// �^�C�����[�v�̃v���W�F�N�V�����}�g���b�N�X�𒊏o
	//m_PosTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(m_Projection[eye], ovrProjection_None);
	//// XMMATRIX�ϐ��ɕϊ�
	//XMMATRIX projection = XMMatrixSet(m_Projection[eye].M[0][0], m_Projection[eye].M[1][0], m_Projection[eye].M[2][0], m_Projection[eye].M[3][0],
	//	m_Projection[eye].M[0][1], m_Projection[eye].M[1][1], m_Projection[eye].M[2][1], m_Projection[eye].M[3][1],
	//	m_Projection[eye].M[0][2], m_Projection[eye].M[1][2], m_Projection[eye].M[2][2], m_Projection[eye].M[3][2],
	//	m_Projection[eye].M[0][3], m_Projection[eye].M[1][3], m_Projection[eye].M[2][3], m_Projection[eye].M[3][3]);
	//Renderer::GetInstance()->SetProjectionMatrix(&projection);

	// �����_�[�^�[�Q�b�g�r���[�̃N���A
	Renderer::GetInstance()->Begin(GetRTV(eye), GetDSV(eye));
	Renderer::GetInstance()->GetDeviceContext()->RSSetViewports(1, &m_ViewPort[eye]);

	ovr_BeginFrame(m_OVRSession, m_FrameIndex);
}

void VRHandle::PPRenderBegin(int eye, VRCamera* mainCam)
{
	if (!m_SessionStatus.IsVisible)
	{
		return;
	}
	// �`��X�N���[���T�C�Y�̐ݒ�
	ovrSizei idealSize = ovr_GetFovTextureSize(m_OVRSession, (ovrEyeType)eye, m_OVRHmd.DefaultEyeFov[eye], 1.0f);
	Renderer::GetInstance()->SetScreen(idealSize.w, idealSize.h);

	m_EyeRenderDesc[0] = ovr_GetRenderDesc(m_OVRSession, ovrEye_Left, m_OVRHmd.DefaultEyeFov[0]);
	m_EyeRenderDesc[1] = ovr_GetRenderDesc(m_OVRSession, ovrEye_Right, m_OVRHmd.DefaultEyeFov[1]);

	// ���ڂ̃|�[�Y�̎擾
	ovrPosef HmdToEyePose[2] = { m_EyeRenderDesc[0].HmdToEyePose, m_EyeRenderDesc[1].HmdToEyePose };

	// �\������鎞�Ԃ�\�����ڂ̎p�����v�Z����
	m_SensorSampleTime = ovr_GetPredictedDisplayTime(m_OVRSession, m_FrameIndex);
	ovrTrackingState hmdState = ovr_GetTrackingState(m_OVRSession, m_SensorSampleTime, ovrTrue);
	ovr_CalcEyePoses(hmdState.HeadPose.ThePose, HmdToEyePose, m_EyeRenderPose);
	//ovr_WaitToBeginFrame(m_OVRSession, m_FrameIndex);
	//�|�[�Y����XMVECTOR�ϐ��ɃZ�b�g
	XMVECTOR eyeQuat = XMVectorSet(m_EyeRenderPose[eye].Orientation.x, m_EyeRenderPose[eye].Orientation.y, m_EyeRenderPose[eye].Orientation.z, m_EyeRenderPose[eye].Orientation.w);
	XMVECTOR eyePos = XMVectorSet(m_EyeRenderPose[eye].Position.x, m_EyeRenderPose[eye].Position.y, m_EyeRenderPose[eye].Position.z, 0);

	// ���C���J�����̈ʒu�Ɩڂ̈ʒu�����Z
	XMVECTOR CombinedPos = XMVectorAdd(mainCam->GetPosition(), XMVector3Rotate(eyePos, mainCam->GetRotation()));
	// ���C���J�����̉�]�Ɩڂ̉�]�����킹��
	VRCamera finalCam(CombinedPos, XMQuaternionMultiply(eyeQuat, mainCam->GetRotation()));
	Renderer::GetInstance()->SetCameraPosition(finalCam.GetPosition());
	// �r���[�}�g���b�N�X�̎Z�o
	m_View[eye] = finalCam.GetViewMatrix();
	Renderer::GetInstance()->SetViewMatrix(&m_View[eye]);

	// �v���W�F�N�V�����}�g���b�N�X�Z�o
	m_Projection[eye] = ovrMatrix4f_Projection(m_EyeRenderDesc[eye].Fov, 0.02f, 200.0f, ovrProjection_None);
	// �^�C�����[�v�̃v���W�F�N�V�����}�g���b�N�X�𒊏o
	m_PosTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(m_Projection[eye], ovrProjection_None);
	// XMMATRIX�ϐ��ɕϊ�
	XMMATRIX projection = XMMatrixSet(m_Projection[eye].M[0][0], m_Projection[eye].M[1][0], m_Projection[eye].M[2][0], m_Projection[eye].M[3][0],
		m_Projection[eye].M[0][1], m_Projection[eye].M[1][1], m_Projection[eye].M[2][1], m_Projection[eye].M[3][1],
		m_Projection[eye].M[0][2], m_Projection[eye].M[1][2], m_Projection[eye].M[2][2], m_Projection[eye].M[3][2],
		m_Projection[eye].M[0][3], m_Projection[eye].M[1][3], m_Projection[eye].M[2][3], m_Projection[eye].M[3][3]);
	Renderer::GetInstance()->SetProjectionMatrix(&projection);

	ID3D11DepthStencilView* depthStencil = GetDSV(eye);
	ID3D11RenderTargetView* ppRenderTargetView = m_PPRenderTargetView[eye];
	Renderer::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, &ppRenderTargetView, depthStencil);

	// �o�b�N�o�b�t�@�N���A
	float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	Renderer::GetInstance()->GetDeviceContext()->ClearRenderTargetView(ppRenderTargetView, ClearColor);
	Renderer::GetInstance()->GetDeviceContext()->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Renderer::GetInstance()->GetDeviceContext()->RSSetViewports(1, &m_ViewPort[eye]);
}

void VRHandle::EyeRenderCommit(int eye)
{
	ovr_CommitTextureSwapChain(m_OVRSession, m_TextureChain[eye]);
	ovr_CommitTextureSwapChain(m_OVRSession, m_DepthTextureChain[eye]);
}

void VRHandle::EyeRenderEnd()
{
	m_SensorSampleTime = ovr_GetPredictedDisplayTime(m_OVRSession, m_FrameIndex);
	m_Layer.ProjectionDesc = m_PosTimewarpProjectionDesc;
	m_Layer.SensorSampleTime = m_SensorSampleTime;

	for (int eye = 0; eye < 2; ++eye)
	{
		m_Layer.ColorTexture[eye] = m_TextureChain[eye];
		m_Layer.DepthTexture[eye] = m_DepthTextureChain[eye];
		m_Layer.Viewport[eye] = m_EyeRenderViewport[eye];
		m_Layer.Fov[eye] = m_OVRHmd.DefaultEyeFov[eye];
		m_Layer.RenderPose[eye] = m_EyeRenderPose[eye];
	}

	ovrLayerHeader* layers = &m_Layer.Header;
	// ���C���[����p���ė��ڂ̉�ʂɕ`��
	ovr_EndFrame(m_OVRSession, m_FrameIndex, nullptr, &layers, 1);

	m_FrameIndex++;
}

void VRHandle::MirrorRender()
{
	// �~���[�e�N�X�`����ID3D11Tecture2D�Ŏg����悤�ɕϊ�
	ID3D11Texture2D* tex = nullptr;
	ovrResult result = ovr_GetMirrorTextureBufferDX(m_OVRSession, m_MirrorTexture, IID_PPV_ARGS(&tex));
	OVR_ERROR_CHECK(result, "�~���[�e�N�X�`���̎擾�Ɏ��s���܂����B");

	// �~���[�e�N�X�`���̓��e�����̂܂܃E�B���h�E�Y��̉�ʂ̃����_�[�^�[�Q�b�g�r���[�ɕ`��
	Renderer::GetInstance()->GetDeviceContext()->CopyResource(Renderer::GetInstance()->GetRenderTargetBackBuffer(), tex);
	tex->Release();
}

ID3D11RenderTargetView* VRHandle::GetRTV(int eye)
{
	int index = 0;
	ovr_GetTextureSwapChainCurrentIndex(m_OVRSession, m_TextureChain[eye], &index);
	return m_TexRtv[eye][index];
}

ID3D11DepthStencilView* VRHandle::GetDSV(int eye)
{
	int index = 0;
	ovr_GetTextureSwapChainCurrentIndex(m_OVRSession, m_DepthTextureChain[eye], &index);
	return m_TexDsv[eye][index];
}
