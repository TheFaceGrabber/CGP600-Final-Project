#include "GameObject.h"
#include "Component.h"
#include "Direct3D.h"
#include <typeinfo>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

//Generate world rotation based upon parent rotation - new design
XMFLOAT3 GameObject::GetWorldRotation()
{
	XMFLOAT3 worldRot = { m_rotation.x,m_rotation.y,m_rotation.z };
	if (m_parent)
		worldRot = { m_parent->GetRotation().x + m_rotation.x , m_parent->GetRotation().y + m_rotation.y ,m_parent->GetRotation().z + m_rotation.z };
	return worldRot;
}

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

void GameObject::SetParent(GameObject* go)
{
	m_parent = go;
}

GameObject* GameObject::GetParent()
{
	return m_parent;
}

XMMATRIX GameObject::GetWorldMatrix()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	m_rotationQuat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));

	XMFLOAT3 worldPos = { m_position.x, m_position.y, m_position.z };
	if (m_parent)
		worldPos = { m_parent->GetPosition().x + m_position.x , m_parent->GetPosition().y + m_position.y ,m_parent->GetPosition().z + m_position.z };

	XMMATRIX trans = XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
	XMMATRIX rot = m_rotationQuat;
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	return scale*rot*trans;
}

XMFLOAT3 GameObject::GetForward()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	m_rotationQuat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 0,0,1 }, m_rotationQuat);
	
	return XMFLOAT3{ v.x, v.y, v.z};
}

XMFLOAT3 GameObject::GetRight()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	m_rotationQuat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 1,0,0 }, m_rotationQuat);

	return XMFLOAT3{ v.x, v.y, v.z };
}

XMFLOAT3 GameObject::GetUp()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	m_rotationQuat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));
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

void GameObject::UpdateShadowMap()
{
	for (int i = 0; i < m_vComponents.size(); i++)
	{
		m_vComponents[i]->UpdateShadowMap();
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
}

void GameObject::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
}
