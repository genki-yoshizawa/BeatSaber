#include "VRCamera.h"

XMMATRIX VRCamera::GetViewMatrix()
{
	XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), m_Rotation);	//íçéãì_
	return(XMMatrixLookAtRH(m_Position, XMVectorAdd(m_Position, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), m_Rotation)));
}