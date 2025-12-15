#include <windows.h>
#include <Engine.h>
#include "Player.h"
#include "SnowMan.h"
#include "Robot.h"
#include "RessourcesManager.h"
#include "Player.h"
#include "MenuManager.h"
#include "JsonImporter.hpp"
#include "RayCast.h"
#include "Elf.h"
#include "Deer.h"
#include "Boss.h"

/*TODO
- texturing 
- UIX
- sound effect
- ennemies spawn 
- scene
*/

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) 
{	
    //----------------------------------INIT WORLD----------------------------------
    gce::Console::Init();
    gce::GameManager::Create();
    RessourcesManager::Create();
    gce::Scene& scene = gce::Scene::Create();
    TileMap tileMap(100, 100, .5f, scene, { 0.f, 0.f, 0.f });

	RessourcesManager::SetTileMap(&tileMap);

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