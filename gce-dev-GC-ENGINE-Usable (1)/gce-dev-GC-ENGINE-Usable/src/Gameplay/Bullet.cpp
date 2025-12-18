#include "Bullet.h"
#include "Player.h"
#include "SnowMan.h"
#include "Robot.h"
#include "Boss.h"
#include "RessourcesManager.h"
#include "MenuManager.h"
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
		MenuManager* mm = MenuManager::GetInstance();
		if (mm && mm->GetGameState() != GameState::Playing)
			return;

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
		if (bullet && bullet->GetLifeTime() >= 0.0f)
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
		if (bullet) bullet->SetLifeTime(bullet->GetLifeTime() - GameManager::DeltaTime());
	}

	void CollisionEnter(GameObject* other) //handle collision with entity
	{
		if (other->GetName() == "RayCast" || other->GetName() == "Floor" || other->GetName() == "Cookies") return;
		gce::Vector<Entity*> entity = RessourcesManager::GetEntities();
		std::cout << "Collision with " << other->GetName() << std::endl;
		Entity* ownerEntity = nullptr;
		ownerEntity = RessourcesManager::GetEntityFromGameObject(m_pOwner);

		Entity* OtherEntity = nullptr;
		OtherEntity = RessourcesManager::GetEntityFromGameObject(other);

		if (m_pOwner->IsActive() && dynamic_cast<Bullet*>(ownerEntity)->GetOwner() != other && !dynamic_cast<Bullet*>(ownerEntity)->GetNotCollide())
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
			if (other->GetName() == "Player" || other->GetName() == "SnowMan" || other->GetName() == "Robot" || other->GetName() == "Elf" || other->GetName() == "Deer" || other->GetName() == "Boss")
			{
				if (other->GetName() == "Boss" && dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(other))->m_isShielded) return;
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
					if (dynamic_cast<Bullet*>(ownerEntity)->GetOwner()->GetName() != "Player") {
						Otherentity->TakeDamage(dynamic_cast<Bullet*>(ownerEntity)->GetDamage());
					}
					else {
						Otherentity->TakeDamage(RessourcesManager::GetPlayer()->m_damage);
					}
				}
			}
		}
	}

	END_SCRIPT

		DECLARE_SCRIPT(FollowingBullet, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)
private:
	inline static Vector<GameObject*> s_pendingDestroy;
	float m_spawnTimer = 0.0f;
	bool m_isHoming = false;
	const float m_forwardDuration = 0.25f;

public:
	void Start() // Start the lifetime of the bullet
	{
		Bullet* self = dynamic_cast<Bullet*>(RessourcesManager::GetEntityFromGameObject(m_pOwner));
		if (!self) return;
		self->SetLifeTime(3.0f);
		m_spawnTimer = 0.0f;
		m_isHoming = false;
	}

	void Update() // move the bullet forward and destroy it when her lifetime is 0
	{
		if (!m_pOwner) return;
		Entity* entity = RessourcesManager::GetEntityFromGameObject(m_pOwner);
		if (!entity) return;
		Bullet* bullet = dynamic_cast<Bullet*>(entity);
		if (!bullet) return;

		if (!m_pOwner->IsActive() || !bullet->GetOwner() || !bullet->GetOwner()->IsActive())
		{
			s_pendingDestroy.PushBack(m_pOwner);
			return;
		}

		if (!s_pendingDestroy.Empty())
		{
			for (GameObject* pObj : s_pendingDestroy)
			{
				if (pObj) pObj->Destroy();
			}
			s_pendingDestroy.Clear();
		}

		if (!m_isHoming && m_spawnTimer < m_forwardDuration)
		{
			Vector3f32 forward = m_pOwner->transform.GetWorldForward();
			float speed = entity->GetSpeed();
			m_pOwner->transform.WorldTranslate(forward * (speed / 100.0f) * GameManager::DeltaTime());
		}
		else
		{
			if (!m_isHoming) m_isHoming = true;

			GameObject* targetGO = nullptr;
			if (RessourcesManager::GetChoosedEnemy()) targetGO = RessourcesManager::GetChoosedEnemy()->GetGameObject();

			if (targetGO)
			{
				Vector3f32 targetPos = targetGO->transform.GetWorldPosition();
				Vector3f32 currentPos = m_pOwner->transform.GetWorldPosition();
				Vector3f32 dir = targetPos - currentPos;
				float dist = dir.Norm();
				if (dist > 0.0001f)
				{
					dir = dir / dist;
					float speed = entity->GetSpeed();
					m_pOwner->transform.WorldTranslate(dir * (speed / 100.0f) * GameManager::DeltaTime());
				}
			}
			else
			{
				Vector3f32 forward = m_pOwner->transform.GetWorldForward();
				float speed = entity->GetSpeed();
				m_pOwner->transform.WorldTranslate(forward * (speed / 100.0f) * GameManager::DeltaTime());
			}
		}

		if (bullet->GetLifeTime() > 0.0f)
		{
			bullet->SetLifeTime(bullet->GetLifeTime() - GameManager::DeltaTime());
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
		m_spawnTimer += GameManager::DeltaTime();
	}

	void CollisionEnter(GameObject* other) //handle collision with entity
	{
		if (other->GetName() == "RayCast" || other->GetName() == "Floor" || other->GetName() == "Cookies" || other->GetName() == "Player") return;
		gce::Vector<Entity*> entity = RessourcesManager::GetEntities();
		std::cout << "Collision with " << other->GetName() << std::endl;
		Entity* ownerEntity = nullptr;
		ownerEntity = RessourcesManager::GetEntityFromGameObject(m_pOwner);

		Entity* OtherEntity = nullptr;
		OtherEntity = RessourcesManager::GetEntityFromGameObject(other);

		if (m_pOwner->IsActive() && dynamic_cast<Bullet*>(ownerEntity)->GetOwner() != other && !dynamic_cast<Bullet*>(ownerEntity)->GetNotCollide())
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
				if (other->GetName() == "Boss" && dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(other))->m_isShielded) return;
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
					if (dynamic_cast<Bullet*>(ownerEntity)->GetOwner()->GetName() != "Player") {
						Otherentity->TakeDamage(dynamic_cast<Bullet*>(ownerEntity)->GetDamage());
					}
					else {
						Otherentity->TakeDamage(RessourcesManager::GetPlayer()->m_damage);
					}
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
		m_notCollide = false;
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

	void Bullet::Addfollow()// add following shoot script
	{
		GameObject* obj = GetGameObject();
		if (obj)
		{
			obj->SetName("Bullet");
			if(obj->GetScript<Shoot_Update>()) obj->RemoveScript<Shoot_Update>();
			obj->AddScript<FollowingBullet>();
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
