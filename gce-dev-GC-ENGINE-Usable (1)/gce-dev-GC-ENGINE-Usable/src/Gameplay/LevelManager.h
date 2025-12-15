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
        static void Init();

        static void LoadLevel(int index);
        static void LoadNextLevel();

        static int GetCurrentLevelIndex() { return m_currentIndex; }

    private:
        static Vector<LevelData> m_levels;
        static int m_currentIndex;
    };

}

