#include "Heal.h"
#include "RessourcesManager.h"
#include "Player.h"
#include "MenuManager.h"

using namespace gce;

DECLARE_SCRIPT(HealScript, ScriptFlag::Update | ScriptFlag::CollisionEnter)
private:

public:
	void Update() // turn the cookies around
	{
        MenuManager* mm = MenuManager::GetInstance();
        if (mm && mm->GetGameState() != GameState::Playing)
            return;
		m_pOwner->transform.WorldRotate({ 0.0f, 5.0f * GameManager::DeltaTime(), 0.0f });
	}

	void CollisionEnter(GameObject* other) //handle collision with player to heal
    {
        if (other->GetName() == "Player")
        {
            Player* player = RessourcesManager::GetPlayer();
            if (player)
            {
				player->Heal(2.0f);
                m_pOwner->SetActive(false);
            }
        }
	}

END_SCRIPT

Heal::Heal(GameObject* obj,float spd) : Entity(obj,spd) {
    GameObject* boxColliderObj = &GameObject::Create(*const_cast<Scene*>(GetGameObject()->GetScene()));
    MeshRenderer* pBoxColliderRenderer = boxColliderObj->AddComponent<MeshRenderer>();
    pBoxColliderRenderer->SetGeometry(RessourcesManager::GetCookiesObj());
    
    boxColliderObj->transform.SetWorldPosition({ obj->transform.GetWorldPosition().x,obj->transform.GetWorldPosition().y - 0.5f,obj->transform.GetWorldPosition().z });

    obj->transform.SetLocalScale({ 1.f, 2.f, 1.f });
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->SetName("Cookies");
    obj->AddChild(*boxColliderObj);
    obj->transform.SetLocalScale({ 0.9f, 1.4f, 0.9f });
    AddScript();
    m_life = 15.f;

	RessourcesManager::AddCookie(this);
}

void Heal::AddScript() {
    GameObject* obj = GetGameObject();
    if (obj)
    {
        obj->SetName("Cookies");
        obj->AddScript<HealScript>();
    }
}

void Heal::Die() {

}

