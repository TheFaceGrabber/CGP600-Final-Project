#include "Scene.h"
#include <algorithm>
#include <fstream>
#include <strstream>
#include "DirectionLight.h"
#include "FlyCamera.h"
#include "MeshRenderer.h"

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

	delete m_pSky;
}

void Scene::ParseLevelGrid(Scene* scene, map<char, Block> blocks, vector<string> grid)
{
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

Scene* Scene::LoadFromFile(std::string file)
{
	std::ifstream f(file);
	if (!f.is_open())
	{
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
				playerObj = new GameObject("Player");
				playerObj->AddComponent(new FlyCamera());
				scene->RegisterGameObject(playerObj);
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

	scene->m_blocks = blocksDictionary;
	scene->m_grid = gridLines;
	
	ParseLevelGrid(scene, blocksDictionary, gridLines);

	return scene;
	//Parse jlvl file and apply to new scene object
}

bool Scene::RegisterGameObject(GameObject* obj)
{
	m_vGameObjects.push_back(obj);

	return true;
}

bool Scene::DestroyGameObject(GameObject* obj)
{
	if(std::find(m_vGameObjects.begin(), m_vGameObjects.end(), obj) != m_vGameObjects.end())
	{
		m_vGameObjects.erase(std::remove(m_vGameObjects.begin(), m_vGameObjects.end(), obj), m_vGameObjects.end());
		delete obj;
		return true;
	}

	std::string objName = obj->GetName();
	OutputDebugString(("Game Object \"" + objName + "\" hasn't been registered in the scene and thus cannot be destroyed").c_str());

	return false;
}

void Scene::Update()
{
	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		m_vGameObjects[i]->Update();
	}
}

void Scene::UpdateGfx()
{
	if(m_pSky)
		m_pSky->UpdateGfx();

	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		m_vGameObjects[i]->UpdateGfx();
	}
}

void Scene::Unload()
{
	//Tell scene manager I am unloading and then "delete this"
}

void Scene::SetSky(string material)
{
	m_pSky = new Skybox(material);
}

bool Scene::CheckForVoxel(XMFLOAT3 pos)
{
	int posX = floor((pos.x / m_Scale) + 0.5);
	int posY = pos.y;// floor((pos.y / m_Scale));
	int posZ = floor((pos.z / m_Scale) + 0.5);

	Block block;
	char key;

	key = m_grid[posZ][posX];
	block = m_blocks[key];

	if (block.Height >= posY)
		return true;

	return false;
}

