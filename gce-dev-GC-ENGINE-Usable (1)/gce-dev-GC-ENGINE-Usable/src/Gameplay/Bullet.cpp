#include "Bullet.h"
#include "Player.h"
#include "SnowMan.h"
#include "Robot.h"
#include "RessourcesManager.h"
#include "MenuManager.h"

using namespace gce;

DECLARE_SCRIPT(Shoot_Update, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)
private:
	inline static Vector<GameObject*> s_pendingDestroy;

public:
	void Start()
	{
		Bullet* self = dynamic_cast<Bullet*>(RessourcesManager::GetEntityFromGameObject(m_pOwner));
		if (!self) return;
		self->SetLifeTime(3.0f);
	}

	void Update()
	{
		if (!m_pOwner || !m_pOwner->IsActive())
		{
			return;
		}

		Entity* entity = RessourcesManager::GetEntityFromGameObject(m_pOwner);
		if (!entity) return;

		Bullet* bullet = dynamic_cast<Bullet*>(entity);
		if (!bullet) return;

		GameObject* owner = bullet->GetOwner();
		if (!owner || !owner->IsActive())
		{
			m_pOwner->SetActive(false);
			bool already = false;
			for (GameObject* p : s_pendingDestroy)
			{
				if (p == m_pOwner)
				{
					already = true;
					break;
				}
			}
			if (!already) s_pendingDestroy.PushBack(m_pOwner);
			return;
		}

		if (!s_pendingDestroy.Empty())
		{
			for (GameObject* pObj : s_pendingDestroy)
			{
				if (pObj)
				{
					pObj->Destroy();
				}
			}
			s_pendingDestroy.Clear();
		}

		if (bullet->GetLifeTime() >= 0.0f)
		{
			m_pOwner->transform.WorldTranslate(m_pOwner->transform.GetLocalForward() * entity->GetSpeed() / 100 * GameManager::DeltaTime());
		}
		else
		{
			m_pOwner->SetActive(false);

			bool already = false;
			for (GameObject* p : s_pendingDestroy)
			{
				if (p == m_pOwner)
				{
					already = true;
					break;
				}
			}
			if (!already) s_pendingDestroy.PushBack(m_pOwner);
		}

		bullet->SetLifeTime(bullet->GetLifeTime() - GameManager::DeltaTime());
	}

	void CollisionEnter(GameObject* other)
	{
		if (!m_pOwner || !m_pOwner->IsActive() || !other || !other->IsActive()) return;

		Entity* ownerEntity = RessourcesManager::GetEntityFromGameObject(m_pOwner);
		if (!ownerEntity) return;

		Bullet* bulletEntity = dynamic_cast<Bullet*>(ownerEntity);
		if (!bulletEntity) return;

		GameObject* bulletOwner = bulletEntity->GetOwner();
		if (bulletOwner == other) return;

		if (m_pOwner->IsActive())
		{
			m_pOwner->SetActive(false);
			bool already = false;
			for (GameObject* p : s_pendingDestroy)
			{
				if (p == m_pOwner)
				{
					already = true;
					break;
				}
			}
			if (!already)
			{
				s_pendingDestroy.PushBack(m_pOwner);
			}
		}

		if (other->IsActive())
		{
			if (other->GetName() == "Player" || other->GetName() == "SnowMan" || other->GetName() == "robot" || other->GetName() == "Elf" || other->GetName() == "Deer" || other->GetName() == "Boss")
			{
				Entity* OtherEntity = RessourcesManager::GetEntityFromGameObject(other);

				if (OtherEntity)
				{
					OtherEntity->TakeDamage(bulletEntity->GetDamage());

					if (other->GetName() == "Player")
					{
						Player* player = dynamic_cast<Player*>(OtherEntity);
						if (player)
						{
							MenuManager* menuManager = MenuManager::GetInstance();
							if (menuManager)
							{
								menuManager->UpdateHealthUI(player->m_life, 13.0f);
							}
						}
					}
				}
			}
		}
	}
	END_SCRIPT

		Bullet::Bullet(GameObject* obj, float spd) : Entity(obj, spd), m_owner(nullptr), m_damage(1), m_lifeTime(0.0f)
	{
		obj->SetName("bullet");
		MeshRenderer* pWeaponRenderer = obj->AddComponent<MeshRenderer>();
		pWeaponRenderer->SetGeometry(SHAPES.CUBE);
		obj->transform.LocalScale({ 0.05, 0.05, 0.05 });
		obj->AddComponent<BoxCollider>()->SetActive(true);
		obj->AddComponent<PhysicComponent>();
		obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
		obj->GetComponent<PhysicComponent>()->SetIsTrigger(true);
		AddShoot();
	}

	void Bullet::AddShoot()
	{
		GameObject* obj = GetGameObject();
		if (obj)
		{
			obj->SetName("Bullet");
			obj->AddScript<Shoot_Update>();
		}
	}

	void Bullet::DeleteShoot()
	{
		GameObject* obj = GetGameObject();
		if (obj)
		{
			obj->RemoveScript<Shoot_Update>();
		}
	}

	void Bullet::SetTexture(std::string_view path)
	{
		GameObject* obj = GetGameObject();
		if (!obj) return;

		MeshRenderer* pMeshRenderer = obj->GetComponent<MeshRenderer>();
		if (!pMeshRenderer) return;

		Texture* pNewTexture = new Texture(path);
		pMeshRenderer->SetAlbedoTexture(pNewTexture);
	}

	void Bullet::SetShape(gce::Geometry* geo)
	{
		GameObject* obj = GetGameObject();
		if (!obj) return;

		MeshRenderer* pMeshRenderer = obj->GetComponent<MeshRenderer>();
		if (!pMeshRenderer) return;

		pMeshRenderer->SetGeometry(geo);
	}

	void Bullet::SetDamage(int dmg)
	{
		if (this == nullptr) return;
		GameObject* go = GetGameObject();
		if (!go) return;

		m_damage = dmg;

		if (!go->HasComponent<BoxCollider>() || !go->HasComponent<PhysicComponent>())
		{
			return;
		}

		BoxCollider* bc = go->GetComponent<BoxCollider>();
		PhysicComponent* pc = go->GetComponent<PhysicComponent>();

		if (m_damage == 0)
		{
			if (bc) bc->SetActive(false);
			if (pc) pc->SetActive(false);
		}
		else
		{
			if (bc) bc->SetActive(true);
			if (pc) pc->SetActive(true);
		}
	}