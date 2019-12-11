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
		worldRot = { m_parent->GetWorldRotation().x + m_rotation.x , m_parent->GetWorldRotation().y + m_rotation.y ,m_parent->GetWorldRotation().z + m_rotation.z };
	return worldRot;
}

XMFLOAT3 GameObject::GetWorldPosition()
{
	XMFLOAT3 worldPos = { m_position.x, m_position.y, m_position.z };
	if (m_parent) {
		worldPos = { m_parent->GetWorldPosition().x + m_position.x,
			m_parent->GetWorldPosition().y + m_position.y,
			m_parent->GetWorldPosition().z + m_position.z };
	}
	return worldPos;
}

GameObject::GameObject(std::string name)
{
	m_name = name;
	SetPosition({ 0,0,0 });
	SetRotation({ 0,0,0 });
	SetScale({ 1,1,1 });
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
	UpdateWorldMat();
	return m_worldMatrix;
}

XMFLOAT3 GameObject::GetForward()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 0,0,1 }, rot);
	
	return XMFLOAT3{ v.x, v.y, v.z};
}

XMFLOAT3 GameObject::GetRight()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 1,0,0 }, rot);

	return XMFLOAT3{ v.x, v.y, v.z };
}

XMFLOAT3 GameObject::GetUp()
{
	XMFLOAT3 worldRot = GetWorldRotation();
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(worldRot.x), XMConvertToRadians(worldRot.y), XMConvertToRadians(worldRot.z));
	XMVECTOR v = XMVector3Transform(XMVECTOR{ 0,1,0 }, rot);

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

void GameObject::UpdateWorldMat()
{
	XMFLOAT3 rotVec = m_rotation;

	XMFLOAT3 posVec = m_position;

	XMMATRIX trans = XMMatrixTranslation(posVec.x, posVec.y, posVec.z);
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotVec.x), XMConvertToRadians(rotVec.y), XMConvertToRadians(rotVec.z));;
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	if (m_parent != nullptr)
	{
		XMMATRIX parentTrans = m_parent->GetWorldMatrix();
		m_worldMatrix = (scale * rot * trans) * parentTrans;
		return;
	}

	m_worldMatrix = scale * rot * trans;
}
