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

struct PathNode
{
public:
	int x;
	int y;
	bool hasBeenVisited = false;
	float globalGoal = 0;
	float localGoal = 0;
	std::vector<PathNode*> neighbours;
	PathNode* parent;
};

class Scene
{
private:
	std::vector<GameObject*> m_vGameObjects;
	std::vector<GameObject*> m_vGameObjectsForDestruction;
	std::vector<PathNode*> m_vPathNodes;
	std::string m_sFileLocation;
	Skybox* m_pSky;
	float m_Scale;

	map<char, Block> m_mBlocks;
	vector<string> m_vGrid;

	static void ParseLevelGrid(Scene* scene, map<char, Block> blocks, vector<string> grid);
	PathNode* GetPathNodeAtPosition(int x, int y);

public:
	static Scene* LoadFromFile(std::string file);

	bool RegisterGameObject(GameObject* obj);
	bool DestroyGameObject(GameObject* obj);

	void SetScale(float scale) { m_Scale = scale; }
	float GetScale() { return m_Scale; }

	void Update();
	void UpdateDestruction();
	void UpdateGfx();
	void UpdateShadowMap();

	void SetSky(string material);

	bool CheckForVoxel(XMFLOAT3 pos);
	GameObject* CheckForObjectCollision(GameObject* object, XMFLOAT3 pos, float width, float height, bool ignorePlayer);

	std::vector<XMFLOAT3> GetAStarPath(XMFLOAT3 startPos, XMFLOAT3 endPos);

	Scene();
	~Scene();
};

