#include "MenuManager.h"
#include <windows.h>
#include <Engine.h>
#include "Player.h"
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

MenuManager* MenuManager::m_Instance = nullptr;

DECLARE_SCRIPT(GameStateChecker, ScriptFlag::Update)
private :
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
            Player* player = RessourcesManager::GetPlayer();

            if (player == nullptr || player->GetGameObject() == nullptr)
            {
                menuManager->SetGameState(GameState::GameOver);
                menuManager->ShowGameOverMenu();
            }

            gce::Vector<Entity*> entities = RessourcesManager::GetEntities();
            int enemyCount = 0;
            for (Entity* entity : entities)
            {
                if (entity->GetGameObject()->GetName() == "Bullet") return;
                if (entity == nullptr) return;
                if (entity == player) return;

                if (dynamic_cast<Ennemy*>(entity))
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
        m_victoryPanel(nullptr), m_victoryBackground(nullptr), m_victoryText(nullptr), m_restartButtonVictory(nullptr), m_mainMenuButtonVictory(nullptr)
    {
    }

    void MenuManager::Create(Scene* scene) // create the menu instance
    {
        if (m_Instance != nullptr) return;
        m_Instance = new MenuManager();
        m_Instance->m_scene = scene;

        m_Instance->m_CameraObject = &GameObject::Create(*scene);
        m_Instance->m_CameraObject->SetName("Camera");
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

        GameObject* stateChecker = &GameObject::Create(*scene);
        stateChecker->SetName("GameStateChecker");
        stateChecker->AddScript<GameStateChecker>();

        m_Instance->ShowMainMenu();
    }

    void MenuManager::OnPlayButtonClick() // start button behavior
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

    // --- création boutons ---
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

    void MenuManager::CreateGameOverMenu()// create game over menu
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

    void MenuManager::CreateVictoryMenu()// create victory menu
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

// --- création affichage ---
    void MenuManager::ShowMainMenu()// show main menu
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(true);
        if (m_pauseMenuPanel) m_pauseMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);

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

    void MenuManager::StartGame() // start games
    {
        if (m_currentState == GameState::Playing) return;
        m_currentState = GameState::Playing;
        HideAllMenus();
        if (m_CameraObject)
        {
            m_CameraObject->SetActive(true);
        }


        GameObject& PlayerObject = GameObject::Create(*m_scene);
        Light* light = PlayerObject.AddComponent<Light>();
        // Correction : Enregistrement et dsactivation propre de la lumire du joueur
        gce::LightManager::AddLight(*light);
        light->DefaultDirectionLight();
        light->intensity = 0.0f;
        light->UpdateLight();

        GameObject& RobotObject = GameObject::Create(*m_scene);

        Texture* pNewTexture = new Texture("res/Exemple/TexturesTest.jpg");

        /*GameObject& testObject = GameObject::Create(scene);
        MeshRenderer* pMeshRenderer = testObject.AddComponent<MeshRenderer>();
        pMeshRenderer->SetGeometry(SHAPES.CUBE);
        pMeshRenderer->SetAlbedoTexture(pNewTexture);
        testObject.AddComponent<BoxCollider>()->SetActive(true);
        testObject.AddComponent<PhysicComponent>();
        testObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
        testObject.SetName("TestObject");*/

        GameObject& Weapon = GameObject::Create(*m_scene);
        Weapon.transform.SetWorldPosition({ .0f,.0f,.0f });
        MeshRenderer* pWeaponRenderer = Weapon.AddComponent<MeshRenderer>();
        pWeaponRenderer->SetGeometry(RessourcesManager::GetGift());
		Weapon.SetName("Weapon");
        Weapon.transform.LocalScale({ 0.03,0.03,0.03 });

        m_CameraObject->transform.LocalTranslate({ 0,0,0 });

		GameObject& RayCastObj = GameObject::Create(*m_scene);

        GameObject& Floor = GameObject::Create(*m_scene);
        Floor.transform.SetWorldPosition({ -5.0f,-10.0f,-5.0f });
        MeshRenderer* pFloorRenderer = Floor.AddComponent<MeshRenderer>();
        pFloorRenderer->SetGeometry(SHAPES.CUBE);
        pFloorRenderer->SetAlbedoTexture(pNewTexture);
        Floor.transform.LocalScale({ 200.f,1.f,200.f });
        Floor.AddComponent<BoxCollider>()->SetActive(true);
        Floor.SetName("Floor");

        Light* light2 = Floor.AddComponent<Light>();
        // Correction : Enregistrement et dsactivation propre de la lumire du sol
        gce::LightManager::AddLight(*light2);
        light2->DefaultDirectionLight();
        light2->intensity = 0.0f;
        light2->UpdateLight();

        // Ajout d'une lumire directionnelle venant du haut pour clairer toute la scne
        GameObject& LightAbove = GameObject::Create(*m_scene);
        Light* pLightAbove = LightAbove.AddComponent<Light>();
        gce::LightManager::AddLight(*pLightAbove);
        pLightAbove->DefaultDirectionLight();
        pLightAbove->direction = { 0.0f, -1.0f, 0.0f }; // Pointe vers le bas
        LightAbove.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
        pLightAbove->intensity = 1.f;
        pLightAbove->color = { 1.0f, 1.0f, 1.0f, 1.0f };
        pLightAbove->UpdateLight();

        // Ajout d'une lumire ponctuelle sur un GameObject "Light" au-dessus de la scne
        /*GameObject& SceneLight = GameObject::Create(*m_scene);
        SceneLight.SetName("Light");
        SceneLight.transform.SetWorldPosition({ 0.0f, 5.0f, 0.0f });
        Light* pSceneLight = SceneLight.AddComponent<Light>();
        gce::LightManager::AddLight(*pSceneLight);
        pSceneLight->DefaultPointLight();
        pSceneLight->intensity = 1.0f;
        pSceneLight->range = 20.0f;
        pSceneLight->UpdateLight();*/

        //----------------------------------TestWorld----------------------------------

		/*GameObject& TestWorld = GameObject::Create(*m_scene);
		MeshRenderer* pTestWorldRenderer = TestWorld.AddComponent<MeshRenderer>();
		pTestWorldRenderer->SetGeometry(GeometryFactory::LoadGeometry("res/Obj/Test_I_Shaped.obj"));
		pTestWorldRenderer->SetAlbedoTexture(new Texture("res/Textures/openPBR_shader2_BaseColor.png"));*/

		/*auto testObj = importSceneFromJsonText("res/Scene/Test_I_Shaped.json");*/

        RessourcesManager::CreateMaterials(
            "res/Textures/Reactor_BaseColor.png",
            "res/Textures/Reactor_Normal.png",
            "res/Textures/Reactor_Metallic.png",
            "res/Textures/Reactor_Roughness.png",
            "res/Textures/Reactor_Displacement.png"
		);

  //      auto levelData = importSceneFromJsonText("res/Scene/Test2.json");

  //      for (auto* col : levelData.allColliders) {
  //          RessourcesManager::GetTileMap()->SetWalkableWithCollider(*col, false);
  //      }

  //      Vector3f32 targetPos = levelData.root->transform.GetWorldPosition();
  //      Vector3f32 lightPos = targetPos + Vector3f32(0.0f, 5.0f, 0.0f); // 5 unités au dessus

  //      // 1. Créer le GameObject pour la lumière
  //      GameObject& lightGo = GameObject::Create(*m_scene);

  //      // 2. Positionner le GameObject
  //      lightGo.transform.SetWorldPosition(lightPos);

  //      // 3. Ajouter le composant Light
  //      Light* pLight = lightGo.AddComponent<Light>();

  //      // 4. Initialiser comme une Point Light (lumière omnidirectionnelle)
  //      pLight->DefaultPointLight();

  //      // 5. Personnaliser les propriétés (optionnel mais recommandé)
  //      // Accès direct aux champs de LightData car Light hérite de LightData
  //      pLight->color = { .8f, 0.8f, 0.8f, 1.0f }; // Couleur un peu chaude
  //      pLight->intensity = 0.2f;                   // Intensité
  //      pLight->range = 20.0f;                      // Rayon d'action
  //      pLight->position = lightPos;                // IMPORTANT : Mettre à jour la position dans la structure de données

  //      // 6. Enregistrer la lumière dans le manager
  //      LightManager::AddLight(*pLight);

		//RessourcesManager::SetupLevelData(levelData);

        //----------------------------------Run----------------------------------
        //testObject.transform.SetWorldPosition({ -2.0f,3.0f,0.0f });
        PlayerObject.transform.SetWorldPosition({ 0.0f,0.f,-10.0f });
        Weapon.transform.SetWorldPosition({ 1.0f,-0.3f,-8.f });
        Weapon.SetName("Weapon_1");

        gce::WindowParam params;
        params.title = L"GCE Engine Window";
        params.width = 1920;
        params.height = 1080;
        params.isFullScreen = true;
        params.isSplitScreen = false;
        params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;

        RobotObject.transform.SetWorldPosition({10.0f,8.0f,3.0f});
        RobotObject.transform.SetWorldRotation({ 00.0f,0.0f,0.0f });
        Robot* robot = new Robot(&RobotObject);

        GameObject& SnowManObject = GameObject::Create(*m_scene);
        SnowManObject.transform.SetWorldPosition({ 1.0f,0.0f,1.0f });
        SnowMan* Snowman = new SnowMan(&SnowManObject, RessourcesManager::GetTileMap());

		Heal* heal = new Heal(&GameObject::Create(*m_scene), 150.0f);
		heal->GetGameObject()->transform.SetWorldPosition({ 2.0f,0.0f,2.0f });

        /*GameObject& SnowManObject2 = GameObject::Create(*m_scene);
        SnowManObject2.transform.SetWorldPosition({ -3.0f, 0.0f, 3.0f });
        SnowMan* Snowman2 = new SnowMan(&SnowManObject2, RessourcesManager::GetTileMap());

        GameObject& SnowManObject3 = GameObject::Create(*m_scene);
        SnowManObject3.transform.SetWorldPosition({ 3.0f, 0.0f, 3.0f });
        SnowMan* Snowman3 = new SnowMan(&SnowManObject3, RessourcesManager::GetTileMap());

        GameObject& DeerObject = GameObject::Create(*m_scene);
        DeerObject.transform.SetWorldPosition({ 1.0f,10.0f,1.0f });
        Deer* deer = new Deer(&DeerObject, RessourcesManager::GetTileMap());*/

        GameObject& EldObject = GameObject::Create(*m_scene);
        EldObject.transform.SetWorldPosition({ 1.0f,0.0f,1.0f });
        Elf* elf = new Elf(&EldObject, RessourcesManager::GetTileMap());

       /* GameObject& BossObject = GameObject::Create(*m_scene);
        BossObject.transform.SetWorldPosition({ 1.0f,0.0f,1.0f });
        Boss* boss = new Boss(&BossObject, RessourcesManager::GetTileMap());*/

		RayCast* raycast = new RayCast(&RayCastObj, m_CameraObject->transform.GetLocalPosition().z);

        Player* player = new Player(&PlayerObject,2);
        player->GetGameObject()->AddChild(*m_CameraObject);
        player->GetGameObject()->AddChild(Weapon);
		m_CameraObject->AddChild(RayCastObj);
        player->m_weaponOriginalPos = Weapon.transform.GetLocalPosition();
        RessourcesManager::SetPlayer(player);
        player->SetCamera(m_CameraObject);

        // Ajout du crosshair
        gce::GameObject& crosshair = gce::GameObject::Create(*m_scene);
        gce::UiImage& uiImage = *crosshair.AddComponent<gce::UiImage>();

        gce::Vector2f32 center = { (float)params.width / 2.f, (float)params.height / 2.f };
        gce::Vector2f32 size = { 64.f, 64.f };
        gce::Vector2f32 posUi = center - size * 0.5f;

        uiImage.InitializeImage(posUi, size, 1.f);
        uiImage.btmBrush = new gce::BitMapBrush("res/Textures/crosshair.png");

        // Calcul de l'échelle : TailleCible / TailleImage
        float scaleX = 64.f / 224.f;
        float scaleY = 64.f / 221.f;
        uiImage.btmBrush->SetTransformMatrix({ posUi.x, posUi.y, 0.f }, { scaleX, scaleY, 1.f }, 0.f);

        uiImage.SetActive(true);

        GameObject& AudioManagerObj = GameObject::Create(*m_scene);
        AudioManagerObj.AddScript<AudioManager>();
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

    void MenuManager::RecreateScene()
    {
    }

    void MenuManager::RestartGame()
    { 
    }

    void MenuManager::ReturnToMainMenu() 
    { 
    }

    void MenuManager::QuitGame() 
    { 
        exit(0);
    }