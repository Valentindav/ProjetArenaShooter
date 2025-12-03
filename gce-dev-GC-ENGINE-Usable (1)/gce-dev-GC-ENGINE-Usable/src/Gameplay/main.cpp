#include <windows.h>
#include <Engine.h>
#include "Player.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {	
    gce::Console::Init();
    gce::GameManager::Create();    

    gce::Scene& scene = gce::Scene::Create();

	gce::GameObject& CameraObject = gce::GameObject::Create(scene);
    CameraObject.transform.LocalTranslate({ 0,0, -10 });
    gce::Camera* pCamera = CameraObject.AddComponent<gce::Camera>();
    pCamera->SetMainCamera();
    pCamera->SetType(gce::PERSPECTIVE);
    // Fov de base ici pi/4
    pCamera->perspective.fov = XM_PIDIV4;
    // point le plus proche que la camera verra
    pCamera->perspective.nearPlane = 0.001f;
    // point le plus eloigne que la camera verra
    pCamera->perspective.farPlane = 500.0f;
    // aspect ratio set en fonction de la largeur et hauteur de la fenetre
    pCamera->perspective.aspectRatio = 1000.0f / 800.0f;
    // ici on definis que l'axe Y va gerer la hauteur 
    pCamera->perspective.up = { 0.0f, 1.0f, 0.0f };
	Player* player = new Player(&CameraObject);

    GameObject& testObject = GameObject::Create(scene);

    MeshRenderer* pMeshRenderer = testObject.AddComponent<MeshRenderer>();
    // SHAPES contient plusieurs shapes de base (je vous laisse regarder) pour faire des formes de bases
    pMeshRenderer->SetGeometry(SHAPES.CUBE);

    Texture* pNewTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pMeshRenderer->SetAlbedoTexture(pNewTexture);

    testObject.transform.SetWorldPosition({ -2.0f,3.0f,0.0f });
	testObject.transform.SetWorldScale({ 5.0f,5.0f,2.0f });

    gce::WindowParam params;
    params.title = L"GCE Engine Window";
    params.width = 1280;
    params.height = 720;
    params.isFullScreen = false;
    params.isSplitScreen = false;
    params.screenDisposition = gce::SplitScreenDisposition::SQUARE_4_PLAYERS;
	player->AddMove();
    
    gce::GameManager::Run(params);	


    gce::GameManager::Destroy();
    gce::Console::UnInit();
    return 0;
}
