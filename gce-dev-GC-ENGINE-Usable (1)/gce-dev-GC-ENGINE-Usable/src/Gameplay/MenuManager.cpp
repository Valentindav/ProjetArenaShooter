#include "MenuManager.h"
#include "RessourcesManager.h"
#include "Player.h"
#include "SnowMan.h"

MenuManager* MenuManager::m_Instance = nullptr;

DECLARE_SCRIPT(GameStateChecker, ScriptFlag::Update)
public:
    void Update()
    {
        MenuManager* menuManager = MenuManager::GetInstance();
        if (!menuManager) return;

        if (GetKeyDown(Keyboard::ESCAPE))
        {
            if (menuManager->GetGameState() == GameState::Playing)
            {
                menuManager->PauseGame();
            }
            else if (menuManager->GetGameState() == GameState::Paused)
            {
                menuManager->ResumeGame();
            }
        }

        if (menuManager->GetGameState() == GameState::Playing)
        {
            Player* player = RessourcesManager::GetPlayer();

            if (player == nullptr || player->GetGameObject() == nullptr || !player->GetGameObject()->IsActive())
            {
                menuManager->SetGameState(GameState::GameOver);
                menuManager->ShowGameOverMenu();
            }

            gce::Vector<Entity*> entities = RessourcesManager::getEntities();
            int enemyCount = 0;
            for (Entity* entity : entities)
            {
                if (entity == nullptr) continue;
                if (entity == player) continue;

                if (dynamic_cast<Ennemy*>(entity) != nullptr)
                {
                    enemyCount++;
                }
            }

            if (enemyCount == 0)
            {
                menuManager->SetGameState(GameState::Victory);
                menuManager->ShowVictoryMenu();
            }
        }
    }
    END_SCRIPT

        MenuManager::MenuManager() : m_currentState(GameState::MainMenu), m_scene(nullptr),
        m_CameraObject(nullptr), pCamera(nullptr),
        m_mainMenuPanel(nullptr), m_playButton(nullptr), m_quitButton(nullptr),
        m_pauseMenuPanel(nullptr), m_resumeButton(nullptr), m_restartButtonPause(nullptr), m_mainMenuButtonPause(nullptr),
        m_gameOverPanel(nullptr), m_gameOverBackground(nullptr), m_gameOverText(nullptr), m_restartButtonGameOver(nullptr), m_mainMenuButtonGameOver(nullptr),
        m_victoryPanel(nullptr), m_victoryBackground(nullptr), m_victoryText(nullptr), m_restartButtonVictory(nullptr), m_mainMenuButtonVictory(nullptr),
        m_hudPanel(nullptr), m_candyCaneUI(nullptr)
    {
    }

    void MenuManager::Create(Scene* scene)
    {
        if (m_Instance != nullptr) return;
        m_Instance = new MenuManager();
        m_Instance->m_scene = scene;

        m_Instance->m_CameraObject = &GameObject::Create(*scene);
        m_Instance->m_CameraObject->SetName("MenuCamera");
        m_Instance->m_CameraObject->transform.LocalTranslate({ 0, 0, -10 });

        m_Instance->pCamera = m_Instance->m_CameraObject->AddComponent<Camera>();
        m_Instance->pCamera->SetMainCamera();
        m_Instance->pCamera->SetType(PERSPECTIVE);
        m_Instance->pCamera->perspective.fov = XM_PIDIV4;
        m_Instance->pCamera->perspective.nearPlane = 0.001f;
        m_Instance->pCamera->perspective.farPlane = 500.0f;
        m_Instance->pCamera->perspective.aspectRatio = 1000.0f / 800.0f;
        m_Instance->pCamera->perspective.up = { 0.0f, 1.0f, 0.0f };

        m_Instance->CreateMainMenu();
        m_Instance->CreatePauseMenu();
        m_Instance->CreateGameOverMenu();
        m_Instance->CreateVictoryMenu();
        m_Instance->CreateHUD();

        GameObject* stateChecker = &GameObject::Create(*scene);
        stateChecker->SetName("GameStateChecker");
        stateChecker->AddScript<GameStateChecker>();

        m_Instance->ShowMainMenu();
    }

    void MenuManager::OnPlayButtonClick()
    {
        if (m_Instance) m_Instance->StartGame();
    }

    void MenuManager::OnQuitButtonClick()
    {
        if (m_Instance) m_Instance->QuitGame();
    }

    void MenuManager::OnResumeButtonClick()
    {
        if (m_Instance) m_Instance->ResumeGame();
    }

    void MenuManager::OnRestartButtonClick()
    {
        if (m_Instance) m_Instance->RestartGame();
    }

    void MenuManager::OnMainMenuButtonClick()
    {
        if (m_Instance) m_Instance->ReturnToMainMenu();
    }

    void MenuManager::CreateMainMenu()
    {
        if (!m_scene) return;

        m_mainMenuPanel = &GameObject::Create(*m_scene);
        m_mainMenuPanel->SetName("MainMenuPanel");
        m_mainMenuPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });

        {
            m_playButton = &GameObject::Create(*m_scene);
            m_playButton->transform.LocalTranslate({ 200.0f, 50.0f, 0.0f });
            m_playButton->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_playButton->AddComponent<UiButton>();
            button->AddListener(OnPlayButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_mainMenuPanel->AddChild(*m_playButton);
        }

        {
            m_quitButton = &GameObject::Create(*m_scene);
            m_quitButton->transform.LocalTranslate({ 200.0f, 150.0f, 0.0f });
            m_quitButton->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_quitButton->AddComponent<UiButton>();
            button->AddListener(OnQuitButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_mainMenuPanel->AddChild(*m_quitButton);
        }
    }

    void MenuManager::CreatePauseMenu()
    {
        if (!m_scene) return;
        m_pauseMenuPanel = &GameObject::Create(*m_scene);
        m_pauseMenuPanel->SetName("PauseMenuPanel");
        m_pauseMenuPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });

        {
            m_resumeButton = &GameObject::Create(*m_scene);
            m_resumeButton->transform.LocalTranslate({ 400.0f, 50.0f, 0.0f });
            m_resumeButton->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_resumeButton->AddComponent<UiButton>();
            button->AddListener(OnResumeButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_pauseMenuPanel->AddChild(*m_resumeButton);
        }

        {
            m_restartButtonPause = &GameObject::Create(*m_scene);
            m_restartButtonPause->transform.LocalTranslate({ 400.0f, 150.0f, 0.0f });
            m_restartButtonPause->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_restartButtonPause->AddComponent<UiButton>();
            button->AddListener(OnRestartButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_pauseMenuPanel->AddChild(*m_restartButtonPause);
        }

        {
            m_mainMenuButtonPause = &GameObject::Create(*m_scene);
            m_mainMenuButtonPause->transform.LocalTranslate({ 400.0f, 250.0f, 0.0f });
            m_mainMenuButtonPause->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_mainMenuButtonPause->AddComponent<UiButton>();
            button->AddListener(OnMainMenuButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_pauseMenuPanel->AddChild(*m_mainMenuButtonPause);
        }

        m_pauseMenuPanel->SetActive(false);
    }

    void MenuManager::CreateGameOverMenu()
    {
        if (!m_scene) return;
        m_gameOverPanel = &GameObject::Create(*m_scene);
        m_gameOverPanel->SetName("GameOverPanel");
        m_gameOverPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });

        {
            m_restartButtonGameOver = &GameObject::Create(*m_scene);
            m_restartButtonGameOver->transform.LocalTranslate({ 600.0f, 50.0f, 0.0f });
            m_restartButtonGameOver->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_restartButtonGameOver->AddComponent<UiButton>();
            button->AddListener(OnRestartButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_gameOverPanel->AddChild(*m_restartButtonGameOver);
        }

        {
            m_mainMenuButtonGameOver = &GameObject::Create(*m_scene);
            m_mainMenuButtonGameOver->transform.LocalTranslate({ 600.0f, 150.0f, 0.0f });
            m_mainMenuButtonGameOver->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_mainMenuButtonGameOver->AddComponent<UiButton>();
            button->AddListener(OnMainMenuButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_gameOverPanel->AddChild(*m_mainMenuButtonGameOver);
        }

        m_gameOverPanel->SetActive(false);
    }

    void MenuManager::CreateVictoryMenu()
    {
        if (!m_scene) return;
        m_victoryPanel = &GameObject::Create(*m_scene);
        m_victoryPanel->SetName("VictoryPanel");
        m_victoryPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });

        {
            m_restartButtonVictory = &GameObject::Create(*m_scene);
            m_restartButtonVictory->transform.LocalTranslate({ 600.0f, 50.0f, 0.0f });
            m_restartButtonVictory->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_restartButtonVictory->AddComponent<UiButton>();
            button->AddListener(OnRestartButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_victoryPanel->AddChild(*m_restartButtonVictory);
        }

        {
            m_mainMenuButtonVictory = &GameObject::Create(*m_scene);
            m_mainMenuButtonVictory->transform.LocalTranslate({ 600.0f, 150.0f, 0.0f });
            m_mainMenuButtonVictory->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_mainMenuButtonVictory->AddComponent<UiButton>();
            button->AddListener(OnMainMenuButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/crosshair.png");
            m_victoryPanel->AddChild(*m_mainMenuButtonVictory);
        }

        m_victoryPanel->SetActive(false);
    }

    void MenuManager::CreateHUD()
    {
        if (!m_scene) return;

        m_hudPanel = &GameObject::Create(*m_scene);
        m_hudPanel->SetName("HUDPanel");

        m_candyCaneUI = &GameObject::Create(*m_scene);
        m_candyCaneUI->SetName("CandyCaneUI");
        m_candyCaneUI->transform.SetWorldPosition({ -8.0f, 4.5f, -9.0f });
        m_candyCaneUI->transform.LocalScale({ 2.0f, 0.3f, 0.1f });

        MeshRenderer* pCandyRenderer = m_candyCaneUI->AddComponent<MeshRenderer>();
        pCandyRenderer->SetGeometry(SHAPES.CUBE);
        Texture* pCandyTexture = new Texture("res/Textures/hp_full.png");
        pCandyRenderer->SetAlbedoTexture(pCandyTexture);

        m_hudPanel->AddChild(*m_candyCaneUI);
        m_hudPanel->SetActive(false);
    }

    void MenuManager::UpdateHealthUI(float currentHealth, float maxHealth)
    {
        if (!m_candyCaneUI) return;

        float healthPercentage = currentHealth / maxHealth;
        if (healthPercentage < 0.0f) healthPercentage = 0.0f;
        if (healthPercentage > 1.0f) healthPercentage = 1.0f;

        MeshRenderer* pRenderer = m_candyCaneUI->GetComponent<MeshRenderer>();
        if (!pRenderer) return;

        if (healthPercentage <= 0.0f)
        {
            Texture* pEmptyTexture = new Texture("res/Textures/hp_empty.png");
            pRenderer->SetAlbedoTexture(pEmptyTexture);
        }
        else
        {
            Texture* pFullTexture = new Texture("Textures/hp_full.png");
            pRenderer->SetAlbedoTexture(pFullTexture);
        }

        float scaleX = 2.0f * healthPercentage;
        if (scaleX < 0.1f) scaleX = 0.1f;

        m_candyCaneUI->transform.SetLocalScale({ scaleX, 0.3f, 0.1f });
    }

    void MenuManager::ShowMainMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(true);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
        if (m_hudPanel) m_hudPanel->SetActive(false);
        gce::UnlockMouseCursor();
    }

    void MenuManager::ShowPauseMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(true);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
        gce::UnlockMouseCursor();
    }

    void MenuManager::ShowGameOverMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(true);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
        if (m_hudPanel) m_hudPanel->SetActive(false);
        gce::UnlockMouseCursor();
    }

    void MenuManager::ShowVictoryMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(true);
        if (m_hudPanel) m_hudPanel->SetActive(false);
        gce::UnlockMouseCursor();
    }

    void MenuManager::HideAllMenus()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
        if (m_hudPanel) m_hudPanel->SetActive(false);
    }

    void MenuManager::SetGameState(GameState state)
    {
        m_currentState = state;
    }

    void MenuManager::StartGame()
    {
        if (m_currentState == GameState::Playing) return;
        m_currentState = GameState::Playing;
        HideAllMenus();

        if (m_hudPanel) m_hudPanel->SetActive(true);

        if (m_CameraObject)
        {
            m_CameraObject->SetActive(true);
        }

        GameObject& PlayerObject = GameObject::Create(*m_scene);
        Light* light = PlayerObject.AddComponent<Light>();
        light->DefaultDirectionLight();
        light->intensity = 1.0f;
        PlayerObject.transform.SetWorldPosition({ 0.0f, 0.0f, -10.0f });

        GameObject& Weapon = GameObject::Create(*m_scene);
        MeshRenderer* pWeaponRenderer = Weapon.AddComponent<MeshRenderer>();
        pWeaponRenderer->SetGeometry(GeometryFactory::LoadGeometry("res/Exemple/bottle.obj"));
        Weapon.transform.LocalScale({ 0.03, 0.03, 0.03 });
        Weapon.transform.SetWorldPosition({ 1.0f, 0.0f, -8.0f });
        Weapon.SetName("Weapon_1");

        Player* player = new Player(&PlayerObject, 2);
        player->GetGameObject()->AddChild(*m_CameraObject);
        player->GetGameObject()->AddChild(Weapon);

        m_CameraObject->transform.LocalTranslate({ 0, 0, 0 });
        RessourcesManager::SetPlayer(player);

        UpdateHealthUI(player->m_life, 13.0f);

        GameObject& Floor = GameObject::Create(*m_scene);
        Floor.transform.SetWorldPosition({ -5.0f, -10.0f, -5.0f });
        MeshRenderer* pFloorRenderer = Floor.AddComponent<MeshRenderer>();
        pFloorRenderer->SetGeometry(SHAPES.CUBE);
        Floor.transform.LocalScale({ 20.f, 1.f, 20.f });
        Floor.AddComponent<BoxCollider>()->SetActive(true);
        Floor.SetName("Floor");

        GameObject& SnowManObject = GameObject::Create(*m_scene);
        SnowManObject.transform.SetWorldPosition({ 1.0f, -5.0f, 1.0f });
        SnowManObject.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
        SnowMan* Snowman = new SnowMan(&SnowManObject, RessourcesManager::GetTileMap());

        GameObject& SnowManObject2 = GameObject::Create(*m_scene);
        SnowManObject2.transform.SetWorldPosition({ -3.0f, 0.0f, 3.0f });
        SnowManObject2.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
        SnowMan* Snowman2 = new SnowMan(&SnowManObject2, RessourcesManager::GetTileMap());

        GameObject& SnowManObject3 = GameObject::Create(*m_scene);
        SnowManObject3.transform.SetWorldPosition({ 3.0f, 0.0f, 3.0f });
        SnowManObject3.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
        SnowMan* Snowman3 = new SnowMan(&SnowManObject3, RessourcesManager::GetTileMap());
    }

    void MenuManager::PauseGame()
    {
        m_currentState = GameState::Paused;
        ShowPauseMenu();
    }

    void MenuManager::ResumeGame()
    {
        m_currentState = GameState::Playing;
        HideAllMenus();
        if (m_hudPanel) m_hudPanel->SetActive(true);
    }

    void MenuManager::RecreateScene()
    {
    }

    void MenuManager::RestartGame()
    {
        exit(0);
    }

    void MenuManager::ReturnToMainMenu()
    {
        exit(0);
    }

    void MenuManager::QuitGame()
    {
        exit(0);
    }