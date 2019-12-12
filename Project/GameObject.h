#pragma once
#include <string>
#include <vector>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>

class Component;

class GameObject
{
private:
	std::string m_name;
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;
	XMMATRIX m_worldMatrix;
	std::vector<Component*> m_vComponents;
	//New design
	GameObject* m_parent;
	void UpdateWorldMat();
	bool m_wasWorldMatUpdateThisFrame;

public:
	GameObject(std::string name);
	~GameObject();

	//New design
	void SetParent(GameObject* go);
	GameObject* GetParent();

	XMFLOAT3 GetWorldRotation();
	XMFLOAT3 GetWorldPosition();

	std::string GetName() { return m_name; }
	void SetName(std::string name) { m_name = name; }
	XMFLOAT3 GetLocalPosition() { return m_position; }
	XMFLOAT3 GetLocalRotation() { return m_rotation; }
	XMFLOAT3 GetScale() { return m_scale; }

	XMMATRIX GetWorldMatrix();

	XMFLOAT3 GetForward();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();

	Component* AddComponent(Component* comp);
	template <class T>
	T* GetComponent()
	{
		for (int i = 0; i < m_vComponents.size(); i++)
		{
			T* val = dynamic_cast<T*>(m_vComponents[i]);
			if (val != nullptr)
			{
				return val;
			}
		}

		return nullptr;
	}

	void SetPosition(XMFLOAT3 pos);
	void SetRotation(XMFLOAT3 rot);
	void SetScale(XMFLOAT3 scale); 

	void Update();
	void UpdateGfx();
	void UpdateShadowMap();
};
