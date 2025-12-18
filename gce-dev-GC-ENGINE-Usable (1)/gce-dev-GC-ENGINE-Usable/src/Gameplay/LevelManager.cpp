#include "LevelManager.h"
#include "RessourcesManager.h"
#include "JsonImporter.hpp"
#include "Player.h"
#include"SnowMan.h"
#include "Elf.h"
#include "Deer.h"
#include "Robot.h"

namespace gce {

    void LevelManager::Create()
    {
		if (m_instance != nullptr) return;
		m_instance = new LevelManager();
        m_instance->m_levels.PushBack({ "res/Scene/Level1.json", "Niveau 1", 5 });
        m_instance->m_levels.PushBack({ "res/Scene/Level2.json", "Niveau 2", 5 });
        m_instance->m_levels.PushBack({ "res/Scene/Level3.json", "Niveau 3", 5 });
    }

    void LevelManager::LoadNextLevel()
    {
        int next = m_instance->m_currentIndex + 1;
        if (next >= m_instance->m_levels.Size())
        {
            next = 0;
        }
        LoadLevel(next);
    }

    void LevelManager::LoadLevel(int index)
    {
        if (index < 0 || index >= m_instance->m_levels.Size()) return;

        m_instance->m_currentIndex = index;
        const auto& level = m_instance->m_levels[index];
     
        RessourcesManager::ClearCurrentLevel();

        auto importedScene = importSceneFromJsonText(level.jsonPath);

		RessourcesManager::AddLevel(importedScene);

		if (index == 0)
		{
			LoadLevel1();
		}
    }

    void LevelManager::LoadLevel1()
    {
        RessourcesManager::GetPlayer()->GetGameObject()->transform.SetWorldPosition({ 0.f, 30.f, 0.f });  
		if (m_instance->m_levels.Empty()) { return; }
		int creditLeft = m_instance->m_levels[0].ennemyCredit;
		
        int enemyCosts[] = {1, 3}; // Snowman, Deer

		while (creditLeft > 0)
		{
			gce::Vector<int> affordableEnemies;
			if (creditLeft >= enemyCosts[0]) affordableEnemies.PushBack(0); // Snowman
			if (creditLeft >= enemyCosts[1]) affordableEnemies.PushBack(1); // Deer

			if (affordableEnemies.Empty())
			{
				break;
			}

			int randomIndex = rand() % affordableEnemies.Size();
			int randomEnemyType = affordableEnemies[randomIndex];

			GameObject& enemyObject = GameObject::Create(const_cast<Scene&>(*RessourcesManager::GetPlayer()->GetGameObject()->GetScene()));
			enemyObject.SetActive(false);

			int enemyCost = enemyCosts[randomEnemyType];

			switch (randomEnemyType)
			{
			case 0:
			{
				SnowMan* snowman = new SnowMan(&enemyObject, RessourcesManager::GetTileMap());
				RessourcesManager::AddEnnemy(snowman);
				break;
			}
			case 1:
			{
				Deer* deer = new Deer(&enemyObject, RessourcesManager::GetTileMap());
				RessourcesManager::AddEnnemy(deer);
				break;
			}
			}
			creditLeft -= enemyCost;
		}
	}
}