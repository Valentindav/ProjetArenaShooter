#pragma once
#include "Containers/Vector.hpp"
namespace gce {

    struct LevelData {
        String jsonPath;
        String name;
        int ennemyCredit;
    };

    class LevelManager
    {
    public:
        static void Create();

        static void LoadLevel(int index);
        static void LoadNextLevel();
        static void LoadLevel1();
		static void LoadLevel3();

        static int GetCurrentLevelIndex() { return m_instance ? m_instance->m_currentIndex : -1; }
		static int GetMaxEnemy() { return m_maxEnemy; }

    private:
		inline static LevelManager* m_instance = nullptr;
        Vector<LevelData> m_levels;
        int m_currentIndex = -1;
		inline static int m_maxEnemy = 10;
    };
}