#include "MenuManager.h"
#include <windows.h>
#include <Engine.h>
#include "Player.h"
#include "Controller.h"
#include "SnowMan.h"
#include "Robot.h"
#include "RessourcesManager.h"
#include "JsonImporter.hpp"
#include "Elf.h"
#include "Deer.h"
#include "Boss.h"
#include "RayCast.h"
#include "TileMap.h"
#include "Heal.h"
#include "AudioManager.h"

MenuManager* MenuManager::m_instance = nullptr;

DECLARE_SCRIPT(GameStateChecker, ScriptFlag::Update)
private:
    int aliveCount = 0;
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
            menuManager->ShowUIHealthBar();
            menuManager->ShowUIStaminaBar();

            Player* player = RessourcesManager::GetPlayer();

            if (player == nullptr || player->GetGameObject() == nullptr || player->m_life <= 0)
            {
                menuManager->SetGameState(GameState::GameOver);
                menuManager->ShowGameOverMenu();
                return; 
            }

            gce::Vector<Entity*> entities = RessourcesManager::GetEntities();
            int enemyCount = 0;
            Boss* currentBoss = nullptr;

            for (Entity* entity : entities)
            {
                if (entity == nullptr || entity == player) continue;

                GameObject* obj = entity->GetGameObject();
                if (!obj) continue;

                if (obj->GetName() == "Bullet") continue;

                Boss* b = dynamic_cast<Boss*>(entity);
                if (b) {
                    currentBoss = b;
                    enemyCount++; 
                    continue;
                }

                if (dynamic_cast<Ennemy*>(entity))
                {
                    enemyCount++;
                }
            }

            if (currentBoss)
            {
                menuManager->UpdateBossHealthBar(currentBoss->m_life, currentBoss->m_baseLife);
            }
            else
            {
                menuManager->UpdateBossHealthBar(0, 100);
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
        m_cameraObject(nullptr), pCamera(nullptr),
        m_mainMenuPanel(nullptr), m_playButton(nullptr), m_quitButton(nullptr),
        m_pauseMenuPanel(nullptr), m_resumeButton(nullptr), m_restartButtonPause(nullptr), m_mainMenuButtonPause(nullptr),
        m_gameOverPanel(nullptr), m_gameOverBackground(nullptr), m_gameOverText(nullptr), m_restartButtonGameOver(nullptr), m_mainMenuButtonGameOver(nullptr),
        m_victoryPanel(nullptr), m_victoryBackground(nullptr), m_victoryText(nullptr), m_restartButtonVictory(nullptr), m_mainMenuButtonVictory(nullptr)
    {
    }

    void MenuManager::Create(Scene* scene) // create the menu m_instance
    {
        if (m_instance != nullptr) return;
        m_instance = new MenuManager();
        m_instance->m_scene = scene;

        m_instance->m_cameraObject = &GameObject::Create(*scene);
        m_instance->m_cameraObject->SetName("Camera");
        m_instance->m_cameraObject->transform.LocalTranslate({ 0, 0, -10 });

        m_instance->pCamera = m_instance->m_cameraObject->AddComponent<Camera>();
        m_instance->pCamera->SetMainCamera();
        m_instance->pCamera->SetType(PERSPECTIVE);
        m_instance->pCamera->perspective.fov = XM_PIDIV4;
        m_instance->pCamera->perspective.nearPlane = 0.001f;
        m_instance->pCamera->perspective.farPlane = 500.0f;
        m_instance->pCamera->perspective.aspectRatio = 1000.0f / 800.0f;
        m_instance->pCamera->perspective.up = { 0.0f, 1.0f, 0.0f };

        m_instance->CreateMainMenu();
        m_instance->CreatePauseMenu();
        m_instance->CreateGameOverMenu();
        m_instance->CreateVictoryMenu();

        GameObject* stateChecker = &GameObject::Create(*scene);
        stateChecker->SetName("GameStateChecker");
        stateChecker->AddScript<GameStateChecker>();

        m_instance->ShowMainMenu();
    }

    void MenuManager::OnPlayButtonClick() // start button behavior
    {
        if (m_instance) m_instance->StartGame();
    }

    void MenuManager::OnQuitButtonClick() // quit button behavior
    {
        if (m_instance) m_instance->QuitGame();
    }

    void MenuManager::OnResumeButtonClick() // resume button behavior
    {
        if (m_instance) m_instance->ResumeGame();
    }

    void MenuManager::OnRestartButtonClick() // restart button behavior
    { 
        if (m_instance) m_instance->RestartGame();
    }

    void MenuManager::OnMainMenuButtonClick() // menu button behavior
    {
        if (m_instance) m_instance->ReturnToMainMenu();
    }

    // --- création boutons ---
    void MenuManager::CreateMainMenu()
    {
        gce::WindowParam params;
        if (!m_scene) return;
        
        m_mainMenuPanel = &GameObject::Create(*m_scene);
        m_mainMenuPanel->SetName("MainMenuPanel");
        m_mainMenuPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });

        //{
        //    gce::GameObject& BackGround = gce::GameObject::Create(*m_scene);
        //    gce::UiImage& uiBackGround = *BackGround.AddComponent<gce::UiImage>();

        //    gce::Vector2f32 center = { (float)params.width, (float)params.height };
        //    gce::Vector2f32 size = { 155, 148 };
        //    gce::Vector2f32 posUi = center - size * 0.5f;

        //    uiBackGround.InitializeImage(posUi, size, 1.f);
        //    uiBackGround.btmBrush = new gce::BitMapBrush("res/Textures/Title/Fond_Vert.jpg");

        //    float scaleX = 1920.f / 155;
        //    float scaleY = 1080.f / 148;
        //    uiBackGround.btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, scaleY, 1.f }, 0.f);
        //}

        {
            m_mainText = &GameObject::Create(*m_scene);
            UiImage& uiMainText = *m_mainText->AddComponent<UiImage>();
            Vector2f32 center = { (float)params.width + 100, (float)params.height * 0.7f - 150 };
            Vector2f32 size = { 1508.f, 208.f };
            Vector2f32 posUi = center - size * 0.5f;
            uiMainText.InitializeImage(posUi, size, 1.f);
            uiMainText.btmBrush = new BitMapBrush("res/Textures/Title/Jingle_Hell.png");
            float scaleX = 600.f / 1508.f;
            float scaleY = 150.f / 208.f;
            uiMainText.btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, scaleY, 1.f }, 0.f);
            if (m_mainMenuPanel)
            {
                m_mainMenuPanel->AddChild(*m_mainText);
            }

        }

        {
            m_playButton = &GameObject::Create(*m_scene);
            m_playButton->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f, 1.0f });
            m_playButton->transform.LocalScale({ 300.0f, 100.0f, 1.0f });
            UiButton* button = m_playButton->AddComponent<UiButton>();
            button->AddListener(OnPlayButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/play.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/play.png");
            m_mainMenuPanel->AddChild(*m_playButton);
        }

        {
            m_quitButton = &GameObject::Create(*m_scene);
            m_quitButton->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f + 150, 1.0f });
            m_quitButton->transform.LocalScale({ 300.0f, 100.0f, 1.0f });
            UiButton* button = m_quitButton->AddComponent<UiButton>();
            button->AddListener(OnQuitButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/quit.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/quit.png");
            m_mainMenuPanel->AddChild(*m_quitButton);
        }
    }

    void MenuManager::CreatePauseMenu()
    {
        gce::WindowParam params;
        if (!m_scene) return;
        m_pauseMenuPanel = &GameObject::Create(*m_scene);
        m_pauseMenuPanel->SetName("PauseMenuPanel");
        m_pauseMenuPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });


        {
            m_pauseText = &GameObject::Create(*m_scene);
            UiImage& uiPauseText = *m_pauseText->AddComponent<UiImage>();
            Vector2f32 center = { (float)params.width * 0.75f, (float)params.height * 0.7f - 150 };
            Vector2f32 size = { 792.f, 253.f };
            Vector2f32 posUi = center - size * 0.5f;
            uiPauseText.InitializeImage(posUi, size, 1.f);
            uiPauseText.btmBrush = new BitMapBrush("res/Textures/Title/Pause.png");
            float scaleX = 600.f / 792.f;
            float scaleY = 150.f / 253.f;
            uiPauseText.btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, scaleY, 1.f }, 0.f);
            if (m_pauseMenuPanel)
            {
                m_pauseMenuPanel->AddChild(*m_pauseText);
            }
        }

        {
            m_resumeButton = &GameObject::Create(*m_scene);
            m_resumeButton->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f, 1.0f });
            m_resumeButton->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_resumeButton->AddComponent<UiButton>();
            button->AddListener(OnResumeButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/resume.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/resume.png");
            m_pauseMenuPanel->AddChild(*m_resumeButton);
        }

        {
            m_restartButtonPause = &GameObject::Create(*m_scene);
            m_restartButtonPause->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f + 150, 1.0f });
            m_restartButtonPause->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_restartButtonPause->AddComponent<UiButton>();
            button->AddListener(OnRestartButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/retry.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/retry.png");
            m_pauseMenuPanel->AddChild(*m_restartButtonPause);
        }

        {
            m_mainMenuButtonPause = &GameObject::Create(*m_scene);
            m_mainMenuButtonPause->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f + 300, 1.0f });
            m_mainMenuButtonPause->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_mainMenuButtonPause->AddComponent<UiButton>();
            button->AddListener(OnMainMenuButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/main_menu.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/main_menu.png");
            m_pauseMenuPanel->AddChild(*m_mainMenuButtonPause);
        }

        m_pauseMenuPanel->SetActive(false);
    }

    void MenuManager::CreateGameOverMenu()// create game over menu
    {
        gce::WindowParam params;
        if (!m_scene) return;
        m_gameOverPanel = &GameObject::Create(*m_scene);
        m_gameOverPanel->SetName("GameOverPanel");
        m_gameOverPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });


        {
            m_gameOverText = &GameObject::Create(*m_scene);
            UiImage& uiGameOverText = *m_gameOverText->AddComponent<UiImage>();
            Vector2f32 center = { (float)params.width * 0.75f, (float)params.height * 0.7f - 150 };
            Vector2f32 size = { 964.f, 223.f };
            Vector2f32 posUi = center - size * 0.5f;
            uiGameOverText.InitializeImage(posUi, size, 1.f);
            uiGameOverText.btmBrush = new BitMapBrush("res/Textures/Title/Pause.png");
            float scaleX = 600.f / 964.f;
            float scaleY = 150.f / 223.f;
            uiGameOverText.btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, scaleY, 1.f }, 0.f);
            if (m_gameOverPanel)
            {
                m_gameOverPanel->AddChild(*m_gameOverText);
            }
        }

    {
        m_restartButtonGameOver = &GameObject::Create(*m_scene);
        m_restartButtonGameOver->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f, 1.0f });
        m_restartButtonGameOver->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
        UiButton* button = m_restartButtonGameOver->AddComponent<UiButton>();
        button->AddListener(OnRestartButtonClick);
        button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/retry.png");
        button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/retry.png");
        m_gameOverPanel->AddChild(*m_restartButtonGameOver);
    }

    {
        m_mainMenuButtonGameOver = &GameObject::Create(*m_scene);
        m_mainMenuButtonGameOver->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f + 150, 1.0f });
        m_mainMenuButtonGameOver->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
        UiButton* button = m_mainMenuButtonGameOver->AddComponent<UiButton>();
        button->AddListener(OnMainMenuButtonClick);
        button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/main_menu.png");
        button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/main_menu.png");
        m_gameOverPanel->AddChild(*m_mainMenuButtonGameOver);
    }
        m_gameOverPanel->SetActive(false);
    }

    void MenuManager::CreateVictoryMenu()// create victory menu
    {
        gce::WindowParam params;
        if (!m_scene) return;
        m_victoryPanel = &GameObject::Create(*m_scene);
        m_victoryPanel->SetName("VictoryPanel");
        m_victoryPanel->transform.SetWorldPosition({ 0.0f, 0.0f, -8.0f });

        {
            m_victoryText = &GameObject::Create(*m_scene);
            UiImage& uiVictoryText = *m_victoryText->AddComponent<UiImage>();
            Vector2f32 center = { (float)params.width * 0.75f, (float)params.height * 0.7f - 150 };
            Vector2f32 size = { 1066.f, 225.f };
            Vector2f32 posUi = center - size * 0.5f;
            uiVictoryText.InitializeImage(posUi, size, 1.f);
            uiVictoryText.btmBrush = new BitMapBrush("res/Textures/Title/Pause.png");
            float scaleX = 600.f / 1066.f;
            float scaleY = 150.f / 225.f;
            uiVictoryText.btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, scaleY, 1.f }, 0.f);
            if (m_victoryPanel)
            {
                m_victoryPanel->AddChild(*m_victoryText);
            }
        }

        {
            m_restartButtonVictory = &GameObject::Create(*m_scene);
            m_restartButtonVictory->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f, 1.0f });
            m_restartButtonVictory->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_restartButtonVictory->AddComponent<UiButton>();
            button->AddListener(OnRestartButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/retry.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/retry.png");
            m_victoryPanel->AddChild(*m_restartButtonVictory);
        }

        {
            m_mainMenuButtonVictory = &GameObject::Create(*m_scene);
            m_mainMenuButtonVictory->transform.LocalTranslate({ (float)params.width * 0.75f, (float)params.height * 0.7f + 150, 1.0f });
            m_mainMenuButtonVictory->transform.LocalScale({ 216.0f, 69.0f, 1.0f });
            UiButton* button = m_mainMenuButtonVictory->AddComponent<UiButton>();
            button->AddListener(OnMainMenuButtonClick);
            button->pBitMapBrush = new BitMapBrush("res/Textures/Buttons/main_menu.png");
            button->pHoverBitMapBrush = new BitMapBrush("res/Textures/Buttons_Pressed/main_menu.png");
            m_victoryPanel->AddChild(*m_mainMenuButtonVictory);
        }
        m_victoryPanel->SetActive(false);
    }

