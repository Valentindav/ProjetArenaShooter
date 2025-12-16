#pragma once
#include <Engine.h>

using namespace gce;

enum class GameState
{
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Victory
};

class MenuManager
{
private:
    static MenuManager* m_Instance;
    GameState m_currentState;
    Scene* m_scene;
    GameObject* m_CameraObject;
    Camera* pCamera;

    GameObject* m_mainMenuPanel;
    GameObject* m_playButton;
    GameObject* m_quitButton;

    GameObject* m_pauseMenuPanel;
    GameObject* m_resumeButton;
    GameObject* m_restartButtonPause;
    GameObject* m_mainMenuButtonPause;

    GameObject* m_gameOverPanel;
    GameObject* m_gameOverBackground;
    GameObject* m_gameOverText;
    GameObject* m_restartButtonGameOver;
    GameObject* m_mainMenuButtonGameOver;

    GameObject* m_victoryPanel;
    GameObject* m_victoryBackground;
    GameObject* m_victoryText;
    GameObject* m_restartButtonVictory;
    GameObject* m_mainMenuButtonVictory;

    GameObject* m_hudPanel;
    GameObject* m_candyCaneUI;

    MenuManager();
    void RecreateScene();

public:
    static void Create(Scene* scene);
    static MenuManager* GetInstance() { return m_Instance; }

    void SetGameState(GameState state);
    GameState GetGameState() const { return m_currentState; }
    GameObject* GetCamera() { return m_CameraObject; }

    void CreateMainMenu();
    void CreatePauseMenu();
    void CreateGameOverMenu();
    void CreateVictoryMenu();
    void CreateHUD();

    void ShowMainMenu();
    void ShowPauseMenu();
    void ShowGameOverMenu();
    void ShowVictoryMenu();
    void HideAllMenus();
    void UpdateHealthUI(float currentHealth, float maxHealth);

    static void OnPlayButtonClick();
    static void OnQuitButtonClick();
    static void OnResumeButtonClick();
    static void OnRestartButtonClick();
    static void OnMainMenuButtonClick();

    void StartGame();
    void PauseGame();
    void ResumeGame();
    void RestartGame();
    void ReturnToMainMenu();
    void QuitGame();
};