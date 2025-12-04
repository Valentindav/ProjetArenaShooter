#include <windows.h>
#include <Engine.h>
#include "Player.h"
#include "SnowMan.h"
#include "RessourcesManager.h"


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {	
    //----------------------------------INIT WORLD----------------------------------
    gce::Console::Init();
    gce::GameManager::Create();    
    RessourcesManager::Create();

    gce::Scene& scene = gce::Scene::Create();

    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1280;
    params.height = 720;
    params.isFullScreen = false;
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
    Weapon.transform.SetWorldPosition({ 0.0f,0.0f,0.0f });
    MeshRenderer* pWeaponRenderer = Weapon.AddComponent<MeshRenderer>();
    pWeaponRenderer->SetGeometry(GeometryFactory::LoadGeometry("res/Exemple/SUZANNE.obj"));
    pWeaponRenderer->SetAlbedoTexture(pNewTexture);
    Weapon.transform.LocalScale({ 0.25,0.25,0.25 });

   
    //----------------------------------Run----------------------------------
    testObject.transform.SetWorldPosition({ -2.0f,3.0f,0.0f });
    PlayerObject.transform.SetWorldPosition({ 0.0f,0.0f,-10.0f });
    Weapon.transform.SetWorldPosition({ 1.0f,0.0f,-8.0f });

    SnowManObject.transform.SetWorldPosition({ 1.0f,5.0f,-8.0f });

    SnowManObject.transform.SetWorldRotation({ 0.0f,0.0f,0.0f });

	SnowMan* Snowman = new SnowMan(&SnowManObject);
	Player* player = new Player(&PlayerObject);
    player->GetGameObject()->AddChild(CameraObject);
    player->GetGameObject()->AddChild(Weapon);
	
    RessourcesManager::AddEntities(player);
    gce::GameManager::Run(params);	

    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}
