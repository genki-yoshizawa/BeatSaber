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
	// �V���O���g��
	static Fade* m_Instance;
	Fade(){}
	~Fade(){}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new Fade();
			if (m_Instance)
			{
				return;
			}
		}
	}

	//ID3D11Buffer*				m_VertexBuffer = nullptr;	// ���_�o�b�t�@
	ID3D11Buffer*				m_VertexBuffer[2] = { nullptr, nullptr };	// ���_�o�b�t�@
	ID3D11ShaderResourceView*	m_Texture = nullptr;		// �e�N�X�`���Z�b�g���邱�ƂŃt�F�[�h�̎��ɉ摜�łł���

	FADE_STATE m_Fade = FADE_STATE_NONE;		// �t�F�[�h���
	SCENE_CATEGORY m_NextScene = SCENE_NONE;	// ���̃V�[��
	float m_FadeAlpha = 0.0f;					// �t�F�[�h�A���t�@�l
	float m_AddAlpha = 1 / 20.0f;				// �t�F�[�h����ۂ�1f�ɉ����铧���x

	D3DXCOLOR m_fadecolor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// �t�F�[�h�F
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

	void Init(void);		// ������
	void Uninit(void);		// ���
	void Update(void);		// �X�V
	void Draw(int eye);		// �`��

	void SetFade(SCENE_CATEGORY scene);	// �t�F�[�h�J�n(�����Ɏ��̃V�[��������)

	FADE_STATE GetFadeState() { return m_Fade; }			//���݂�Fade�̏�Ԃ��擾����
};