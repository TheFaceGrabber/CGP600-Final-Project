#include "Scene.h"
#include <algorithm>
#include <fstream>
#include <strstream>
#include "DirectionLight.h"
#include "FlyCamera.h"
#include "MeshRenderer.h"
#include "PhysicsComponent.h"
#include "BoundingBoxCollider.h"
#include "Camera.h"
#include "FirstPersonPlayer.h"
#include "PigeonComponent.h"
#include <list>

Scene::Scene()
{

}

Scene::~Scene()
{
	//Delete all game objects in the list	
	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		delete m_vGameObjects[i];
	}

	m_vGameObjects.resize(0);
	
	//Delete all path nodes in the list	
	for (int i = 0; i < m_vPathNodes.size(); i++)
	{
		delete m_vPathNodes[i];
	}

	m_vPathNodes.resize(0);

	delete m_pSky;
}

void Scene::ParseLevelGrid(Scene* scene, map<char, Block> blocks, vector<string> grid)
{
	scene->m_vPathNodes.resize(0);
	map<char, vector<DefaultVertex>> meshVerts;

	GameObject* level = new GameObject("Level Grid");
	const float scale = scene->m_Scale;
	level->SetScale({ scale,scale,scale });

	for (auto& pair : blocks)
	{
		meshVerts.emplace(pair.first, vector<DefaultVertex>());
	}

	for (int y = grid.size() - 1; y > 0; y--)
	{
		for (int x = 0; x < grid[y].length(); x++)
		{
			float xf, yf;
			xf = (float)x;
			yf = (float)y;
			char curChar = grid[y][x];
			Block block = blocks[curChar];
			
			//Create path node is block height is 0 (is floor)
			if (block.Height == 0) {
				PathNode* node = new PathNode();
				node->x = x;
				node->y = y;
				node->hasBeenVisited = false;
				node->parent = false;
				scene->m_vPathNodes.push_back(node);
			}
			//Create top
			{
				DefaultVertex tl;
				tl.Pos = { xf - .5f, block.Height, yf + .5f };
				tl.Norm = { 0,1,0 };
				tl.UV = { 0, 1- block.TileAmount };
				DefaultVertex bl;
				bl.Pos = { xf - .5f, block.Height, yf - .5f };
				bl.Norm = { 0,1,0 };
				bl.UV = { 0, 1 - 0};
				DefaultVertex tr;
				tr.Pos = { xf + .5f, block.Height, yf + .5f };
				tr.Norm = { 0,1,0 };
				tr.UV = { block.TileAmount, 1 - block.TileAmount };
				DefaultVertex br;
				br.Pos = { xf + .5f, block.Height, yf - .5f };
				br.Norm = { 0,1,0 };
				br.UV = { block.TileAmount, 1 - 0};

				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(tl);
				meshVerts[curChar].push_back(tr);

				meshVerts[curChar].push_back(br);
				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(tr);
			}
			//Create east/right
			if (x != grid[x].size() - 1 && blocks[grid[y][x + 1]].Height < block.Height)
			{
				DefaultVertex tl;
				tl.Pos = { xf + .5f, block.Height, yf - .5f };
				tl.Norm = { 1,0,0 };
				tl.UV = { 0, 1 - block.TileAmount * block.Height };
				DefaultVertex bl;
				bl.Pos = { xf + .5f, 0, yf - .5f };
				bl.Norm = { 1,0,0 };
				bl.UV = { 0, 1 - 0 };
				DefaultVertex tr;
				tr.Pos = { xf + .5f, block.Height, yf + .5f };
				tr.Norm = { 1,0,0 };
				tr.UV = { block.TileAmount,1 - block.TileAmount * block.Height };
				DefaultVertex br;
				br.Pos = { xf + .5f, 0, yf + .5f };
				br.Norm = { 1,0,0 };
				br.UV = { block.TileAmount, 1 - 0 };

				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(tl);
				meshVerts[curChar].push_back(tr);

				meshVerts[curChar].push_back(br);
				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(tr);
			}
			//Create west/left
			if (x != 0 && blocks[grid[y][x - 1]].Height < block.Height)
			{
				DefaultVertex tl;
				tl.Pos = { xf - .5f, block.Height, yf - .5f };
				tl.Norm = { -1,0,0 };
				tl.UV = { 0, 1 - block.TileAmount * block.Height };
				DefaultVertex bl;
				bl.Pos = { xf - .5f, 0, yf - .5f };
				bl.Norm = { -1,0,0 };
				bl.UV = { 0, 1 - 0 };
				DefaultVertex tr;
				tr.Pos = { xf - .5f, block.Height, yf + .5f };
				tr.Norm = { -1,0,0 };
				tr.UV = { block.TileAmount, 1 - block.TileAmount * block.Height };
				DefaultVertex br;
				br.Pos = { xf - .5f, 0, yf + .5f };
				br.Norm = { -1,0,0 };
				br.UV = { block.TileAmount,1 - 0 };

				meshVerts[curChar].push_back(tr);
				meshVerts[curChar].push_back(tl);
				meshVerts[curChar].push_back(bl);

				meshVerts[curChar].push_back(tr);
				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(br);
			}
			//Create north
			if (y != grid.size() - 1 && blocks[grid[y + 1][x]].Height < block.Height)
			{
				DefaultVertex tl;
				tl.Pos = { xf - .5f, block.Height, yf + .5f };
				tl.Norm = { 0,0,1 };
				tl.UV = { 0, 1 - block.TileAmount * block.Height };
				DefaultVertex bl;
				bl.Pos = { xf - .5f, 0, yf + .5f };
				bl.Norm = { 0,0,1 };
				bl.UV = { 0, 1 - 0 };
				DefaultVertex tr;
				tr.Pos = { xf + .5f, block.Height, yf + .5f };
				tr.Norm = { 0,0,1 };
				tr.UV = { block.TileAmount,1 - block.TileAmount * block.Height };
				DefaultVertex br;
				br.Pos = { xf + .5f, 0, yf + .5f };
				br.Norm = { 0,0,1 };
				br.UV = { block.TileAmount,1 - 0 };

				meshVerts[curChar].push_back(tr);
				meshVerts[curChar].push_back(tl);
				meshVerts[curChar].push_back(bl);

				meshVerts[curChar].push_back(tr);
				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(br);
			}
			//Create south
			if (y != 0 && blocks[grid[y - 1][x]].Height < block.Height)
			{
				DefaultVertex tl;
				tl.Pos = { xf - .5f, block.Height, yf - .5f };
				tl.Norm = { 0,0,-1 };
				tl.UV = { 0, 1 - block.TileAmount * block.Height };
				DefaultVertex bl;
				bl.Pos = { xf - .5f, 0, yf - .5f };
				bl.Norm = { 0,0,-1 };
				bl.UV = { 0, 1 - 0 };
				DefaultVertex tr;
				tr.Pos = { xf + .5f, block.Height, yf - .5f };
				tr.Norm = { 0,0,-1 };
				tr.UV = { block.TileAmount, 1 - block.TileAmount * block.Height };
				DefaultVertex br;
				br.Pos = { xf + .5f, 0, yf - .5f };
				br.Norm = { 0,0,-1 };
				br.UV = { block.TileAmount, 1 - 0 };

				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(tl);
				meshVerts[curChar].push_back(tr);

				meshVerts[curChar].push_back(br);
				meshVerts[curChar].push_back(bl);
				meshVerts[curChar].push_back(tr);
			}
		}
	}

	//Loop through each path node
	//Assign neighbours to each path node
	for (int i = 0; i < scene->m_vPathNodes.size(); i++)
	{
		PathNode* possibleNeighbour = nullptr;
		if(scene->m_vPathNodes[i]->x > 0)
		{
			possibleNeighbour = scene->GetPathNodeAtPosition(scene->m_vPathNodes[i]->x - 1, scene->m_vPathNodes[i]->y);
			if (possibleNeighbour != nullptr)
				scene->m_vPathNodes[i]->neighbours.push_back(possibleNeighbour);
		}
		if (scene->m_vPathNodes[i]->x < grid[scene->m_vPathNodes[i]->y].length())
		{
			possibleNeighbour = scene->GetPathNodeAtPosition(scene->m_vPathNodes[i]->x + 1, scene->m_vPathNodes[i]->y);
			if (possibleNeighbour != nullptr)
				scene->m_vPathNodes[i]->neighbours.push_back(possibleNeighbour);
		}
		if (scene->m_vPathNodes[i]->y > 0) {
			possibleNeighbour = scene->GetPathNodeAtPosition(scene->m_vPathNodes[i]->x, scene->m_vPathNodes[i]->y - 1);
			if (possibleNeighbour != nullptr)
				scene->m_vPathNodes[i]->neighbours.push_back(possibleNeighbour);
		}
		if (scene->m_vPathNodes[i]->y < grid.size()) {
			possibleNeighbour = scene->GetPathNodeAtPosition(scene->m_vPathNodes[i]->x, scene->m_vPathNodes[i]->y + 1);
			if (possibleNeighbour != nullptr)
				scene->m_vPathNodes[i]->neighbours.push_back(possibleNeighbour);
		}
	}

	//Apply materials and vertices to level grid mesh
	for (auto& pair : meshVerts)
	{
		Mesh* mesh = new Mesh();
		mesh->ApplyVertices(pair.second, true);
		mesh->SetShader(Shader::LoadFromMaterial(blocks[pair.first].Material));
		MeshRenderer* r = (MeshRenderer*)level->AddComponent(new MeshRenderer());
		r->SetMesh(mesh);
	}

	scene->RegisterGameObject(level);
}

