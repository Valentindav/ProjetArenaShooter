#include "MenuManager.h"
#include "RessourcesManager.h"
#include "Player.h"

MenuManager* MenuManager::m_Instance = nullptr;

DECLARE_SCRIPT(GameStateChecker, ScriptFlag::Update)
public:
    void Update()
    {
        MenuManager* menuManager = MenuManager::GetInstance();
        if (!menuManager) return;

        if (menuManager->GetGameState() == GameState::Playing)
        {
            Player* player = RessourcesManager::GetPlayer();
            if (player && player->GetGameObject() && !player->GetGameObject()->IsActive())
            {
                menuManager->SetGameState(GameState::GameOver);
                menuManager->ShowGameOverMenu();
            }

            gce::Vector<Entity*> entities = RessourcesManager::getEntities();
            int aliveCount = 0;
            for (Entity* entity : entities)
            {
                if (entity && entity->GetGameObject() && entity->GetGameObject()->IsActive())
                {
                    Player* playerCheck = dynamic_cast<Player*>(entity);
                    if (playerCheck == nullptr)
                    {
                        aliveCount++;
                    }
                }
            }

            if (aliveCount == 0)
            {
                menuManager->SetGameState(GameState::Victory);
                menuManager->ShowVictoryMenu();
            }
        }
    }
    END_SCRIPT

        MenuManager::MenuManager() : m_currentState(GameState::MainMenu), m_scene(nullptr),
        m_startButton(nullptr),
        m_mainMenuPanel(nullptr), m_gameOverPanel(nullptr), m_victoryPanel(nullptr)
    {
    }

    void MenuManager::Create(Scene* scene)
    {
        if (m_Instance != nullptr) return;
        m_Instance = new MenuManager();
        m_Instance->m_scene = scene;
        m_Instance->CreateMainMenu();
        m_Instance->CreateGameOverMenu();
        m_Instance->CreateVictoryMenu();
        m_Instance->ShowMainMenu();
    }

    void MenuManager::OnStartButtonClick()
    {
        if (m_Instance)
        {
            m_Instance->StartGame();
        }
    }

    void MenuManager::CreateMainMenu()
    {
        if (!m_scene) return;

        m_mainMenuPanel = &GameObject::Create(*m_scene);
        m_mainMenuPanel->SetName("MainMenuPanel");

        m_startButton = &GameObject::Create(*m_scene);
        m_startButton->SetName("StartButton");
        m_mainMenuPanel->AddChild(*m_startButton);

        UiButton* pStartButton = m_startButton->AddComponent<UiButton>();
        BitMapBrush* startBrush = new BitMapBrush("res/Exemple/TexturesTest.jpg");
        pStartButton->pBitMapBrush = startBrush;
        pStartButton->AddListener(OnStartButtonClick);
    }

    void MenuManager::CreateGameOverMenu()
    {
        if (!m_scene) return;

        m_gameOverPanel = &GameObject::Create(*m_scene);
        m_gameOverPanel->SetName("GameOverPanel");

        m_gameOverPanel->SetActive(false);
    }

    void MenuManager::CreateVictoryMenu()
    {
        if (!m_scene) return;

        m_victoryPanel = &GameObject::Create(*m_scene);
        m_victoryPanel->SetName("VictoryPanel");

        m_victoryPanel->SetActive(false);
    }

    void MenuManager::ShowMainMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(true);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
        gce::UnlockMouseCursor();
    }

    void MenuManager::ShowGameOverMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(true);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
        gce::UnlockMouseCursor();
    }

    void MenuManager::ShowVictoryMenu()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(true);
        gce::UnlockMouseCursor();
    }

    void MenuManager::HideAllMenus()
    {
        if (m_mainMenuPanel) m_mainMenuPanel->SetActive(false);
        if (m_gameOverPanel) m_gameOverPanel->SetActive(false);
        if (m_victoryPanel) m_victoryPanel->SetActive(false);
    }

    void MenuManager::SetGameState(GameState state)
    {
        m_currentState = state;
    }

    void MenuManager::StartGame()
    {
        m_currentState = GameState::Playing;
        HideAllMenus();

        GameObject* stateChecker = &GameObject::Create(*m_scene);
        stateChecker->SetName("GameStateChecker");
        stateChecker->AddScript<GameStateChecker>();
    }