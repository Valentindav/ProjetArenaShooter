#include <windows.h>
#include <Engine.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    gce::Console::Init();
    gce::GameManager::Create();

    gce::Scene& scene = gce::Scene::Create();

    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1280;
    params.height = 720;
    params.isFullScreen = false;
    params.isSplitScreen = false;
    params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;

    gce::GameManager::Run(params);

    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}