//Try to get a path node at a given position
PathNode* Scene::GetPathNodeAtPosition(int x, int y)
{
	for (int i = 0; i < m_vPathNodes.size(); i++)
	{
		if (m_vPathNodes[i]->x == x && m_vPathNodes[i]->y == y) 
		{
			return m_vPathNodes[i];
		}
	}
	return nullptr;
}

Scene* Scene::LoadFromFile(std::string file)
{
	std::ifstream f(file);
	if (!f.is_open())
	{
		//If the file couldn't be found then throw an error
		throw exception("Scene file could not be opened. Reason: Location does not exists");
		return nullptr;
	}

	Scene* scene = new Scene();

	map<char, Block> blocksDictionary;
	vector<string> gridLines;

	std::string fileCurSection = "";

	GameObject* sunObj = nullptr;
	DirectionLight* sun = nullptr;

	GameObject* playerObj = nullptr;

	//Loop through each line in the file
	while (!f.eof())
	{
		const int size = 128;
		char line[size];
		f.getline(line, size);

		std::strstream s;
		s << line;
		if (fileCurSection.empty())
			s >> fileCurSection;

		std::string caption;
		s >> caption;

		if (caption == "End")
			fileCurSection = "";
		else if (fileCurSection == "Scale")
		{
			if (!caption.empty()) {
				scene->SetScale(std::stof(caption));
				fileCurSection = "";
			}
		}
		else if (fileCurSection == "Sun")
		{
			if (caption.empty())
			{
				sunObj = new GameObject("Sun");
				sun = (DirectionLight*)sunObj->AddComponent(new DirectionLight());
				scene->RegisterGameObject(sunObj);
			}
			else if (caption == "Rotation")
			{
				float x, y, z;
				s >> x >> y >> z;
				sunObj->SetRotation({ x,y,z });
			}
			else if (caption == "Intensity")
			{
				float i;
				s >> i;
				sun->SetIntensity(i);
			}
			else if (caption == "Colour")
			{
				float r, g, b;
				s >> r >> g >> b;
				sun->SetColour({ r,g,b,1 });
			}
			else if (caption == "Ambient")
			{
				float r, g, b;
				s >> r >> g >> b;
				sun->SetAmbientColour({ r,g,b,1 });
			}
		}
		else if (fileCurSection == "Player")
		{
			if (caption.empty())
			{
				//Create player object
				playerObj = new GameObject("Player");
				playerObj->AddComponent(new FirstPersonPlayer());

				BoundingBoxCollider* col = (BoundingBoxCollider*)playerObj->AddComponent(new BoundingBoxCollider());
				col->SetWidth(.25);
				col->SetHeight(1);
				playerObj->AddComponent(new PhysicsComponent());

				//Create camera object
				GameObject* cam = new GameObject("Camera");
				Camera::SetMain(cam);
				cam->SetParent(playerObj);
				cam->SetPosition({ 0,0.8,0 });
				
				GameObject* gunObj = new GameObject("Gun");
				gunObj->SetParent(Camera::GetMain());
				gunObj->SetPosition({ .3, -.5, 1 });
				gunObj->SetRotation({ 0, 90, 0 });
				MeshRenderer* r = (MeshRenderer*)gunObj->AddComponent(new MeshRenderer());
				Mesh* m = new Mesh();
				m->LoadFromFile("Assets/Models/Guns/9mm.obj");
				m->UpdateMaterial("Assets/Materials/Gun.jmtl");
				r->SetMesh(m);
				
				scene->RegisterGameObject(gunObj);
				scene->RegisterGameObject(playerObj);
				scene->RegisterGameObject(cam);
			}
			else if (caption == "Rotation")
			{
				float x, y, z;
				s >> x >> y >> z;
				playerObj->SetRotation({ x,y,z });
			}
			else if (caption == "Position")
			{
				float x, y, z;
				s >> x >> y >> z;
				playerObj->SetPosition({ x,y,z });
			}
		}
		else if (fileCurSection == "Entity")
		{
			if (caption.empty())
			{
				GameObject* ent = new GameObject("New Entity");
				ent->SetPosition({ 0, 0, 0 });
				scene->RegisterGameObject(ent);
			}
			else if (caption == "Name")
			{
				std::string name;
				s >> name;
				int last = scene->m_vGameObjects.size() - 1;
				scene->m_vGameObjects[last]->SetName(name);
			}
			else if (caption == "Position")
			{
				float x, y, z;
				s >> x >> y >> z;
				int last = scene->m_vGameObjects.size() - 1;
				scene->m_vGameObjects[last]->SetPosition({ x,y,z });
			}
			else if (caption == "Rotation")
			{
				float x, y, z;
				s >> x >> y >> z;
				int last = scene->m_vGameObjects.size() - 1;
				scene->m_vGameObjects[last]->SetRotation({ x,y,z });
			}
			else if (caption == "Scale")
			{
				float x, y, z;
				s >> x >> y >> z;
				int last = scene->m_vGameObjects.size() - 1;
				scene->m_vGameObjects[last]->SetScale({ x,y,z });
			}
			else if (caption == "Component")
			{
				std::string comp;
				s >> comp;
				if (comp == "MeshRenderer")
				{
					std::string mesh;
					std::string mat;
					s >> mesh >> mat;
					int last = scene->m_vGameObjects.size() - 1;
					MeshRenderer* r = (MeshRenderer*)scene->m_vGameObjects[last]->AddComponent(new MeshRenderer());
					Mesh* m = new Mesh();
					m->LoadFromFile(mesh);
					m->UpdateMaterial(mat);
					r->SetMesh(m);
				}
				else if (comp == "PhysicsComponent")
				{
					int last = scene->m_vGameObjects.size() - 1;
					scene->m_vGameObjects[last]->AddComponent(new PhysicsComponent());
				}
				else if (comp == "BoxCollider")
				{
					float width;
					float height;
					s >> width >> height;
					int last = scene->m_vGameObjects.size() - 1;
					BoundingBoxCollider* r = (BoundingBoxCollider*)scene->m_vGameObjects[last]->AddComponent(new BoundingBoxCollider());
					r->SetWidth(width);
					r->SetHeight(height);
				}
				else if(comp == "Pigeon")
				{
					int last = scene->m_vGameObjects.size() - 1;
					PigeonComponent* r = (PigeonComponent*)scene->m_vGameObjects[last]->AddComponent(new PigeonComponent());
					r->SetPlayer(playerObj);
				}
			}
		}
		else if (fileCurSection == "Skybox")
		{
			if (caption == "Material")
			{
				std::string comp;
				s >> comp;
				scene->SetSky(comp);
			}
		}
		else if (fileCurSection == "Blocks")
		{
			if (caption == "Block") 
			{
				char key;
				float height = 0;
				float tile;
				string mat;
				s >> key >> height >> mat >> tile;
				Block b;
				b.TileAmount = tile;
				b.Height = height;
				b.Material = mat;
				blocksDictionary.emplace(key, b);
			}
		}
		else if (fileCurSection == "Grid")
		{
			if(!caption.empty())
				gridLines.push_back(caption);
		}
	}

	scene->m_mBlocks = blocksDictionary;
	scene->m_vGrid = gridLines;
	
	ParseLevelGrid(scene, blocksDictionary, gridLines);

	return scene;
	//Parse jlvl file and apply to new scene object
}

