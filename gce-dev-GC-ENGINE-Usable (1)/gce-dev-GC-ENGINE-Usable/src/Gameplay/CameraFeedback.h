#pragma once
#include <Engine.h>

using namespace gce;

class CameraFeedback
{
    GameObject* m_camera;

    //--------shake----------
    bool m_shakeEnabled = true;
    bool m_isShaking = false;
    float m_shakeTimer = 0.0f;
    float m_shakeDuration = 0.0f;
    float m_shakeIntensity = 0.0f;
    float m_shakeFrequency = 0.0f;
    Vector3f32 m_shakeOffset;

    //------------recoil----------
    bool m_recoilEnabled = true;
    bool m_isRecoiling = false;
    float m_recoilTimer = 0.0f;
    float m_recoilDuration = 0.15f;
    Vector3f32 m_recoilOffset;
    Quaternion m_recoilRotation;

    //-------------walk--------
    float m_walkTimer = 0.0f;
    float m_walkSpeed = 10.0f;
    float m_walkAmount = 0.05f;
    bool m_isMoving = false;

    Vector3f32 m_originalLocalPosition;
    Quaternion m_originalLocalRotation;

public:
    struct ShakeParams
    {
        float intensity = 0.1f;
        float duration = 0.3f;
        float frequency = 20.0f;
    };

    void UpdateShake(float deltaTime);
    void UpdateRecoil(float deltaTime);
    void UpdateWalk(float deltaTime);
    void ApplyTransform();

    CameraFeedback(GameObject* camera);
    ~CameraFeedback() = default;

    void TriggerShake(const ShakeParams& params);
    void TriggerDamageShake();
    void TriggerShootRecoil();
    void TriggerJumpBounce();
    void TriggerLandingImpact(float fallVelocity);
    Quaternion MultiplyQuaternions(const Quaternion& q1, const Quaternion& q2);

    void Update(float deltaTime);
    void Reset();

    void SetShakeEnabled(bool enabled) { m_shakeEnabled = enabled; }
    void SetRecoilEnabled(bool enabled) { m_recoilEnabled = enabled; }
};