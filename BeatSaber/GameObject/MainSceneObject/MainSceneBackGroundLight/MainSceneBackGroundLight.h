#pragma once
#include "GameObject/GameObject.h"
#include "model.h"

class MainSceneBackGroundLight : public GameObject
{
private:
	XMFLOAT3 m_PositionLeftLight;	// �����\�����f���ʒu
	XMFLOAT3 m_PositionRightLight;	// �E���\�����f���ʒu
	XMVECTOR m_RotationLeftLight;	// �����\�����f���̉�]
	XMVECTOR m_RotationRightLight;	// �E���\�����f���̉�]
	XMFLOAT3 m_Scale;			// �傫��

	CModel* m_LightModel;		// ���C�g���f��

	ID3D11VertexShader*			m_VertexShader = nullptr;	// ���_�V�F�[�_�[
	ID3D11PixelShader*			m_PixelShader = nullptr;	// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*			m_VertexLayout = nullptr;	// �C���v�b�g���C�A�E�g

	ID3D11Buffer*				m_PositionBuffer;	// �X�g���N�`���[�h�o�b�t�@
	ID3D11ShaderResourceView*	m_PositionSRV;		// �V�F�[�_�[���\�[�X�r���[

	ID3D11ShaderResourceView*	m_BlackTexture = nullptr;		// ����Ȃ����C�g�e�N�X�`��
	ID3D11Buffer*				m_isLightBuffer;				// ���邩�̃X�g���N�`���[�h�o�b�t�@
	ID3D11ShaderResourceView*	m_isLightSRV;					// ���邩��bool�^�V�F�[�_�[���\�[�X�r���[

	BOOL						m_isLight[100];					// ���点��t���O
	int							m_FrameCount;					// �t���[���J�E���^
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};