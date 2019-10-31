#pragma once
#include <vector>
#include <map>
#include "GameObject.h"
#include "Skybox.h"

struct Block
{
public:
	float Height;
	string Material;
	float TileAmount;
};

class Scene
{
private:
	std::vector<GameObject*> m_vGameObjects;
	std::string fileLocation;
	Skybox* m_pSky;

	static void ParseLevelGrid(Scene* scene, map<char, Block> blocks, vector<string> grid);

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

