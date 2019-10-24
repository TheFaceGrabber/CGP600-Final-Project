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
}

Scene* Scene::LoadFromFile(std::string file)
{
	Scene* scene = new Scene();

	std::ifstream f(file);
	if (!f.is_open())
	{
		throw "Model file could not be openned. Reason: Location does not exists";
		return nullptr;
	}

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
		if(fileCurSection.empty())
			s >> fileCurSection;

		std::string caption;
		s >> caption;

		if (caption == "End")
			fileCurSection = "";
		else if(fileCurSection == "Sun")
		{
			if(caption.empty())
			{
				sunObj = new GameObject("Sun");
				sun = (DirectionLight*)sunObj->AddComponent(new DirectionLight());
				scene->RegisterGameObject(sunObj);
			}
			else if(caption == "Rotation")
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
				float r,g,b;
				s >> r >> g >> b;
				sun->SetColour({r,g,b,1});
			}
			else if (caption == "Ambient")
			{
				float r, g, b;
				s >> r >> g >> b;
				sun->SetAmbientColour({ r,g,b,1 });
			}
		}
		else if(fileCurSection == "Player")
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
		else if(fileCurSection == "Entity")
		{
			if (caption.empty())
			{
				GameObject* ent = new GameObject("New Entity");
				ent->SetPosition({ 0, 0, 0 });
				scene->RegisterGameObject(ent);
			}
			else if(caption == "Name")
			{
				std::string name;
				s >> name;
				int last = scene->m_vGameObjects.size() - 1;
				scene->m_vGameObjects[last]->SetName(name);
			}
			else if(caption == "Position")
			{
				float x, y, z;
				s >> x >> y >> z;
				int last = scene->m_vGameObjects.size() - 1;
				scene->m_vGameObjects[last]->SetPosition({ x,y,z });
			}
			else if(caption == "Rotation")
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
			else if(caption == "Component")
			{
				std::string comp;
				s >> comp;
				if(comp == "MeshRenderer")
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
	}

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
	for (int i = 0; i < m_vGameObjects.size(); i++)
	{
		m_vGameObjects[i]->UpdateGfx();
	}
}

void Scene::Unload()
{
	//Tell scene manager I am unloading and then "delete this"
}

