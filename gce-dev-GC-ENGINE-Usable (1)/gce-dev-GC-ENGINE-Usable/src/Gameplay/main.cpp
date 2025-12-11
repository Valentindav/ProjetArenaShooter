#include <windows.h>
#include <Engine.h>
#include "RessourcesManager.h"
#include "Player.h"
#include "MenuManager.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    gce::Console::Init();
    gce::GameManager::Create();
    RessourcesManager::Create();
    gce::Scene& scene = gce::Scene::Create();
    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1920;
    params.height = 1080;
    params.isFullScreen = true;
    params.isSplitScreen = false;
    params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;

    MenuManager::Create(&scene);
    gce::GameManager::Run(params);
    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;

}