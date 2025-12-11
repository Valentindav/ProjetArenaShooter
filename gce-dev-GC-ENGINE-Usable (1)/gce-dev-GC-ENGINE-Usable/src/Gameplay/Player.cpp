#include "Player.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Controller.h"
#include "MoveScript.h"

using namespace gce;

Player::Player(GameObject* obj, float spd) : Entity(obj, spd) 
{
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    obj->AddComponent<BoxCollider>()->SetActive(true);
	obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Player");
    AddMove();
}

void Player::AddMove() // add move script to player
{
    GameObject* obj = GetGameObject();
    obj->AddScript<Move>();
}