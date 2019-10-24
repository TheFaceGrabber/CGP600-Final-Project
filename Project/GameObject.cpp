#include "GameObject.h"
#include "Component.h"
#include "Direct3D.h"

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

GameObject::GameObject(std::string name)
{
	m_name = name;
	m_scale = { 1,1,1 };
}

GameObject::~GameObject()
{
	for (int i = 0; i < m_vComponents.size(); i++)
	{
		delete m_vComponents[i];
	}
	m_vComponents.resize(0);
}

XMMATRIX GameObject::GetWorldMatrix()
{
	//Force update rotation quaternion
	SetRotation(m_rotation);
	XMMATRIX trans = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX rot = m_rotationQuat;
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	return scale*rot*trans;
}

XMFLOAT3 GameObject::GetForward()
{
	//Force update rotation quaternion
	SetRotation(m_rotation);
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 0,0,1 }, m_rotationQuat);
	
	return XMFLOAT3{ v.x, v.y, v.z};
}

XMFLOAT3 GameObject::GetRight()
{
	//Force update rotation quaternion
	SetRotation(m_rotation);
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 1,0,0 }, m_rotationQuat);

	return XMFLOAT3{ v.x, v.y, v.z };
}

XMFLOAT3 GameObject::GetUp()
{
	//Force update rotation quaternion
	SetRotation(m_rotation);
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 0,1,0 }, m_rotationQuat);

	return XMFLOAT3{ v.x, v.y, v.z };
}

void GameObject::Update()
{
	for (int i = 0; i < m_vComponents.size(); i++)
	{
		m_vComponents[i]->Update();
	}
}

void GameObject::UpdateGfx()
{
	for (int i = 0; i < m_vComponents.size(); i++)
	{
		m_vComponents[i]->UpdateGfx();
	}
}

Component* GameObject::AddComponent(Component* comp)
{
	comp->SetOwner(this);
	m_vComponents.push_back(comp);
	comp->Start();
	return comp;
}

void GameObject::SetPosition(XMFLOAT3 pos)
{
	m_position = pos;
}

void GameObject::SetRotation(XMFLOAT3 rot)
{
	m_rotation = rot; 
	m_rotationQuat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rot.x), XMConvertToRadians(rot.y), XMConvertToRadians(rot.z));
}

void GameObject::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
}

template <class T>
bool GameObject::GetComponent(T& outComp)
{
	for (int i = 0; i < m_vComponents.size(); i++)
	{
		T* val = dynamic_cast<const T*>(m_vComponents[i]);
		if (val != nullptr)
		{
			outComp = *val;
			return true;
		}
	}

	return false;
}
