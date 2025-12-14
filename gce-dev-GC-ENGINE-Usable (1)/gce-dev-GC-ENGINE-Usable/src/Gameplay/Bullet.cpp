#include "Bullet.h"
#include "Player.h"
#include "SnowMan.h"
#include "RessourcesManager.h"

using namespace gce;

DECLARE_SCRIPT(Shoot_Update, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)
private:
    float m_lifeTime = 0.0f;
    bool hasCollided = false;
    inline static Vector<GameObject*> s_pendingDestroy;

public:
    void Start()
    {
        m_lifeTime = 5.0f;
        hasCollided = false;
    }

    void Update()
    {
        if (!s_pendingDestroy.Empty())
        {
            for (GameObject* pObj : s_pendingDestroy)
            {
                if (pObj && pObj->IsActive())
                {
                    pObj->Destroy();
                }
            }
            s_pendingDestroy.Clear();
        }

        if (hasCollided || !m_pOwner || !m_pOwner->IsActive()) return;

        if (m_lifeTime >= 0.0f)
        {
            m_pOwner->transform.WorldTranslate(m_pOwner->transform.GetLocalForward() * 2 * GameManager::DeltaTime());
        }
        else
        {
            hasCollided = true;
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

        m_lifeTime -= GameManager::DeltaTime();
    }

    void CollisionEnter(GameObject* other)
    {
        if (hasCollided || !m_pOwner || !m_pOwner->IsActive() || !other || !other->IsActive()) return;

        gce::Vector<Entity*> entity = RessourcesManager::getEntities();
        Entity* ownerEntity = nullptr;

        for (Entity* p : entity)
        {
            if (p && m_pOwner == p->GetGameObject())
            {
                ownerEntity = p;
                break;
            }
        }

        if (!ownerEntity) return;

        Bullet* bulletEntity = dynamic_cast<Bullet*>(ownerEntity);
        if (!bulletEntity) return;

        GameObject* bulletOwner = bulletEntity->GetOwner();
        if (bulletOwner == other) return;

        hasCollided = true;
        m_pOwner->SetActive(false);

        bool alreadyBullet = false;
        for (GameObject* p : s_pendingDestroy)
        {
            if (p == m_pOwner)
            {
                alreadyBullet = true;
                break;
            }
        }
        if (!alreadyBullet)
        {
            s_pendingDestroy.PushBack(m_pOwner);
        }

        if (other->GetName() == "Player")
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

            if (!alreadyOther)
            {
                Player* m_player = RessourcesManager::GetPlayer();
                if (m_player && m_player->GetGameObject() == other)
                {
                    if (m_player->m_life <= 1)
                    {
                        m_player->m_life = 0;
                        other->SetActive(false);
                        s_pendingDestroy.PushBack(other);
                    }
                    else
                    {
                        m_player->m_life = m_player->m_life - 1;
                    }
                }
            }
        }

        if (other->GetName() == "SnowMan")
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

            if (!alreadyOther)
            {
                SnowMan* m_snowman = nullptr;
                for (Entity* p : entity)
                {
                    if (p && other == p->GetGameObject())
                    {
                        m_snowman = dynamic_cast<SnowMan*>(p);
                        break;
                    }
                }

                if (m_snowman)
                {
                    if (m_snowman->m_life <= 1)
                    {
                        m_snowman->m_life = 0;
                        other->SetActive(false);
                        s_pendingDestroy.PushBack(other);
                    }
                    else
                    {
                        m_snowman->m_life = m_snowman->m_life - 1;
                    }
                }
            }
        }
    }
    END_SCRIPT

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