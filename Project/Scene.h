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
	float m_Scale;

	map<char, Block> m_blocks;
	vector<string> m_grid;

	static void ParseLevelGrid(Scene* scene, map<char, Block> blocks, vector<string> grid);

public:
	static Scene* LoadFromFile(std::string file);

	bool RegisterGameObject(GameObject* obj);
	bool DestroyGameObject(GameObject* obj);

	void SetScale(float scale) { m_Scale = scale; }
	float GetScale() { return m_Scale; }

	void Update();
	void UpdateGfx();

	void Unload();

	void SetSky(string material);

	bool CheckForVoxel(XMFLOAT3 pos);

	Scene();
	~Scene();
};