// --- création affichage ---
        void MenuManager::ShowMainMenu()
        {
            m_currentState = GameState::MainMenu;

            HideAllMenus();

            if (m_mainMenuPanel)
                m_mainMenuPanel->SetActive(true); 

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
        gce::UnlockMouseCursor();
    }

    void MenuManager::ShowVictoryMenu() 
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(true);
        gce::UnlockMouseCursor();
    }

    void MenuManager::HideAllMenus() 
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
    }

    void MenuManager::SetGameState(GameState state) 
    {
        m_currentState = state; 
    }

    void MenuManager::StartGame()
    {
        if (m_isGameInitialized)
        {
            m_currentState = GameState::Playing;
            HideAllMenus();

            if (m_cameraObject) m_cameraObject->SetActive(true);
            if (m_floorObject) m_floorObject->SetActive(true);
            if (m_crosshairObject) m_crosshairObject->SetActive(true);

            Player* player = RessourcesManager::GetPlayer();
            if (player && player->GetGameObject()) 
            {
                player->GetGameObject()->SetActive(true);
                player->GetGameObject()->transform.SetWorldPosition({ 0.0f, 2.0f, -10.0f });

                player->m_life = 14.0f;
                player->m_energy = 11.0f;
            }

            gce::Vector<Entity*> entities = RessourcesManager::GetEntities();
            for (Entity* entity : entities) 
            {
                if (entity && entity->GetGameObject())
                {
                    entity->GetGameObject()->SetActive(true);
                   
                }
            }

            gce::LockMouseCursor();
            return;
        }

        m_currentState = GameState::Playing;
        HideAllMenus();
        if (m_cameraObject) m_cameraObject->SetActive(true);
        m_isGameInitialized = true;
        gce::LockMouseCursor();

        GameObject& PlayerObject = GameObject::Create(*m_scene);
        Light* light = PlayerObject.AddComponent<Light>();
        gce::LightManager::AddLight(*light);
        light->DefaultDirectionLight();
        light->intensity = 0.0f;
        light->UpdateLight();

        GameObject& RobotObject = GameObject::Create(*m_scene);
        Texture* pNewTexture = new Texture("res/Exemple/TexturesTest.jpg");

        GameObject& Weapon = GameObject::Create(*m_scene);
        Weapon.transform.SetWorldPosition({ .0f,.0f,.0f });
        MeshRenderer* pWeaponRenderer = Weapon.AddComponent<MeshRenderer>();
        pWeaponRenderer->SetGeometry(RessourcesManager::GetGift());
        Weapon.SetName("Weapon");
        Weapon.transform.LocalScale({ 0.03,0.03,0.03 });

        m_cameraObject->transform.LocalTranslate({ 0,0,0 });

        GameObject& RayCastObj = GameObject::Create(*m_scene);

        m_floorObject = &GameObject::Create(*m_scene);
        m_floorObject->transform.SetWorldPosition({ -5.0f,-10.0f,-5.0f });
        MeshRenderer* pFloorRenderer = m_floorObject->AddComponent<MeshRenderer>();
        pFloorRenderer->SetGeometry(SHAPES.CUBE);
        pFloorRenderer->SetAlbedoTexture(pNewTexture);
        m_floorObject->transform.LocalScale({ 200.f,1.f,200.f });
        m_floorObject->AddComponent<BoxCollider>()->SetActive(true);
        m_floorObject->SetName("Floor");

        Light* light2 = m_floorObject->AddComponent<Light>();
        gce::LightManager::AddLight(*light2);
        light2->DefaultDirectionLight();
        light2->intensity = 0.0f;
        light2->UpdateLight();

        GameObject& LightAbove = GameObject::Create(*m_scene);
        Light* pLightAbove = LightAbove.AddComponent<Light>();
        gce::LightManager::AddLight(*pLightAbove);
        pLightAbove->DefaultDirectionLight();
        pLightAbove->direction = { 0.0f, -1.0f, 0.0f };
        LightAbove.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
        pLightAbove->intensity = 1.f;
        pLightAbove->color = { 1.0f, 1.0f, 1.0f, 1.0f };
        pLightAbove->UpdateLight();

        RessourcesManager::CreateMaterials(
            "res/Textures/Reactor_BaseColor.png",
            "res/Textures/Reactor_Normal.png",
            "res/Textures/Reactor_Metallic.png",
            "res/Textures/Reactor_Roughness.png",
            "res/Textures/Reactor_Displacement.png"
        );

        PlayerObject.transform.SetWorldPosition({ 0.0f,0.f,-10.0f });
        Weapon.transform.SetWorldPosition({ 1.0f,-0.3f,-8.f });
        Weapon.SetName("Weapon_1");

        gce::WindowParam params;
        params.width = 1920;
        params.height = 1080;

        RobotObject.transform.SetWorldPosition({ 10.0f,8.0f,3.0f });
        Robot* robot = new Robot(&RobotObject);

        GameObject& SnowManObject = GameObject::Create(*m_scene);
        SnowManObject.transform.SetWorldPosition({ 1.0f,0.0f,1.0f });
        new SnowMan(&SnowManObject, RessourcesManager::GetTileMap());

        GameObject& bossObject = GameObject::Create(*m_scene);
        bossObject.transform.SetWorldPosition({ 2.0f, 0.0f, 1.0f });
        bossObject.SetName("Boss");
        new Boss(&bossObject, RessourcesManager::GetTileMap());

        CreateBossUI();

        Heal* heal = new Heal(&GameObject::Create(*m_scene), 150.0f);
        heal->GetGameObject()->transform.SetWorldPosition({ 2.0f, 0.0f, 2.0f });

        GameObject& EldObject = GameObject::Create(*m_scene);
        EldObject.transform.SetWorldPosition({ 1.0f, 0.0f, 1.0f });
        new Elf(&EldObject, RessourcesManager::GetTileMap());

        new RayCast(&RayCastObj, m_cameraObject->transform.GetLocalPosition().z);

        Player* player = new Player(&PlayerObject, 2);
        player->GetGameObject()->AddChild(*m_cameraObject);
        player->GetGameObject()->AddChild(Weapon);
        m_cameraObject->AddChild(RayCastObj);
        player->m_weaponOriginalPos = Weapon.transform.GetLocalPosition();
        RessourcesManager::SetPlayer(player);
        player->SetCamera(m_cameraObject);

        m_crosshairObject = &gce::GameObject::Create(*m_scene);
        gce::UiImage& uiCrosshair = *m_crosshairObject->AddComponent<gce::UiImage>();
        gce::Vector2f32 center = { (float)params.width / 2.f, (float)params.height / 2.f };
        gce::Vector2f32 size = { 64.f, 64.f };
        gce::Vector2f32 posUi = center - size * 0.5f;
        uiCrosshair.InitializeImage(posUi, size, 1.f);
        uiCrosshair.btmBrush = new gce::BitMapBrush("res/Textures/UI/crosshair.png");
        uiCrosshair.SetActive(true);

        GameObject& AudioManagerObj = GameObject::Create(*m_scene);
        AudioManagerObj.AddScript<AudioManager>();

        m_isGameInitialized = true;
        gce::LockMouseCursor();

        gce::Vector2f32 center3 = { 576.f, (float)params.height - 192.f };
        gce::Vector2f32 size3 = { 1117.f, 301.f };
        gce::Vector2f32 posUi3 = center3 - size3 * 0.5f;
        float scaleX2 = 750.f / 1117.f;
        float scaleY2 = 225.f / 301.f;


        gce::GameObject& staminaBar = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar = *staminaBar.AddComponent<gce::UiImage>();
        staminaBar.SetName("UI_Stamina");
        uiStaminaBar.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_empty.png");
        uiStaminaBar.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[0] = &uiStaminaBar;

        gce::GameObject& staminaBar1 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar1 = *staminaBar1.AddComponent<gce::UiImage>();
        staminaBar1.SetName("UI_Stamina_1");
        uiStaminaBar1.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar1.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_1.png");
        uiStaminaBar1.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[1] = &uiStaminaBar1;

        gce::GameObject& staminaBar2 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar2 = *staminaBar2.AddComponent<gce::UiImage>();
        staminaBar2.SetName("UI_Stamina_2");
        uiStaminaBar2.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar2.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_2.png");
        uiStaminaBar2.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[2] = &uiStaminaBar2;

        gce::GameObject& staminaBar3 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar3 = *staminaBar3.AddComponent<gce::UiImage>();
        staminaBar3.SetName("UI_Stamina_3");
        uiStaminaBar3.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar3.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_3.png");
        uiStaminaBar3.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[3] = &uiStaminaBar3;

        gce::GameObject& staminaBar4 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar4 = *staminaBar4.AddComponent<gce::UiImage>();
        staminaBar4.SetName("UI_Stamina_4");
        uiStaminaBar4.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar4.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_4.png");
        uiStaminaBar4.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[4] = &uiStaminaBar4;

        gce::GameObject& staminaBar5 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar5 = *staminaBar.AddComponent<gce::UiImage>();
        staminaBar5.SetName("UI_Stamina_5");
        uiStaminaBar5.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar5.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_5.png");
        uiStaminaBar5.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[5] = &uiStaminaBar5;

        gce::GameObject& staminaBar6 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar6 = *staminaBar6.AddComponent<gce::UiImage>();
        staminaBar6.SetName("UI_Stamina_6");
        uiStaminaBar6.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar6.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_6.png");
        uiStaminaBar6.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[6] = &uiStaminaBar6;

        gce::GameObject& staminaBar7 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar7 = *staminaBar7.AddComponent<gce::UiImage>();
        staminaBar7.SetName("UI_Stamina_7");
        uiStaminaBar7.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar7.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_7.png");
        uiStaminaBar7.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[7] = &uiStaminaBar7;

        gce::GameObject& staminaBar8 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar8 = *staminaBar8.AddComponent<gce::UiImage>();
        staminaBar8.SetName("UI_Stamina_8");
        uiStaminaBar8.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar8.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_8.png");
        uiStaminaBar8.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[8] = &uiStaminaBar8;

        gce::GameObject& staminaBar9 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar9 = *staminaBar.AddComponent<gce::UiImage>();
        staminaBar9.SetName("UI_Stamina_9");
        uiStaminaBar9.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar9.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_9.png");
        uiStaminaBar9.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[9] = &uiStaminaBar9;

        gce::GameObject& staminaBar10 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiStaminaBar10 = *staminaBar10.AddComponent<gce::UiImage>();
        staminaBar10.SetName("UI_Stamina_10");
        uiStaminaBar10.InitializeImage(posUi3, size3, 1.f);
        uiStaminaBar10.btmBrush = new gce::BitMapBrush("res/Textures/UI/energy_full.png");
        uiStaminaBar10.btmBrush->SetTransformMatrix({ posUi3.x, posUi3.y, 0.f }, { scaleX2, scaleY2, 1.f }, 0.f);
        m_uiStaminaBar[10] = &uiStaminaBar10;
 
        //ajout de la barre de vie
        gce::Vector2f32 center2 = { 576.f, (float)params.height - 128.f };
        gce::Vector2f32 size2 = { 1117.f, 301.f };
        gce::Vector2f32 posUi2 = center2 - size2 * 0.5f;
        float scaleX1 = 750.f / 1117.f;
        float scaleY1 = 225.f / 301.f;


        gce::GameObject& healthBar = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar = *healthBar.AddComponent<gce::UiImage>();
        healthBar.SetName("UI_Life");
        uiLifeBar.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_empty.png");
        uiLifeBar.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[0] = &uiLifeBar;

		//ajout de la barre de vie 1
        gce::GameObject& healthBar1 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar1 = *healthBar1.AddComponent<gce::UiImage>();
        healthBar1.SetName("UI_Life_1");
        uiLifeBar1.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar1.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_1.png");
        uiLifeBar1.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[1] = &uiLifeBar1;

        //ajout de la barre de vie 2
        gce::GameObject& healthBar2 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar2 = *healthBar2.AddComponent<gce::UiImage>();
        healthBar2.SetName("UI_Life_2");
        uiLifeBar2.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar2.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_2.png");
        uiLifeBar2.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[2] = &uiLifeBar2;

        //ajout de la barre de vie 3
        gce::GameObject& healthBar3 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar3 = *healthBar3.AddComponent<gce::UiImage>();
        healthBar3.SetName("UI_Life_3");
        uiLifeBar3.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar3.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_3.png");
        uiLifeBar3.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[3] = &uiLifeBar3;

		//ajout de la barre de vie 4
        gce::GameObject& healthBar4 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar4 = *healthBar4.AddComponent<gce::UiImage>();
        healthBar4.SetName("UI_Life_4");
        uiLifeBar4.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar4.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_4.png");
        uiLifeBar4.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[4] = &uiLifeBar4;

        //ajout de la barre de vie 5
        gce::GameObject& healthBar5 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar5 = *healthBar5.AddComponent<gce::UiImage>();
        healthBar5.SetName("UI_Life_5");
        uiLifeBar5.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar5.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_5.png");
        uiLifeBar5.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[5] = &uiLifeBar5;

        //ajout de la barre de vie 6
        gce::GameObject& healthBar6 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar6 = *healthBar6.AddComponent<gce::UiImage>();
        healthBar6.SetName("UI_Life_6");
        uiLifeBar6.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar6.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_6.png");
        uiLifeBar6.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[6] = &uiLifeBar6;

        //ajout de la barre de vie 7
        gce::GameObject& healthBar7 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar7 = *healthBar7.AddComponent<gce::UiImage>();
        healthBar7.SetName("UI_Life_7");
        uiLifeBar7.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar7.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_7.png");
        uiLifeBar7.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[7] = &uiLifeBar7;

        //ajout de la barre de vie 8
        gce::GameObject& healthBar8 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar8 = *healthBar8.AddComponent<gce::UiImage>();
        healthBar8.SetName("UI_Life_8");
        uiLifeBar8.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar8.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_8.png");
        uiLifeBar8.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[8] = &uiLifeBar8;

        //ajout de la barre de vie 9
        gce::GameObject& healthBar9 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar9 = *healthBar9.AddComponent<gce::UiImage>();
        healthBar9.SetName("UI_Life_9");
        uiLifeBar9.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar9.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_9.png");
        uiLifeBar9.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[9] = &uiLifeBar9;

        //ajout de la barre de vie 10
        gce::GameObject& healthBar10 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar10 = *healthBar10.AddComponent<gce::UiImage>();
        healthBar10.SetName("UI_Life_10");
        uiLifeBar10.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar10.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_10.png");
        uiLifeBar10.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[10] = &uiLifeBar10;

        //ajout de la barre de vie 11
        gce::GameObject& healthBar11 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar11 = *healthBar11.AddComponent<gce::UiImage>();
        healthBar11.SetName("UI_Life_11");
        uiLifeBar11.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar11.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_11.png");
        uiLifeBar11.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[11] = &uiLifeBar11;

        //ajout de la barre de vie 12
        gce::GameObject& healthBar12 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar12 = *healthBar12.AddComponent<gce::UiImage>();
        healthBar12.SetName("UI_Life_12");
        uiLifeBar12.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar12.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_12.png");
        uiLifeBar12.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[12] = &uiLifeBar12;

        //ajout de la barre de vie 13
        gce::GameObject& healthBar13 = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiLifeBar13 = *healthBar13.AddComponent<gce::UiImage>();
        healthBar13.SetName("UI_Life_13");
        uiLifeBar13.InitializeImage(posUi2, size2, 1.f);
        uiLifeBar13.btmBrush = new gce::BitMapBrush("res/Textures/UI/hp_full.png");
        uiLifeBar13.btmBrush->SetTransformMatrix({ posUi2.x, posUi2.y, 0.f }, { scaleX1, scaleY1, 1.f }, 0.f);
        m_uiLifeBars[13] = &uiLifeBar13;


        uiCrosshair.SetActive(true);
    }

    void MenuManager::ShowUIHealthBar()
    {
        MenuManager* menuManager = MenuManager::GetInstance();
        if (!menuManager) return;
        if (menuManager->GetGameState() != GameState::Playing) return;

        Player* player = RessourcesManager::GetPlayer();
        if (!player) return;
        float hp = player->m_life;
        if (hp < 0.0f) hp = 0.0f;
        if (hp > 14.0f) hp = 14.0f;
        int index = static_cast<int>(hp);

        for (int i = 0; i < 14; ++i)
        {
            if (m_uiLifeBars[i])
            {
                if (i == index)
                    m_uiLifeBars[i]->SetActive(true);
                else
                    m_uiLifeBars[i]->SetActive(false);
            }
        }
    }

    void MenuManager::ShowUIStaminaBar()
    {
        MenuManager* menuManager = MenuManager::GetInstance();
        if (!menuManager) return;
        if (menuManager->GetGameState() != GameState::Playing) return;

        Player* player = RessourcesManager::GetPlayer();
        if (!player) return;
        float nrg = player->m_energy;
        if (nrg < 0.0f) nrg = 0.0f;
        if (nrg > 12.0f) nrg = 12.0f;
        int index = static_cast<int>(nrg);

        for (int i = 0; i < 12; ++i)
        {
            if (m_uiStaminaBar[i])
            {
                if (i == index)
                    m_uiStaminaBar[i]->SetActive(true);
                else
                    m_uiStaminaBar[i]->SetActive(false);
            }
        }
    }

    void MenuManager::CreateBossUI()
    {
        gce::WindowParam params;
        gce::Vector2f32 size = { 1772.f, 188.f };
        gce::Vector2f32 posUi = { (1920.f * 0.5f) - (size.x * 0.5f), 50.f };
        float scaleX1 = 750.f / 1772.f;
        float scaleY1 = 225.f / 188.f;

        gce::GameObject& bossBarEmpty = gce::GameObject::Create(*m_scene);
        m_uiBossBarEmpty = bossBarEmpty.AddComponent<gce::UiImage>();
        bossBarEmpty.SetName("UI_Boss_Life_Empty");
        m_uiBossBarEmpty->InitializeImage(posUi, size, 1.f);
        m_uiBossBarEmpty->btmBrush = new gce::BitMapBrush("res/Textures/UI/boss_bar.png");

        gce::GameObject& bossBarFill = gce::GameObject::Create(*m_scene);
        m_uiBossBarFill = bossBarFill.AddComponent<gce::UiImage>();
        bossBarFill.SetName("UI_Boss_Life_Fill");
        m_uiBossBarFill->InitializeImage(posUi, size, 1.f);
        m_uiBossBarFill->btmBrush = new gce::BitMapBrush("res/Textures/UI/slider.png");

        m_uiBossBarEmpty->SetActive(false);
        m_uiBossBarFill->SetActive(false);
    }

    void MenuManager::UpdateBossHealthBar(float currentHp, float maxHp)
    {
        if (!m_uiBossBarFill || !m_uiBossBarEmpty) return;

        if (currentHp <= 0) {
            m_uiBossBarEmpty->SetActive(false);
            m_uiBossBarFill->SetActive(false);
            return;
        }

        m_uiBossBarEmpty->SetActive(true);
        m_uiBossBarFill->SetActive(true);

        float healthRatio = currentHp / maxHp;
        if (healthRatio < 0.f) healthRatio = 0.f;
        if (healthRatio > 1.f) healthRatio = 1.f;

        gce::Vector2f32 originalSize = { 1772.f, 188.f };
        gce::Vector2f32 newSize = { originalSize.x * healthRatio, originalSize.y };
        gce::Vector2f32 posUi = { (1920.f * 0.5f) - (originalSize.x * 0.5f), 50.f };

        m_uiBossBarFill->InitializeImage(posUi, newSize, 1.f);

        float scaleX = (800.f * healthRatio) / 800.f;
        m_uiBossBarFill->btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, 1.f, 1.f }, 0.f);
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
    }


    void MenuManager::RestartGame()
    {
        m_currentState = GameState::Playing;
        StartGame();
        HideAllMenus();
    }

    void MenuManager::ReturnToMainMenu() 
    {
        m_currentState = GameState::MainMenu;
        ShowMainMenu();
        if (m_floorObject) m_floorObject->SetActive(false);
        if (m_crosshairObject) m_crosshairObject->SetActive(false);
        Player* player = RessourcesManager::GetPlayer();
        if (player && player->GetGameObject()) 
        {
            player->GetGameObject()->SetActive(false);
        }
        if (m_cameraObject) m_cameraObject->SetActive(false);
    }

    void MenuManager::QuitGame() 
    { 
        exit(0);
    }