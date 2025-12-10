#include <windows.h>
#include <Engine.h>
#include "RessourcesManager.h"
#include "Player.h"
#include "MenuManager.h"


static void TestEventButton()
{
    Console::Log("CLICK CLICK CLICK!!!");
}
static void TestEventButton2()
{
    Console::Log("CLACK CLACK CLACK!!!");
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    // 1. Initialisation du moteur
    gce::Console::Init();
    gce::GameManager::Create();
    RessourcesManager::Create();

    // 2. Création de la scène vide
    gce::Scene& scene = gce::Scene::Create();

    // 3. Paramètres de la fenêtre
    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1920;
    params.height = 1080;
    params.isFullScreen = true;
    params.isSplitScreen = false;
    params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;

    MenuManager::Create(&scene);
   /* gce::BitMapBrush Shop{"res/Exemple/TexturesTest.jpg"};
    gce::BitMapBrush hover { "res/Textures/crosshair.png" };
    {
        GameObject& object = GameObject::Create(scene);
        object.transform.LocalTranslate({ 200.0f, 50.0f, 0.0f });
        object.transform.LocalScale({ 216.0f, 69.0f, 1.0f });
        UiButton& button = *object.AddComponent<UiButton>();

        button.AddListener(TestEventButton);
        button.AddListener(TestEventButton2);

        button.pBitMapBrush = &Shop;
        button.pHoverBitMapBrush = &hover;

    }*/
    
    // 5. Lancement de la boucle de jeu
    gce::GameManager::Run(params);

    // 6. Nettoyage
    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;

}