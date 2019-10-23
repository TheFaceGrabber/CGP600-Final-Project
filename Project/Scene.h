#pragma once
#include <vector>
#include "GameObject.h"

class Scene
{
private:
	std::vector<GameObject*> m_vGameObjects;
	std::string fileLocation;
public:
	static Scene* LoadFromFile(std::string file);

	bool RegisterGameObject(GameObject* obj);
	bool DestroyGameObject(GameObject* obj);

	void Update();
	void UpdateGfx();

	void Unload();

	Scene();
	~Scene();
};

