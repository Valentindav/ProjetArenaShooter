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
    m_life = 500000000000000;
    obj->AddComponent<BoxCollider>()->SetActive(true);
	obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Player");
	m_weaponLevel = 1;
    m_baseSpeed = spd;
    AddMove();
}

void Player::UpdateWeapon() // change the weapno look according to the current state -> TODO
{
    GameObject* obj = nullptr;
    for (auto child : GetGameObject()->GetChildren()) {
        if (child->GetName() == "Weapon_1") {
			obj = child;
        }
	}
    if (obj == nullptr) return;
    MeshRenderer* pChildRenderer = obj->GetComponent<MeshRenderer>();
    switch (m_currentState) {
    case GIFT_WEAPON:
        pChildRenderer->SetGeometry(SHAPES.CUBE);
		break;
    case NERF_WEAPON:
        pChildRenderer->SetGeometry(SHAPES.CAPSULE);
        break;
	case THOMPSON_WEAPON:
        pChildRenderer->SetGeometry(SHAPES.CYLINDER);
        break;
    case CANDY_CANE:
        pChildRenderer->SetGeometry(SHAPES.DONUT);
        break;
    case BROKEN_CANDY_CANE:
        pChildRenderer->SetGeometry(SHAPES.HALF_SPHERE);
        break;
    case TESSON:
        pChildRenderer->SetGeometry(SHAPES.SPHERE);
        break;
    case BAZZOKA_WEAPON:
        pChildRenderer->SetGeometry(RessourcesManager::GetBottle());
    }
	std::cout << "Weapon updated to state: " << m_currentState << std::endl;
}

void Player::AddMove() // add move script to player
{
    GameObject* obj = GetGameObject();
    obj->AddScript<Move>();
}