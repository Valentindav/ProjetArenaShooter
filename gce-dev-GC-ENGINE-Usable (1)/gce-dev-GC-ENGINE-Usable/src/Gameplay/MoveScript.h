#include <Engine.h>
#include "Player.h"
#include "Bullet.h"
#include "Controller.h"
#include "MenuManager.h"

DECLARE_SCRIPT(Move, ScriptFlag::Update | ScriptFlag::CollisionStay)
public:
    Bullet* lastBullet = nullptr;
    float m_shootTimer = SHOOT_TIMER_WAIT;
    bool onGround = false;
    float sensitivity = 0.0005f;
    float jumpForce = 50.0f;
    Controller* inputManager;
	bool b_menu = true;

    void Update() // allow player to move
    {
        // Si un MenuManager existe et qu'on n'est pas en état Playing, ne pas exécuter Update
        MenuManager* mm = MenuManager::GetInstance();
        if (mm && mm->GetGameState() != GameState::Playing)
            return;

        gce::LockMouseCursor();
        GameObject* obj = m_pOwner;
        float32 gravity = obj->GetComponent<PhysicComponent>()->GetGravityScale();
        obj->GetComponent<PhysicComponent>()->SetBounciness(-1.0f);

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
            if (other->GetName() == "Floor")
            {
                onGround = true;
            }
        }
    }

    END_SCRIPT