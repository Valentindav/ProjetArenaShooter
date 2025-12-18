#include "LevelManager.h"
#include "RessourcesManager.h"
#include "JsonImporter.hpp"
#include "Player.h"
#include"SnowMan.h"
#include "Elf.h"
#include "Deer.h"
#include "Robot.h"
#include "Boss.h"

namespace gce {

    void LevelManager::Create()
    {
		if (m_instance != nullptr) return;
		m_instance = new LevelManager();
        m_instance->m_levels.PushBack({ "res/Scene/Level1.json", "Niveau 1", 50 });
        m_instance->m_levels.PushBack({ "res/Scene/Level2.json", "Niveau 2", 5 });
        m_instance->m_levels.PushBack({ "res/Scene/Level3.json", "Niveau 3", 30 });
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
		if (index == 1)
		{
			LoadLevel2();
		}
		if (index == 2)
		{
			LoadLevel3();
		}
    }

    void LevelManager::LoadLevel1()
    {
        RessourcesManager::GetPlayer()->GetGameObject()->transform.SetWorldPosition({ 0.f, 30.f, 0.f });  
        RessourcesManager::GetPlayer()->m_weaponLevel = 1;
        RessourcesManager::GetPlayer()->UpdateWeapon(true);
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

	void LevelManager::LoadLevel2()
	{
		RessourcesManager::GetPlayer()->GetGameObject()->transform.SetWorldPosition({ 0.f, 30.f, 0.f });
		if (m_instance->m_levels.Empty()) { return; }
		int creditLeft = m_instance->m_levels[0].ennemyCredit;

		int enemyCosts[] = { 1, 3 }; // Elf, Robot

		while (creditLeft > 0)
		{
			gce::Vector<int> affordableEnemies;
			if (creditLeft >= enemyCosts[0]) affordableEnemies.PushBack(0); // Elf
			if (creditLeft >= enemyCosts[1]) affordableEnemies.PushBack(1); // Robot

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
				Elf* elf = new Elf(&enemyObject, RessourcesManager::GetTileMap());
				RessourcesManager::AddEnnemy(elf);
				break;
			}
			case 1:
			{
				Robot* robot = new Robot(&enemyObject);
				RessourcesManager::AddEnnemy(robot);
				break;
			}
			}
			creditLeft -= enemyCost;
		}
	}

    void LevelManager::LoadLevel3()
    {
        RessourcesManager::GetPlayer()->GetGameObject()->transform.SetWorldPosition({ 0.f, 30.f, 0.f });
        RessourcesManager::GetPlayer()->m_weaponLevel = 3;
        RessourcesManager::GetPlayer()->UpdateWeapon(true);
        if (m_instance->m_levels.Empty()) { return; }
        int creditLeft = m_instance->m_levels[2].ennemyCredit;

        Scene& scene = const_cast<Scene&>(*RessourcesManager::GetPlayer()->GetGameObject()->GetScene());

            GameObject& bossObject = GameObject::Create(scene);
            bossObject.SetActive(false);

            Boss* boss = new Boss(&bossObject, RessourcesManager::GetTileMap());

            
            RessourcesManager::AddEnnemy(boss);
            RessourcesManager::SpawnEnnemies(10.0f);

        // 0: SnowMan (1), 1: Elf (1), 2: Deer (3), 3: Robot (3)
        int enemyCosts[] = { 1, 1, 3, 3 };

        while (creditLeft > 0)
        {
            gce::Vector<int> affordableEnemies;
            if (creditLeft >= enemyCosts[0]) affordableEnemies.PushBack(0); // SnowMan
            if (creditLeft >= enemyCosts[1]) affordableEnemies.PushBack(1); // Elf
            if (creditLeft >= enemyCosts[2]) affordableEnemies.PushBack(2); // Deer
            if (creditLeft >= enemyCosts[3]) affordableEnemies.PushBack(3); // Robot

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
                Elf* elf = new Elf(&enemyObject, RessourcesManager::GetTileMap());
                RessourcesManager::AddEnnemy(elf);
                break;
            }
            case 2:
            {
                Deer* deer = new Deer(&enemyObject, RessourcesManager::GetTileMap());
                RessourcesManager::AddEnnemy(deer);
                break;
            }
            case 3:
            {
                Robot* robot = new Robot(&enemyObject);
                RessourcesManager::AddEnnemy(robot);
                break;
            }
            }

            creditLeft -= enemyCost;
        }
    }
}