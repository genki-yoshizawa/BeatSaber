#pragma once
#include "GameObject/GameObject.h"
#include "model.h"

class MainSceneBackGroundTable : public GameObject
{
private:
	XMFLOAT3 m_Position;		// �ʒu
	XMVECTOR m_Rotation;		// ��]
	XMFLOAT3 m_Scale;			// �傫��

	CModel* m_TableModel;		// �e�[�u�����f��

	ID3D11VertexShader*			m_VertexShader = nullptr;	// ���_�V�F�[�_�[
	ID3D11PixelShader*			m_PixelShader = nullptr;	// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*			m_VertexLayout = nullptr;	// �C���v�b�g���C�A�E�g

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};