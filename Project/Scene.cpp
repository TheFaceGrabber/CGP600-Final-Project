#include "Scene.h"
#include <algorithm>
#include <fstream>
#include <strstream>

Scene::Scene()
{

}

Scene::~Scene()
{
	//Delete all game objects in the list
}

Scene* Scene::LoadFromFile(std::string file)
{
	Scene* scene = new Scene();

	std::ifstream f(file);
	if (!f.is_open())
	{
		throw "Model file could not be openned. Reason: Location does not exists";
		return;
	}

	std::string fileCurSection;

	while (!f.eof())
	{
		const int size = 128;
		char line[size];
		f.getline(line, size);

		std::strstream s;
		s << line;
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

