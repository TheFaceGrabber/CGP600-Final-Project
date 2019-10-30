#pragma once
#include <vector>
#include "GameObject.h"
#include "Skybox.h"

class Scene
{
private:
	std::vector<GameObject*> m_vGameObjects;
	std::string fileLocation;
	Skybox* m_pSky;
public:
	static Scene* LoadFromFile(std::string file);

	bool RegisterGameObject(GameObject* obj);
	bool DestroyGameObject(GameObject* obj);

	void Update();
	void UpdateGfx();

	void Unload();

	void SetSky(string material);

	Scene();
	~Scene();
};

