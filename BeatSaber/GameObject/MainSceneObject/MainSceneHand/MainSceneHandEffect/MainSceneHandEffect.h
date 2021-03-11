#pragma once

#include "Main/Main.h"
#include "GameObject/GameObject.h"

class MainSceneHandEffect : public GameObject
{
private:
	static const int EFFECT_POLYGON_NUM = 10;	// �G�t�F�N�g�Ɏg���|���S������
	XMFLOAT3 m_PointLog[2 * EFFECT_POLYGON_NUM + 2];		// ��̈ʒu�ƌ��̐�[�̋L�^(�����[��������̈ʒu)

	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11Buffer*				m_IndexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;		
public:

	void Init(){}
	void Init(bool is_right, float length);
	void Uninit();
	void Update(){}
	void Update(XMFLOAT3 position, XMFLOAT3 forward, float length);
	void Draw();
};