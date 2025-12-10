#include <windows.h>
#include <Engine.h>
#include "RessourcesManager.h"
#include "MenuManager.h"

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

    // 4. On crée UNIQUEMENT le MenuManager ici.
    MenuManager::Create(&scene);

    // 5. Lancement de la boucle de jeu
    gce::GameManager::Run(params);

    // 6. Nettoyage
    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}