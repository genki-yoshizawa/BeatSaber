#include "Board.h"

#include "GameManager/GameManager.h"

void Board::Init()
{
	m_Position = XMFLOAT3(0.0f, 0.0f, -2.0f);
	
	m_Rotation = XMQuaternionIdentity();

	float m_Width = 2.0f;
	float m_Height = 1.0f;

}

void Board::Init(XMFLOAT3 position, XMVECTOR rotation, float width, float height)
{
	m_Position = position;

	m_Rotation = rotation;

	m_Width = width;
	m_Height = height;
}

void Board::Uninit()
{
	for (Placard* placard : m_Placard)
	{
		placard->Uninit();
	}
	for (Button* button : m_Button)
	{
		button->Uninit();
	}
}

void Board::Update()
{
	for (Placard* placard : m_Placard)
	{
		placard->Update();
	}
	for (Button* button : m_Button)
	{
		button->Update();
	}
}

void Board::Draw()
{
	for (Placard* placard : m_Placard)
	{
		placard->Draw(m_Position, m_Rotation);
	}
	for (Button* button : m_Button)
	{
		button->Draw(m_Position, m_Rotation);
	}
}

void Board::WhenTriggerInput()
{
	for (Button* button : m_Button)
	{
		button->WhenTriggerInput();
	}
}

bool Board::isCollisiontoHandRay(const XMFLOAT3* InStartPosition, const XMFLOAT3* InVector, XMFLOAT3* OutPosition)
{
	// ボードの法線ベクトルを求める
	XMFLOAT3 normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMVECTOR vec = XMVector3Rotate(XMLoadFloat3(&normal), m_Rotation);
	normal = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));

	normal = XMFloat3Normalize(normal);

	// ボードと光線の交点を求める
	// 法線ベクトルと光線のベクトルの内積
	float ray_normal_dot = XMFloat3Dot(normal, *InVector);
		
	// 内積が0のときは平行、正のときはベクトルが同じ方向(交わっているのは始点より後ろの位置)
	if(ray_normal_dot >= 0.0f)
	{
		OutPosition = nullptr;
		return false;
	}

	//半直線の式(InStartPosition + t * InVector)の媒介変数tを求める
	// t * (normal・InVector) = normal・(m_Position - InStartPosition)
	float t = XMFloat3Dot(normal, XMFloat3Suba(m_Position, *InStartPosition));
	t = t / ray_normal_dot;
	//tより交点の位置を求める
	*OutPosition = XMFLOAT3(InStartPosition->x + t * InVector->x, InStartPosition->y + t * InVector->y, InStartPosition->z + t * InVector->z);


	// 衝突位置が板の範囲にあるか調べる

	// 右方向のベクトルの変換を求める
	XMFLOAT3 x_axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vec = XMVector3Rotate(XMLoadFloat3(&x_axis), m_Rotation);
	x_axis = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));
	// 上方向のベクトルの変換を求める
	XMFLOAT3 y_axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vec = XMVector3Rotate(XMLoadFloat3(&y_axis), m_Rotation);
	y_axis = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));
	
	// 板上での板中心から衝突点のベクトル
	XMFLOAT2 to_hit = XMFLOAT2(OutPosition->x - m_Position.x, OutPosition->y - m_Position.y);

	if(!( (to_hit.x > -x_axis.x * m_Width * 0.5f + -y_axis.x * m_Height * 0.5f 
		&& to_hit.x < x_axis.x * m_Width * 0.5f + y_axis.x * m_Height * 0.5f)
		&& (to_hit.y > -y_axis.y * m_Height * 0.5f + -x_axis.y * m_Width * 0.5f
		&& to_hit.y < y_axis.y * m_Height * 0.5f + x_axis.y * m_Width * 0.5f)))
	{
		// 板の範囲にいなかった(!演算子ついてることに注意)
		OutPosition = nullptr;
		return false;
	}

	// 板の範囲内にあればButtonに位置情報を渡す
	m_RayHitPosition.x = XMFloat3Dot(x_axis, XMFloat3Suba(*OutPosition, m_Position));
	m_RayHitPosition.y = XMFloat3Dot(y_axis, XMFloat3Suba(*OutPosition, m_Position));

	for (Button* button : m_Button)
	{
		if (button->isCollisionHandRay(m_RayHitPosition))
		{
			break;
		}
	}

	return true;
}