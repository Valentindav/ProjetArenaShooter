#include "LevelManager.h"
#include "RessourcesManager.h"
#include "JsonImporter.hpp"

namespace gce {

    Vector<LevelData> LevelManager::m_levels;
    int LevelManager::m_currentIndex = -1;

    void LevelManager::Init()
    {
        m_levels.PushBack({ "res/Scene/Level1.json", "Niveau 1" });
        m_levels.PushBack({ "res/Scene/Level2.json", "Niveau 2" });
        m_levels.PushBack({ "res/Scene/Level3.json", "Niveau 3" });
    }

    void LevelManager::LoadNextLevel()
    {
        int next = m_currentIndex + 1;
        if (next >= m_levels.Size())
        {
            next = 0;
        }
        LoadLevel(next);
    }

    void LevelManager::LoadLevel(int index)
    {
        if (index < 0 || index >= m_levels.Size()) return;

        m_currentIndex = index;
        const auto& level = m_levels[index];
     
        RessourcesManager::ClearCurrentLevel();

        auto importedScene = importSceneFromJsonText("res/Scene/T_Shape with hitbox.json");

        for (auto& [name, pObj] : importedScene)
        {
            RessourcesManager::AddLevelObject(pObj);
		}

        //Player* p = RessourcesManager::GetPlayer();
        //if (p && p->GetGameObject())
        //{
        //    // Tu peux définir une position de spawn dans ton JSON ou en dur
        //    p->GetGameObject()->transform.SetLocalPosition({ 0.f, 2.f, 0.f });

        //    // On s'assure qu'il est actif
        //    p->GetGameObject()->SetActive(true);
        //}
    }

}