#pragma once
#include "list"
#include "Main/Main.h"
#include "GameObject/GameObject.h"
#include "Placard/Placard.h"
#include "Button/Button.h"

class Board : public GameObject
{
private:
	XMFLOAT3 m_Position;			// �ʒu
	XMVECTOR m_Rotation;			// ��]
	float m_Width;					// ����
	float m_Height;					// �c��
	XMFLOAT2 m_RayHitPosition;		// �������q�b�g���Ă���ʒu
	std::list<Button*> m_Button;	// �{�^���I�u�W�F�N�g
	std::list<Placard*> m_Placard;	// ���莆�I�u�W�F�N�g

public:
	Board(){}
	~Board(){}
	void Init() override;	
	void Init(XMFLOAT3 position, XMVECTOR rotation , float width, float height);	// ������
	void Uninit() override;		// ���
	void Update() override;		// �X�V
	void Draw() override;		// �`��
	void WhenTriggerInput();	// �g���K�[���͂��ꂽ��
	bool isCollisiontoHandRay(const XMFLOAT3* InStartPosition,const XMFLOAT3* InVector , XMFLOAT3* OutPosition);	// �肩��̌����ƃq�b�g���Ă��邩���ׂ�

	void AddButton(Button* button){ m_Button.push_back(button); }	// �{�^���̒ǉ�
	void AddPlacard(Button* button) { m_Button.push_back(button); }	// ���莆�̒ǉ�

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(Board), __alignof(Board));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};