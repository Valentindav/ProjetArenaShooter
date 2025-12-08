#include <windows.h>
#include <Engine.h>
#include "Player.h"
#include "SnowMan.h"
#include "RessourcesManager.h"
#include "TileMap.h"


/*TODO
- Pathfinding A*
- state Machine for ennemies
- other enemies
- other weapons

*/
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {	
    //----------------------------------INIT WORLD----------------------------------
    gce::Console::Init();
    gce::GameManager::Create();    
    RessourcesManager::Create();

    gce::Scene& scene = gce::Scene::Create();

    TileMap::CreateInstance(50, 50, 0.5f, { -12.5f, -12.5f, 0.0f });
    //TileMap* tileMap = TileMap::Instance();

    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1920;
    params.height = 1080;
    params.isFullScreen = true;
    params.isSplitScreen = false;
    params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;

    //----------------------------------INIT Camera----------------------------------
	gce::GameObject& CameraObject = gce::GameObject::Create(scene);
    CameraObject.transform.LocalTranslate({ 0,0, -10 });
    gce::Camera* pCamera = CameraObject.AddComponent<gce::Camera>();
    pCamera->SetMainCamera();
    pCamera->SetType(gce::PERSPECTIVE);
    pCamera->perspective.fov = XM_PIDIV4;
    pCamera->perspective.nearPlane = 0.001f;
    pCamera->perspective.farPlane = 500.0f;
    pCamera->perspective.aspectRatio = 1000.0f / 800.0f;
    pCamera->perspective.up = { 0.0f, 1.0f, 0.0f };

    //----------------------------------Creating GameObject----------------------------------
    GameObject& PlayerObject = GameObject::Create(scene);
    Light* light = PlayerObject.AddComponent<Light>();
    light->DefaultDirectionLight();
    light->intensity = 1.0f;

    GameObject& SnowManObject = GameObject::Create(scene);

    GameObject& testObject = GameObject::Create(scene);
    MeshRenderer* pMeshRenderer = testObject.AddComponent<MeshRenderer>();
    pMeshRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pNewTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pMeshRenderer->SetAlbedoTexture(pNewTexture);
    testObject.AddComponent<BoxCollider>()->SetActive(true);
    testObject.AddComponent<PhysicComponent>();
	testObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);

   GameObject& Weapon = GameObject::Create(scene);
    Weapon.transform.SetWorldPosition({ .0f,.0f,.0f });
    MeshRenderer* pWeaponRenderer = Weapon.AddComponent<MeshRenderer>();
    pWeaponRenderer->SetGeometry(GeometryFactory::LoadGeometry("res/Exemple/bottle.obj"));
    Weapon.transform.LocalScale({ 0.03,0.03,0.03 });

    GameObject& Floor = GameObject::Create(scene);
    Floor.transform.SetWorldPosition({ -5.0f,-10.0f,-5.0f });
    MeshRenderer* pFloorRenderer = Floor.AddComponent<MeshRenderer>();
    pFloorRenderer->SetGeometry(SHAPES.CUBE);
    Floor.transform.LocalScale({ 20.f,1.f,20.f });
    Floor.AddComponent<BoxCollider>()->SetActive(true);
    Floor.AddComponent<PhysicComponent>()->SetGravityScale(.0f);
    Floor.AddComponent<PhysicComponent>()->SetMass(10000000.f);   
	Floor.SetName("Floor");

    //----------------------------------Run----------------------------------

    testObject.transform.SetWorldPosition({ -2.0f,3.0f,0.0f });
    PlayerObject.transform.SetWorldPosition({ 0.0f,0.f,-10.0f });
    Weapon.transform.SetWorldPosition({ 1.0f,0.0f,-8.0f });
	Weapon.SetName("Weapon_1");

    SnowManObject.transform.SetWorldPosition({ 1.0f,0.0f,1.0f });

	SnowMan* Snowman = new SnowMan(&SnowManObject);
	Player* player = new Player(&PlayerObject);
    player->GetGameObject()->AddChild(CameraObject);
    player->GetGameObject()->AddChild(Weapon);
	RessourcesManager::SetPlayer(player);
    RessourcesManager::AddEntities(player);
    gce::GameManager::Run(params);	

    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}