//Register a given gameobject into the scene
bool Scene::RegisterGameObject(GameObject* obj)
{
	m_vGameObjects.push_back(obj);

	return true;
}

//Destroy and remove a gameobject from the scene
bool Scene::DestroyGameObject(GameObject* obj)
{
	if(std::find(m_vGameObjects.begin(), m_vGameObjects.end(), obj) != m_vGameObjects.end())
	{
		//Apply to list of objects to destroy at the end of the frame
		if(std::find(m_vGameObjectsForDestruction.begin(), m_vGameObjectsForDestruction.end(), obj) == m_vGameObjectsForDestruction.end())
			m_vGameObjectsForDestruction.push_back(obj);
		
		return true;
	}

	std::string objName = obj->GetName();
	OutputDebugString(("Game Object \"" + objName + "\" hasn't been registered in the scene and thus cannot be destroyed").c_str());

	return false;
}

//Process update on all game objects
void Scene::Update()
{
	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		m_vGameObjects[i]->Update();
	}
}

void Scene::UpdateDestruction()
{
	for (int i = 0; i < m_vGameObjectsForDestruction.size(); i++)
	{
		m_vGameObjects.erase(std::remove(m_vGameObjects.begin(), m_vGameObjects.end(), m_vGameObjectsForDestruction[i]), m_vGameObjects.end());
		delete m_vGameObjectsForDestruction[i];
	}
	m_vGameObjectsForDestruction.resize(0);
}

