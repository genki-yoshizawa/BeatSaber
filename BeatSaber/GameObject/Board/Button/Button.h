#pragma once
#include "Main/Main.h"
#include "Renderer/Renderer.h"

enum BUTTON_ACTION
{
	ACTION_NONE,

	ACTION_TEST,
	ACTION_SCENE_CHANGE_TO_MAIN,
	ACTION_SCENE_CHANGE_TO_TITLE,
};

class Button
{
private:
	XMFLOAT2 m_Position;	// �ʒu
	float m_Width;			// ����
	float m_Height;			// �c��
	static ID3D11Buffer* m_VertexBuffer;			// ���_�o�b�t�@
	ID3D11ShaderResourceView* m_Texture;			// �e�N�X�`��
	ID3D11ShaderResourceView* m_HandRayHitTexture;	// ���������Ă��Ă���Ƃ��̃e�N�X�`��
	bool m_isHandRayHit;	// ���������Ă��Ă��邩�t���O

public:
	Button() {}
	~Button() {}
	void Init(BUTTON_ACTION action, XMFLOAT2 position, float width, float height, const char* texture, const char* texture_ray_hit);	// ������
	void Uninit();												// ���
	void Update();												// �X�V
	void Draw(XMFLOAT3 board_pos, XMVECTOR board_rotation);		// �`��
	void WhenTriggerInput();									// �g���K�[���͂���������
	bool isCollisionHandRay(XMFLOAT2 ray_hit_position);			// �����ƏՓ˂��Ă��邩���ׂ�

	// �g���K�[���͂��ꂽ���̃{�^���̓����State�p�^�[���Ŏ���
	// ����֐���1�Ȃ̂Ŋ֐��|�C���^�Ŏ���(�ǐ����ǂ��Ȃ邩�̌��؂̈Ӑ}����)
private:
	// BUTTON_ACTION�ɑΉ������֐����L�q
	static void ActionTest(Button* const this_button);
	static void ActionSceneChangeToMain(Button* const this_button);
	static void ActionSceneChangeToTitle(Button* const this_button);

public:
	// ���s����֐��|�C���^
	void (*ButtonAction)(Button* const this_button);
};