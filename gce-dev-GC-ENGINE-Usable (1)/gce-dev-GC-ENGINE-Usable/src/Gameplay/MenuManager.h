#pragma once
#include <Engine.h>

using namespace gce;

enum class GameState {
    MainMenu,
    Playing,
    GameOver,
    Victory
};

class MenuManager {
private:
    static MenuManager* m_Instance;
    GameState m_currentState;
    Scene* m_scene;

    GameObject* m_startButton;

    GameObject* m_mainMenuPanel;
    GameObject* m_gameOverPanel;
    GameObject* m_victoryPanel;

    MenuManager();

public:
    static void Create(Scene* scene);
    static MenuManager* GetInstance() { return m_Instance; }

    void SetGameState(GameState state);
    GameState GetGameState() const { return m_currentState; }

    void CreateMainMenu();
    void CreateGameOverMenu();
    void CreateVictoryMenu();

    void ShowMainMenu();
    void ShowGameOverMenu();
    void ShowVictoryMenu();
    void HideAllMenus();

    static void OnStartButtonClick();

    void StartGame();
};