#include "Bullet.h"
#include "Player.h"
#include "SnowMan.h"
#include "Robot.h"
#include "RessourcesManager.h"

using namespace gce;

DECLARE_SCRIPT(Shoot_Update, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)
private:
	inline static Vector<GameObject*> s_pendingDestroy;

public:
	void Start() // Start the lifetime of the bullet
	{
		Bullet* self = dynamic_cast<Bullet*>(RessourcesManager::GetEntityFromGameObject(m_pOwner));
		if (!self) return;
		self->SetLifeTime(3.0f);
	}

	void Update() // move the bullet forward and destroy it when her lifetime is 0
	{
		Entity* entity = nullptr;
		entity = RessourcesManager::GetEntityFromGameObject(m_pOwner);
		Bullet* bullet = dynamic_cast<Bullet*>(entity);
		if (!m_pOwner && !m_pOwner->IsActive() && !bullet->GetOwner() && !bullet->GetOwner()->IsActive())
		{
			s_pendingDestroy.PushBack(m_pOwner);
			return;
		}
		if (!s_pendingDestroy.Empty())
		{
			for (GameObject* pObj : s_pendingDestroy)
			{
				pObj->Destroy();
			}
			s_pendingDestroy.Clear();
		}
		if (bullet->GetLifeTime() >= 0.0f)
		{
			m_pOwner->transform.WorldTranslate(m_pOwner->transform.GetLocalForward() * entity->GetSpeed()/100 * GameManager::DeltaTime());
		}
		else
		{
			if (!m_pOwner->IsActive())
			{
				return;
			}

			m_pOwner->SetActive(false);

			bool already = false;
			for (GameObject* p : s_pendingDestroy)
			{
				if (p == m_pOwner)
				{
					already = true; break;
				}
			}
			if (!already) s_pendingDestroy.PushBack(m_pOwner);
		}
		bullet->SetLifeTime(bullet->GetLifeTime() - GameManager::DeltaTime());
	}

	void CollisionEnter(GameObject* other) //handle collision with entity
	{
		gce::Vector<Entity*> entity = RessourcesManager::getEntities();

		Entity* ownerEntity = nullptr;
		ownerEntity = RessourcesManager::GetEntityFromGameObject(m_pOwner);

		Entity* OtherEntity = nullptr;
		OtherEntity = RessourcesManager::GetEntityFromGameObject(other);

		if (m_pOwner->IsActive() && dynamic_cast<Bullet*>(ownerEntity)->GetOwner() != other)
		{
			m_pOwner->SetActive(false);
			bool already = false;
			for (GameObject* p : s_pendingDestroy)
			{
				if (p == m_pOwner)
				{
					already = true; break;
				}
			}
			if (!already)
			{
				s_pendingDestroy.PushBack(m_pOwner);
			}
		}
		if (other->IsActive() && dynamic_cast<Bullet*>(ownerEntity)->GetOwner() != other)
		{
			if (other->GetName() == "Player" || other->GetName() == "SnowMan" || other->GetName() == "robot" || other->GetName() == "Elf" || other->GetName() == "Deer" || other->GetName() == "Boss")
			{
				bool alreadyOther = false;
				for (GameObject* p : s_pendingDestroy)
				{
					if (p == other)
					{
						alreadyOther = true;
						break;
					}
				}

				Entity* Otherentity = nullptr;
				Otherentity = RessourcesManager::GetEntityFromGameObject(other);
				if (!alreadyOther)
				{
						Otherentity->TakeDamage(dynamic_cast<Bullet*>(ownerEntity)->GetDamage());
				}
			}
		}
	}

	END_SCRIPT

	Bullet::Bullet(GameObject* obj, float spd) : Entity(obj,spd)
	{
		obj->SetName("bullet");
		MeshRenderer* pWeaponRenderer = obj->AddComponent<MeshRenderer>();
		pWeaponRenderer->SetGeometry(SHAPES.CUBE);
		obj->transform.LocalScale({ 0.05,0.05,0.05 });
		obj->AddComponent<BoxCollider>()->SetActive(true);
		obj->AddComponent<PhysicComponent>();
		obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
		AddShoot();
		obj->GetComponent<PhysicComponent>()->SetIsTrigger(true);
	}

	
    void Bullet::AddShoot() // add shoot script
    {
        GameObject* obj = GetGameObject();
        if (obj)
        {
            obj->SetName("Bullet");
            obj->AddScript<Shoot_Update>();
        }
    }

    void Bullet::DeleteShoot() // delete shoot script
    {
        GameObject* obj = GetGameObject();
        if (obj)
        {
            obj->RemoveScript<Shoot_Update>();
        }
    }

	void Bullet::SetTexture(std::string_view path) // set bullet texture
	{
		MeshRenderer* pMeshRenderer = GetGameObject()->AddComponent<MeshRenderer>();
		Texture* pNewTexture = new Texture(path);
		pMeshRenderer->SetAlbedoTexture(pNewTexture);
	}

	void Bullet::SetShape(gce::Geometry* geo)// set bullet shape
	{
		MeshRenderer* pMeshRenderer = GetGameObject()->AddComponent<MeshRenderer>();
		pMeshRenderer->SetGeometry(geo);
	}

	void Bullet::SetDamage(int dmg)
	{
		if (this == nullptr) return;
		GameObject* go = GetGameObject();
		if (!go) return;
		if (!go->HasComponent<BoxCollider>() || !go->HasComponent<PhysicComponent>()) {
			m_damage = dmg;
			return;
		}
		m_damage = dmg;
		BoxCollider* bc = go->GetComponent<BoxCollider>();
		PhysicComponent* pc = go->GetComponent<PhysicComponent>();
		if (m_damage == 0) {
			if (bc) bc->SetActive(false);
			if (pc) pc->SetActive(false);
		}
		else {
			if (bc) bc->SetActive(true);
			if (pc) pc->SetActive(true);
		}
	}