//Process drawing on all game objects
void Scene::UpdateGfx()
{
	if(m_pSky)
		m_pSky->UpdateGfx();

	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		m_vGameObjects[i]->UpdateGfx();
	}
}

//Process drawing shadows on all game objects
void Scene::UpdateShadowMap()
{
	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		m_vGameObjects[i]->UpdateShadowMap();
	}
}

void Scene::SetSky(string material)
{
	m_pSky = new Skybox(material);
}

bool Scene::CheckForVoxel(XMFLOAT3 pos)
{
	//Round the position to conform to the level grid
	int posX = floor((pos.x / m_Scale) + 0.5);
	float posY = (pos.y);
	int posZ = floor((pos.z / m_Scale) + 0.5);

	Block block;
	char key;

	//Make sure we are actually inside of the level
	if (posZ < 0 || posZ >= m_vGrid.size())
		return false;

	//Make sure we are actually inside of the level
	if (posX < 0 || posX >= m_vGrid[posZ].size())
		return false;

	//Get the block key based upon the rounded positions
	key = m_vGrid[posZ][posX];
	//Get the block object from the key
	block = m_mBlocks[key];

	//We are colliding if the block at the given position is higher than our height
	if (block.Height * m_Scale >= posY)
		return true;

	return false;
}

GameObject* Scene::CheckForObjectCollision(GameObject* object, XMFLOAT3 pos, float width, float height, bool ignorePlayer)
{
	/*float xMin = pos.x - width;
	float xMax = pos.x + width;

	float zMin = pos.z - width;
	float zMax = pos.z + width;

	float yMin = pos.y - height;
	float yMax = pos.y + height;
	*/
	
	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		if (m_vGameObjects[i] == object || (m_vGameObjects[i]->GetName() == "Player" && ignorePlayer))
			continue;

		BoundingBoxCollider* col = m_vGameObjects[i]->GetComponent<BoundingBoxCollider>();
		if (col == nullptr)
			continue;

		XMFLOAT3 objPos = m_vGameObjects[i]->GetWorldPosition();

		objPos.x += col->GetCentre().x;
		objPos.y += col->GetCentre().y;
		objPos.z += col->GetCentre().z;

		/*float ObjxMin = objPos.x - col->GetWidth();
		float ObjxMax = objPos.x + col->GetWidth();

		float objzMin = objPos.z - col->GetWidth();
		float objzMax = objPos.z + col->GetWidth();

		float ObjyMin = objPos.y - col->GetWidth();
		float ObjyMax = objPos.y + col->GetWidth();
		*/
		if (abs(pos.x - objPos.x) > width + col->GetWidth())
			continue;

		if (abs(pos.z - objPos.z) > width + col->GetWidth())
			continue;

		if (abs(pos.y - objPos.y) > height + col->GetHeight())
			continue;

		return m_vGameObjects[i];
	}

	return nullptr;
}

