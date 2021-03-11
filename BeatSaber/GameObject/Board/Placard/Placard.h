#pragma once
#include "Main/Main.h"
#include "Renderer/Renderer.h"

class Placard
{
private:
	XMFLOAT2 m_Position;	// �ʒu
	float m_Width;			// ����
	float m_Height;			// �c��
	static ID3D11Buffer* m_VertexBuffer;	// ���_�o�b�t�@
	ID3D11ShaderResourceView* m_Texture;	// �e�N�X�`��

public:
	Placard(){}
	~Placard(){}
	void Init(XMFLOAT2 position, float width, float height, const char* texture);	// ������
	void Uninit();																	// ���
	void Update();																	// �X�V
	void Draw(XMFLOAT3 board_pos, XMVECTOR board_rotation);							// �`��
};