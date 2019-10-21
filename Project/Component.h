#pragma once

class GameObject;

class Component
{
protected:
	GameObject* m_owner;
public:
	Component();

	void SetOwner(GameObject* owner) { m_owner = owner; }
	GameObject* GetOwner() { return m_owner; }

	virtual void Start();
	virtual void Update();
	virtual void UpdateGfx();
};

