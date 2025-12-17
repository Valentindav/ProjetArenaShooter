#include "LevelManager.h"
#include "RessourcesManager.h"
#include "JsonImporter.hpp"

namespace gce {

    void LevelManager::Create()
    {
		if (m_instance != nullptr) return;
		m_instance = new LevelManager();
        m_instance->m_levels.PushBack({ "res/Scene/Level1.json", "Niveau 1" });
        m_instance->m_levels.PushBack({ "res/Scene/Level2.json", "Niveau 2" });
        m_instance->m_levels.PushBack({ "res/Scene/Level3.json", "Niveau 3" });
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
    }

}