std::vector<XMFLOAT3> Scene::GetAStarPath(XMFLOAT3 startPos, XMFLOAT3 endPos)
{
	std::vector<XMFLOAT3> output = {};

	XMFLOAT3 roundedStart = { startPos.x,startPos.y,startPos.z };
	XMFLOAT3 roundedEnd = { endPos.x,endPos.y,endPos.z };
	
	//Round start pos to level grid
	roundedStart.x = floor((roundedStart.x / m_Scale) + 0.5);
	roundedStart.z = floor((roundedStart.z / m_Scale) + 0.5);

	//Round start pos to level grid
	roundedEnd.x = floor((roundedEnd.x / m_Scale) + 0.5);
	roundedEnd.z = floor((roundedEnd.z / m_Scale) + 0.5);

	//All grid sections with a height of 0 will be counted as a pathing node

	//First check if the rounded start position and rounded end position are the same (they are at the same path node)
	//If so, just add the unrounded end position to the output list and return it
	if (roundedEnd.x == roundedStart.x && roundedEnd.z == roundedStart.z) 
	{
		output.push_back(endPos);
		return output;
	}

	//Get nodes at the rounded start and end pos
	PathNode* startNode = GetPathNodeAtPosition(roundedStart.x, roundedStart.z);
	PathNode* endNode = GetPathNodeAtPosition(roundedEnd.x, roundedEnd.z);

	//If either of those nodes don't exist, return null because the algorithm cannot be completed without them
	if (startNode == nullptr || endNode == nullptr)
		return output;
	
	//A* Algorithm here
	
	//Reset all nodes
	for (int i = 0; i < m_vPathNodes.size(); i++)
	{
		m_vPathNodes[i]->hasBeenVisited = false;
		m_vPathNodes[i]->globalGoal = INFINITY;
		m_vPathNodes[i]->localGoal = INFINITY;
		m_vPathNodes[i]->parent = nullptr;
	}

	//Lambda function to get the heuristic (distance) used for A*
	auto heuristic = [](PathNode* a, PathNode* b)
	{
		return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
	};

	//start the search at the starting node
	PathNode* currentNode = startNode;
	startNode->localGoal = 0;
	startNode->globalGoal = heuristic(startNode, endNode);

	list<PathNode*> nodesToTest;
	nodesToTest.push_back(startNode);

	//Run this loop so long as we have nodes to test
	while(!nodesToTest.empty())
	{
		//Sort our nodes to test by their global goal (based upon the heuristic)
		nodesToTest.sort([](const PathNode* lhs, const PathNode* rhs) { return lhs->globalGoal < rhs->globalGoal; });

		//If the next node in the list has already been tested, remove it
		while (!nodesToTest.empty() && nodesToTest.front()->hasBeenVisited)
			nodesToTest.pop_front();

		//If we have no more nodes to test, leave the loop
		if (nodesToTest.empty())
			break;

		//Get next node to test
		currentNode = nodesToTest.front();
		currentNode->hasBeenVisited = true;

		//Loop through its neighbours
		for (int i = 0; i < currentNode->neighbours.size(); i++)
		{
			if (!currentNode->neighbours[i]->hasBeenVisited)
				nodesToTest.push_back(currentNode->neighbours[i]);

			//Calculate what maybe possibly be a better goal value based on the parent nodes goal and heuristic (distance)
			float possiblyLowerGoal = currentNode->localGoal + heuristic(currentNode, currentNode->neighbours[i]);

			if(possiblyLowerGoal < currentNode->neighbours[i]->localGoal)
			{
				currentNode->neighbours[i]->parent = currentNode;
				currentNode->neighbours[i]->localGoal = possiblyLowerGoal;

				currentNode->neighbours[i]->globalGoal = currentNode->neighbours[i]->localGoal + heuristic(currentNode->neighbours[i], endNode);
			}
		}
	}

	//Add the end pos to the output list
	//This is because the end pos might not necessarily be on the grid
	output.push_back(endPos);
	currentNode = endNode->parent;
	//Go through each node, adding it and finding its parent until we have reach the starting node
	while(currentNode != startNode)
	{
		output.push_back({ (float)currentNode->x * m_Scale,0,(float)currentNode->y * m_Scale });
		currentNode = currentNode->parent;
	}

	//Reverse the output list so it goes to start to end and not end to start
	std::reverse(output.begin(), output.end());
	
	return output;
}

