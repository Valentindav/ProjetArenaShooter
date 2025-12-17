#pragma once
#include "Containers/Vector.hpp"
namespace gce {

    struct LevelData {
        String jsonPath;
        String name;
    };

    class LevelManager
    {
    public:
        static void Create();

        static void LoadLevel(int index);
        static void LoadNextLevel();

        int GetCurrentLevelIndex() { return m_currentIndex; }

    private:
		inline static LevelManager* m_instance = nullptr;
        Vector<LevelData> m_levels;
        int m_currentIndex = -1;
    };

}

