#include "GameObject.h"

bool GameObject::Destroy()
{
	if (m_Destroy)
	{
		Uninit();
		delete this;
		return true;
	}
	else
	{
		return false;
	}
}