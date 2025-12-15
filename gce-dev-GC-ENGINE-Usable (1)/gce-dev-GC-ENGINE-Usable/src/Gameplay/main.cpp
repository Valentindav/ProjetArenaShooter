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

	bool debugMode = true;
    HideMouseCursor();

    //----------------------------------INIT GameObject----------------------------------
    GameObject& PlayerObject = GameObject::Create(scene);
    Light* light = PlayerObject.AddComponent<Light>();
    // Correction : Enregistrement et dsactivation propre de la lumire du joueur
    gce::LightManager::AddLight(*light);
    light->DefaultDirectionLight();
    light->intensity = 0.0f; 
    light->UpdateLight();

    GameObject& RobotObject = GameObject::Create(scene);

    Texture* pNewTexture = new Texture("res/Exemple/TexturesTest.jpg");

    /*GameObject& testObject = GameObject::Create(scene);
    MeshRenderer* pMeshRenderer = testObject.AddComponent<MeshRenderer>();
    pMeshRenderer->SetGeometry(SHAPES.CUBE);    
    pMeshRenderer->SetAlbedoTexture(pNewTexture);
    testObject.AddComponent<BoxCollider>()->SetActive(true);
    testObject.AddComponent<PhysicComponent>();
    testObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
    testObject.SetName("TestObject");*/

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
    // Correction : Enregistrement et dsactivation propre de la lumire du sol
    gce::LightManager::AddLight(*light2);
    light2->DefaultDirectionLight();
    light2->intensity = 0.0f;
    light2->UpdateLight();

    // Ajout d'une lumire directionnelle venant du haut pour clairer toute la scne
    GameObject& LightAbove = GameObject::Create(scene);
    Light* pLightAbove = LightAbove.AddComponent<Light>();
    gce::LightManager::AddLight(*pLightAbove);
    pLightAbove->DefaultDirectionLight();
    pLightAbove->direction = { 0.0f, -1.0f, 0.0f }; // Pointe vers le bas
    LightAbove.transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f }); 
    pLightAbove->intensity = 1.f; 
    pLightAbove->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    pLightAbove->UpdateLight();

    // Ajout d'une lumire ponctuelle sur un GameObject "Light" au-dessus de la scne
    GameObject& SceneLight = GameObject::Create(scene);
    SceneLight.SetName("Light");
    SceneLight.transform.SetWorldPosition({ 0.0f, 5.0f, 0.0f });
    Light* pSceneLight = SceneLight.AddComponent<Light>();
    gce::LightManager::AddLight(*pSceneLight);
    pSceneLight->DefaultPointLight();
    pSceneLight->intensity = 1.0f;
    pSceneLight->range = 20.0f;
    pSceneLight->UpdateLight();	

    //----------------------------------TestWorld----------------------------------

    // Importation de la scne JSON et ajout des BoxCollider pour visualisation
    auto importedScene = importSceneFromJsonText("res/Scene/T_Shape with hitbox.json");

    // --- AJOUT : Création d'un pivot pour déplacer la scène ---
    gce::GameObject& sceneRoot = gce::GameObject::Create(scene);
    sceneRoot.SetName("SceneAnchor");

    //// On parcourt les objets importés et on attache les "racines" à notre pivot
    for (auto& [name, pObj] : importedScene)
    {
        // Si l'objet est valide et n'a pas de parent (c'est une racine dans le JSON)
        if (pObj && !pObj->HasParent())
        {
            pObj->SetParent(sceneRoot);
        }
    }

    // Maintenant, vous pouvez déplacer/tourner/scaler toute la scène via sceneRoot
    sceneRoot.transform.SetWorldPosition({ 0.0f, -5.0f, 0.0f });
	sceneRoot.transform.SetLocalScale({ 3.0f, 3.0f, 3.0f });
     sceneRoot.transform.SetWorldRotation({ 0.0f, 45.0f, 0.0f });
    // ----------------------------------------------------------


    //----------------------------------Run----------------------------------
    //testObject.transform.SetWorldPosition({ -2.0f,3.0f,0.0f });
    PlayerObject.transform.SetWorldPosition({ 0.0f,0.f,-10.0f });
    Weapon.transform.SetWorldPosition({ 1.0f,0.0f,-8.0f });
	Weapon.SetName("Weapon_1");

    /*RobotObject.transform.SetWorldPosition({ 10.0f,-9.0f,3.0f });
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
    RessourcesManager::AddEntities(Snowman3);*/

    Player* player = new Player(&PlayerObject);
    player->GetGameObject()->AddChild(CameraObject);
    player->GetGameObject()->AddChild(Weapon);
    RessourcesManager::SetPlayer(player);

    MenuManager::Create(&scene);    

    // Ajout du crosshair
    gce::GameObject& crosshair = gce::GameObject::Create(scene);
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

    GameObject& testObject2 = GameObject::Create(scene);
	testObject2.transform.SetWorldPosition({ .0f,-10.0f,0.0f });
	MeshRenderer* pMeshRenderer2 = testObject2.AddComponent<MeshRenderer>();
	pMeshRenderer2->SetGeometry(SHAPES.CUBE);
	pMeshRenderer2->SetAlbedoTexture(pNewTexture);		
    testObject2.AddComponent<BoxCollider>()->SetActive(true);
    tileMap.SetWalkableWithCollider(*testObject2.GetComponent<BoxCollider>(), false);

    

	GameObject& crossHair = GameObject::Create(scene);
    UiImage* crossHairImg = crossHair.AddComponent<UiImage>();
	crossHairImg->InitializeImage({ 0.0f, 0.0f }, { 1.f, 1.f }, .0f);
	crossHairImg->btmBrush = new BitMapBrush("res/Textures/crosshair.png");

    if (debugMode)
    {
        gce::Console::Log("Debug Mode Activated", gce::Console::LogType::WARNING);
        tileMap.DebugMode(scene);		
		ShowMouseCursor();

        // AJOUT : Visualisation des hitboxes invisibles (BoxCollider sans MeshRenderer actif)
        for (auto& [name, pObj] : importedScene)
        {
            if (pObj && pObj->HasComponent<gce::BoxCollider>() && pObj->HasComponent<gce::MeshRenderer>())
            {
                // Si le MeshRenderer est inactif (ce qui est le cas pour nos BoxColliders importés), on l'active
                gce::MeshRenderer* mr = pObj->GetComponent<gce::MeshRenderer>();
                if (!mr->IsActive())
                {
                    mr->SetActive(true);
                    // Optionnel : Assigner une texture de debug si nécessaire
                    // mr->SetAlbedoTexture(pNewTexture); 
                }
            }
        }
	}

    gce::GameManager::Run(params);
    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}