#pragma once
#include <Engine.h>

using namespace gce;

class Player;

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
    static MenuManager* m_instance;
    GameState m_currentState;
    Scene* m_scene;

    GameObject* m_cameraObject;
    Camera* pCamera;
    bool m_isGameInitialized = false;
    GameObject* m_floorObject = nullptr;
    GameObject* m_crosshairObject = nullptr;

    GameObject* m_mainMenuPanel;
    GameObject* m_mainText;
    GameObject* m_playButton;
    GameObject* m_quitButton;

    GameObject* m_pauseMenuPanel;
    GameObject* m_pauseText;
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


    MenuManager();


public:



    static void Create(Scene* scene);
    static MenuManager* GetInstance() 
    {
        return m_instance; 
    }

    void SetGameState(GameState state);
    GameState GetGameState() const 
    {
        return m_currentState; 
    }

    GameObject* GetCamera() { return m_cameraObject; }

    void CreateMainMenu();
    void CreatePauseMenu();
    void CreateGameOverMenu();
    void CreateVictoryMenu();

    void ShowMainMenu();
    void ShowPauseMenu();
    void ShowGameOverMenu();
    void ShowVictoryMenu();
    void HideAllMenus();

    static void OnPlayButtonClick();
    static void OnQuitButtonClick();
    static void OnResumeButtonClick();
    static void OnRestartButtonClick();
    static void OnMainMenuButtonClick();

    void StartGame();
    UiImage* m_uiStaminaBar[11] = { nullptr };
    void ShowUIStaminaBar();
    UiImage* m_uiLifeBars[13] = { nullptr };
    void ShowUIHealthBar();
    void PauseGame();
    void ResumeGame();
    void RestartGame();
    void ReturnToMainMenu();
    void QuitGame();
};