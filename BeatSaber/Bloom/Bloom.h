#pragma once
#include "Main/Main.h"

class Bloom
{
	// �V���O���g��
	static Bloom* m_Instance;
	Bloom() {}
	~Bloom() {}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new Bloom();
			if (m_Instance)
			{
				return;
			}
		}
	}

	ID3D11Buffer*				m_VertexBuffer[2] = { nullptr, nullptr };	// ���_�o�b�t�@
	ID3D11DepthStencilView*		m_DepthStencilView = nullptr; // �f�v�X�X�e���V���r���[
	//ID3D11ShaderResourceView*	m_BloomTexture = nullptr;		// �}���`�p�X�����_�����O�ɂ��`�挋�ʂ̃e�N�X�`��

	ID3D11VertexShader*     m_VertexShader = nullptr;					// ���_�V�F�[�_
	ID3D11PixelShader*      m_PixelShader = nullptr;					// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*      m_VertexLayout = nullptr;					// �C���v�b�g���C�A�E�g
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

	void Init(void);		// ������
	void Uninit(void);		// ���
	void Update(void);		// �X�V
	void Draw(int eye);		// �`��
private:
	// �P�x���o
	ID3D11RenderTargetView*					m_LuminanceRenderTargetView[2] = { {},{} };		// �����_�[�^�[�Q�b�g
	ID3D11ShaderResourceView*				m_LuminanceShaderResourceView[2] = { {},{} };	// �e�N�X�`���[
	ID3D11VertexShader*						m_LuminanceVertexShader = nullptr;				// ���_�V�F�[�_
	ID3D11PixelShader*						m_LuminancePixelShader = nullptr;				// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*						m_LuminanceVertexLayout = nullptr;				// �C���v�b�g���C�A�E�g

	// X�����t�B���^�[
	ID3D11RenderTargetView*					m_FilterXRenderTargetView[2] = { {},{} };		// �����_�[�^�[�Q�b�g
	ID3D11ShaderResourceView*				m_FilterXShaderResourceView[2] = { {},{} };		// �e�N�X�`���[
	ID3D11VertexShader*						m_FilterXVertexShader = nullptr;				// ���_�V�F�[�_
	ID3D11PixelShader*						m_FilterXPixelShader = nullptr;					// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*						m_FilterXVertexLayout = nullptr;				// �C���v�b�g���C�A�E�g

	// Y�����t�B���^�[
	ID3D11RenderTargetView*					m_FilterYRenderTargetView[2] = { {},{} };		// �����_�[�^�[�Q�b�g
	ID3D11ShaderResourceView*				m_FilterYShaderResourceView[2] = { {},{} };		// �e�N�X�`���[
	ID3D11VertexShader*						m_FilterYVertexShader = nullptr;				// ���_�V�F�[�_
	ID3D11PixelShader*						m_FilterYPixelShader = nullptr;					// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*						m_FilterYVertexLayout = nullptr;				// �C���v�b�g���C�A�E�g

public:
	void Luminance(int eye);
	void Filter(int eye);
};