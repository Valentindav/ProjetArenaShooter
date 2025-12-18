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
        m_instance->m_levels.PushBack({ "res/Scene/Level1.json", "Niveau 1", 50 });
        m_instance->m_levels.PushBack({ "res/Scene/Level2.json", "Niveau 2", 50 });
        m_instance->m_levels.PushBack({ "res/Scene/Level3.json", "Niveau 3", 50 });
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
		int randomEnemy;
		int enemyCost = 0;         
        while (creditLeft > 0)
        {
			randomEnemy = rand() % 2;
			GameObject& enemyObject = GameObject::Create(const_cast<Scene&>(*RessourcesManager::GetPlayer()->GetGameObject()->GetScene()));
			enemyObject.SetActive(false);
			switch (randomEnemy)
			{
			case 0:
				enemyCost = 1;
				break;
			case 1:
				enemyCost = 3;
				break;
			default:
				enemyCost = 0;
				break;
			}
            if (enemyCost > creditLeft)
            {
                continue;
			}
			switch (randomEnemy)
			{
			case 0:
			{
				SnowMan * snowman = new SnowMan(&enemyObject, RessourcesManager::GetTileMap());
				RessourcesManager::AddEnnemy(snowman);
				break;
			}
			case 1:
			{
				Deer* deer = new Deer(&enemyObject, RessourcesManager::GetTileMap());
				RessourcesManager::AddEnnemy(deer);
				break;
			}
			default:
				break;
			}
				creditLeft -= enemyCost;
				enemyCost = 0;
		}
	}
}