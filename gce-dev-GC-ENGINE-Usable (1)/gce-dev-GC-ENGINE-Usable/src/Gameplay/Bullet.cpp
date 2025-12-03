#include "Bullet.h"

using namespace gce;

DECLARE_SCRIPT(Shoot_Update, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)
private:
	float m_lifeTime = 0.0f;

public:
void Start() {
	 m_lifeTime = 5.0f;
	
}

void Update()
{
	if (m_lifeTime >= 0.0f) {
		m_pOwner->transform.WorldTranslate(m_pOwner->transform.GetLocalForward() * 2 * GameManager::DeltaTime());
	}
	else {
		m_pOwner->Destroy();
	}
	m_lifeTime -= GameManager::DeltaTime();
}

void CollisionEnter(GameObject* other) {
	m_pOwner->Destroy();
	other->Destroy();
}

END_SCRIPT

void Bullet::AddShoot()
{
	GameObject* obj = GetGameObject();
	obj->AddScript<Shoot_Update>();
}
