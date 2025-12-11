#include <windows.h>
#include <Engine.h>
#include "Player.h"
#include "SnowMan.h"
#include "Robot.h"
#include "RessourcesManager.h"
#include "TileMap.h"
#include "MenuManager.h"
#include "JsonImporter.hpp"

/*TODO
- Pathfinding A*
- state Machine for ennemies
- other enemies
- other weapons

*/
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) 
{	
    //----------------------------------INIT WORLD----------------------------------
    gce::Console::Init();
    gce::GameManager::Create();
    RessourcesManager::Create();
    gce::Scene& scene = gce::Scene::Create();
	TileMap tileMap(50, 50, 1.5f, scene, { 0.f, 0.f, 0.f });

    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1920;
    params.height = 1080;
    params.isFullScreen = true;
    params.isSplitScreen = false;
    params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;

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

    //----------------------------------INIT GameObject----------------------------------
    GameObject& PlayerObject = GameObject::Create(scene);
    Light* light = PlayerObject.AddComponent<Light>();
    // Correction : Enregistrement et d�sactivation propre de la lumi�re du joueur
    gce::LightManager::AddLight(*light);
    light->DefaultDirectionLight();
    light->intensity = 0.0f; 
    light->UpdateLight();

    GameObject& RobotObject = GameObject::Create(scene);

    GameObject& testObject = GameObject::Create(scene);
    MeshRenderer* pMeshRenderer = testObject.AddComponent<MeshRenderer>();
    pMeshRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pNewTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pMeshRenderer->SetAlbedoTexture(pNewTexture);
    testObject.AddComponent<BoxCollider>()->SetActive(true);
    testObject.AddComponent<PhysicComponent>();
    testObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
    testObject.SetName("TestObject");

    GameObject& Weapon = GameObject::Create(scene);
    Weapon.transform.SetWorldPosition({ .0f,.0f,.0f });
    MeshRenderer* pWeaponRenderer = Weapon.AddComponent<MeshRenderer>();
    pWeaponRenderer->SetGeometry(GeometryFactory::LoadGeometry("res/Exemple/bottle.obj"));
    Weapon.transform.LocalScale({ 0.03,0.03,0.03 });

    GameObject& Floor = GameObject::Create(scene);
    Floor.transform.SetWorldPosition({ -5.0f,-10.0f,-5.0f });
    MeshRenderer* pFloorRenderer = Floor.AddComponent<MeshRenderer>();
    pFloorRenderer->SetGeometry(SHAPES.CUBE);
    pFloorRenderer->SetAlbedoTexture(pNewTexture);
    Floor.transform.LocalScale({ 200.f,1.f,200.f });
    Floor.AddComponent<BoxCollider>()->SetActive(true);
	Floor.SetName("Floor");
    
    Light* light2 = Floor.AddComponent<Light>();
    // Correction : Enregistrement et d�sactivation propre de la lumi�re du sol
    gce::LightManager::AddLight(*light2);
    light2->DefaultDirectionLight();
    light2->intensity = 0.0f;
    light2->UpdateLight();

    // Ajout d'une lumi�re directionnelle venant du haut pour �clairer toute la sc�ne
    GameObject& LightAbove = GameObject::Create(scene);
    Light* pLightAbove = LightAbove.AddComponent<Light>();
    gce::LightManager::AddLight(*pLightAbove);
    pLightAbove->DefaultDirectionLight();
    pLightAbove->direction = { 0.0f, -1.0f, 0.0f }; // Pointe vers le bas
    LightAbove.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f }); 
    pLightAbove->intensity = 1.f; 
    pLightAbove->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    pLightAbove->UpdateLight();

    // Ajout d'une lumi�re ponctuelle sur un GameObject "Light" au-dessus de la sc�ne
    GameObject& SceneLight = GameObject::Create(scene);
    SceneLight.SetName("Light");
    SceneLight.transform.SetWorldPosition({ 0.0f, 5.0f, 0.0f });
    Light* pSceneLight = SceneLight.AddComponent<Light>();
    gce::LightManager::AddLight(*pSceneLight);
    pSceneLight->DefaultPointLight();
    pSceneLight->intensity = 1.0f;
    pSceneLight->range = 20.0f;
    pSceneLight->UpdateLight();

	tileMap.DebugMode(scene);

    //----------------------------------TestWorld----------------------------------

    // Importation de la sc�ne JSON et ajout des BoxCollider pour visualisation
    /*auto importedScene = importSceneFromJsonText("res/Scene/SceneTest6.json");*/
    //for (auto& [name, obj] : importedScene) if (obj) obj->AddComponent<BoxCollider>();

    //----------------------------------Run----------------------------------
    testObject.transform.SetWorldPosition({ -2.0f,3.0f,0.0f });
    PlayerObject.transform.SetWorldPosition({ 0.0f,0.f,-10.0f });
    Weapon.transform.SetWorldPosition({ 1.0f,0.0f,-8.0f });
	Weapon.SetName("Weapon_1");

    RobotObject.transform.SetWorldPosition({ 10.0f,-9.0f,3.0f });
    RobotObject.transform.SetWorldRotation({ 00.0f,0.0f,0.0f });
    Robot* robot = new Robot(&RobotObject);

    GameObject& SnowManObject = GameObject::Create(scene);
    SnowManObject.transform.SetWorldPosition({ 1.0f,0.0f,1.0f });
    SnowManObject.transform.SetWorldRotation({ 90.0f,0.0f,0.0f });
    SnowMan* Snowman = new SnowMan(&SnowManObject, &tileMap);
    RessourcesManager::AddEntities(Snowman);

    GameObject& SnowManObject2 = GameObject::Create(scene);
    SnowManObject2.transform.SetWorldPosition({ -3.0f, 0.0f, 3.0f });
    SnowManObject2.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
    SnowMan* Snowman2 = new SnowMan(&SnowManObject2, &tileMap);
    RessourcesManager::AddEntities(Snowman2);

    GameObject& SnowManObject3 = GameObject::Create(scene);
    SnowManObject3.transform.SetWorldPosition({ 3.0f, 0.0f, 3.0f });
    SnowManObject3.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });
    SnowMan* Snowman3 = new SnowMan(&SnowManObject3, &tileMap);
    RessourcesManager::AddEntities(Snowman3);

    //SnowManObject.transform.SetWorldPosition({ -10.0f,-9.0f,-5.0f });
    //SnowManObject.transform.SetWorldRotation({ 0.0f,0.0f,0.0f });
    Player* player = new Player(&PlayerObject);
    player->GetGameObject()->AddChild(CameraObject);
    player->GetGameObject()->AddChild(Weapon);
    RessourcesManager::SetPlayer(player);

    MenuManager::Create(&scene);    

    GameObject& testObject2 = GameObject::Create(scene);
	testObject2.transform.SetWorldPosition({ .0f,-10.0f,0.0f });
	MeshRenderer* pMeshRenderer2 = testObject2.AddComponent<MeshRenderer>();
	pMeshRenderer2->SetGeometry(SHAPES.CUBE);
	pMeshRenderer2->SetAlbedoTexture(pNewTexture);		
    testObject2.AddComponent<BoxCollider>()->SetActive(true);
    tileMap.SetWalkableWithCollider(*testObject2.GetComponent<BoxCollider>(), false);
    std::cout << tileMap.GetNodeFromWorldPosition(testObject2.transform.GetWorldPosition())->data->walkable << std::endl;
	std::cout << tileMap.GetNodeFromWorldPosition(testObject2.transform.GetWorldPosition())->data->gridX << ", " << tileMap.GetNodeFromWorldPosition(testObject2.transform.GetWorldPosition())->data->gridY << std::endl;

    gce::GameManager::Run(params);
    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}