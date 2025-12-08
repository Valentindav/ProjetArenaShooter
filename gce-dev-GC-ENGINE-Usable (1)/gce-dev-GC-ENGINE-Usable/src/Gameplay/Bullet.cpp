#include "Bullet.h"
#include "RessourcesManager.h"
using namespace gce;

DECLARE_SCRIPT(Shoot_Update, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)
private:
	float m_lifeTime = 0.0f;
	inline static Vector<GameObject*> s_pendingDestroy;

public:
void Start() 
{
	 m_lifeTime = 5.0f;
	
}

void Update()
{
	if (!s_pendingDestroy.Empty())
	{
		for (GameObject* pObj : s_pendingDestroy)
		{
			if (pObj) pObj->Destroy();
		}
		s_pendingDestroy.Clear();
	}

	if (m_lifeTime >= 0.0f) 
	{
		m_pOwner->transform.WorldTranslate(m_pOwner->transform.GetLocalForward() * 2 * GameManager::DeltaTime());
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
	m_lifeTime -= GameManager::DeltaTime();
}

void CollisionEnter(GameObject* other) 
{
	gce::Vector<Entity*> entity = RessourcesManager::getEntities();
	Entity* ownerEntity = nullptr;
	for (Entity* p : entity) 
	{
		if (m_pOwner == p->GetGameObject()) 
		{
			ownerEntity = p;
		}
	}
	if (m_pOwner && m_pOwner->IsActive() && m_pOwner && dynamic_cast<Bullet*>(ownerEntity)->GetOwner() != other)
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
	if (other && other->IsActive()&& dynamic_cast<Bullet*>(ownerEntity)->GetOwner() != other)
	{
		other->SetActive(false);
		bool alreadyOther = false;
		for (GameObject* p : s_pendingDestroy)
		{
			if (p == other)
			{ 
				alreadyOther = true; break; 
			} 
		}
		if (!alreadyOther) s_pendingDestroy.PushBack(other);
	}
}

END_SCRIPT	

void Bullet::AddShoot()
{
	GameObject* obj = GetGameObject();
	obj->SetName("Bullet"); 
	obj->AddScript<Shoot_Update>();
}

void Bullet::DeleteShoot()
{
	GameObject* obj = GetGameObject();
	obj->RemoveScript<Shoot_Update>();
}
