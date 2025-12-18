#include <Engine.h>
#include "Player.h"
#include "Bullet.h"
#include "Controller.h"
#include "MenuManager.h"
#include "RessourcesManager.h"

DECLARE_SCRIPT(Move, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionStay)
public:
    Bullet* lastBullet = nullptr;
    float m_shootTimer = SHOOT_TIMER_WAIT;
    bool onGround = false;
    float sensitivity = 0.0005f;
    float jumpForce = 50.0f;
    Controller* inputManager;

    inline static Controller s_inputManagerInstance;

    void Start()
    {
        inputManager = &s_inputManagerInstance;
    }

    void Update()
    {
        GameObject* obj = m_pOwner;
        Entity* entityPlayer = RessourcesManager::GetEntityFromGameObject(obj);
        Player* player = dynamic_cast<Player*>(entityPlayer);
        MenuManager* mm = MenuManager::GetInstance();
        if (mm && mm->GetGameState() != GameState::Playing)
            return;

        gce::LockMouseCursor();
        float32 gravity = obj->GetComponent<PhysicComponent>()->GetGravityScale();
        obj->GetComponent<PhysicComponent>()->SetBounciness(-1.0f);

        if (player)
        {
            if (player->GetCameraFeedback())
                player->GetCameraFeedback()->Update(GameManager::DeltaTime());
            player->UpdateWeaponAnimation(GameManager::DeltaTime());
        }

        if (inputManager) // garde de sécurité
            inputManager->HandleInput(obj);

        gce::WindowParam windowParam = GameManager::GetWindowParam();
        gce::Vector2i32 const center = { windowParam.width / 2, windowParam.height / 2 };
        gce::Vector2i32 const currentPos = GetMousePosition();
        gce::Vector2f32 const deltaPixels = currentPos - center;

        static float yaw = 0.0f;
        static float pitch = 0.0f;

        const float pitchMin = -1.4f;
        const float pitchMax = 1.4f;

        yaw += deltaPixels.x * sensitivity;
        pitch += deltaPixels.y * sensitivity;
        pitch = gce::Clamp(pitch, pitchMin, pitchMax);

        Quaternion quaternion = Quaternion::RotationEuler(pitch, yaw, 0.0f);
        obj->transform.SetLocalRotation(quaternion);

        SetMousePosition(center);
        m_shootTimer -= GameManager::DeltaTime();
    }

    void CollisionStay(GameObject* other) // stay on the ground
    {
        if (m_pOwner && m_pOwner->IsActive())
        {
            // On récupère le nom de l'objet touché
            if (!other->GetName()) return;
            String name = other->GetName();
            Entity* entityPlayer = RessourcesManager::GetEntityFromGameObject(m_pOwner);
            Player* player = dynamic_cast<Player*>(entityPlayer);

            // Exclure explicitement les entités ennemies / balles (ne doivent pas être considérées sol)
            bool isEnemyOrProjectile =
                (name == "SnowMan" || name == "Robot" || name == "Bullet" || name == "bullet" ||
                    name == "Elf" || name == "Boss" || name == "Deer");

            // Si ce n'est pas une entité ennemie/projetile, considérer les objets solides (BoxCollider non trigger) comme sol
            if (!isEnemyOrProjectile)
            {
                if (other->HasComponent<BoxCollider>())
                {
                    BoxCollider* bc = other->GetComponent<BoxCollider>();
                    if (bc && !bc->isTrigger)
                    {
                        onGround = true;
                        if (player) player->m_jumpCount = 0;
                        return;
                    }
                }
                // fallback : accepter aussi les objets nommés "Floor" (compatibilité)
                if (name == "Floor")
                {
                    onGround = true;
                    if (player) player->m_jumpCount = 0;
                    return;
                }
            }
        }
    }

    END_SCRIPT