#pragma once
#include "Engine.h"

class Entity;
class RessourcesManager
{
public : 
	static void Create();
    static void AddEntities(Entity* entity);
	static gce::Vector<Entity*> getEntities();
private : 
	gce::Vector<Entity*> m_entities;
	inline static RessourcesManager* m_Instance = nullptr;
};

