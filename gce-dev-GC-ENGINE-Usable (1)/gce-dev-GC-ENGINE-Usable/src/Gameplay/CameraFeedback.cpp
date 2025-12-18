#include "CameraFeedback.h"
#include <cmath>

CameraFeedback::CameraFeedback(GameObject* camera)
    : m_camera(camera)
{
    if (m_camera)
    {
        m_originalLocalPosition = m_camera->transform.GetLocalPosition();
        m_originalLocalRotation = m_camera->transform.GetLocalRotation();
    }
}

void CameraFeedback::TriggerShake(const ShakeParams& params)
{
    if (!m_shakeEnabled || !m_camera) return;

    m_isShaking = true;
    m_shakeTimer = 0.0f;
    m_shakeDuration = params.duration;
    m_shakeIntensity = params.intensity;
    m_shakeFrequency = params.frequency;
}

void CameraFeedback::TriggerDamageShake()
{
    ShakeParams params;
    params.intensity = 0.3f;
    params.duration = 0.4f;
    params.frequency = 25.0f;
    TriggerShake(params);
}

void CameraFeedback::TriggerShootRecoil()
{
    if (!m_recoilEnabled || !m_camera) return;

    m_isRecoiling = true;
    m_recoilTimer = 0.0f;

    float Angle = -5.0f;
    float Rad = Angle * (XM_PI / 180.0f);

    m_recoilOffset = Vector3f32(0.0f, 0.0f, -0.05f);

    m_recoilRotation = Quaternion::RotationEuler(Rad, 0.0f, 0.0f);
}

void CameraFeedback::TriggerJumpBounce()
{
    ShakeParams params;
    params.intensity = 0.08f;
    params.duration = 0.2f;
    params.frequency = 15.0f;
    TriggerShake(params);
}

void CameraFeedback::TriggerLandingImpact(float fallVelocity)
{
    float impact = gce::Clamp(abs(fallVelocity) / 20.0f, 0.0f, 1.0f);

    ShakeParams params;
    params.intensity = 0.15f * impact;
    params.duration = 0.25f;
    params.frequency = 20.0f;
    TriggerShake(params);
}

void CameraFeedback::Update(float deltaTime)
{
    if (!m_camera) return;

    UpdateShake(deltaTime);
    UpdateRecoil(deltaTime);
    UpdateWalk(deltaTime);
    ApplyTransform();
}

void CameraFeedback::UpdateShake(float deltaTime)
{
    if (!m_isShaking)
    {
        m_shakeOffset = Vector3f32(0.0f, 0.0f, 0.0f);
        return;
    }

    m_shakeTimer += deltaTime;

    if (m_shakeTimer >= m_shakeDuration)
    {
        m_isShaking = false;
        m_shakeOffset = Vector3f32(0.0f, 0.0f, 0.0f);
        return;
    }

    float progress = m_shakeTimer / m_shakeDuration;
    float falloff = 1.0f - progress;

    float x = sin(m_shakeTimer * m_shakeFrequency) * m_shakeIntensity * falloff;
    float y = sin(m_shakeTimer * m_shakeFrequency * 1.3f) * m_shakeIntensity * falloff;
    float z = sin(m_shakeTimer * m_shakeFrequency * 0.8f) * m_shakeIntensity * falloff * 0.5f;

    m_shakeOffset = Vector3f32(x, y, z);
}

void CameraFeedback::UpdateRecoil(float deltaTime)
{
    if (!m_isRecoiling)
    {
        m_recoilOffset = Vector3f32(0.0f, 0.0f, 0.0f);
        m_recoilRotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    m_recoilTimer += deltaTime;

    if (m_recoilTimer >= m_recoilDuration)
    {
        m_isRecoiling = false;
        m_recoilOffset = Vector3f32(0.0f, 0.0f, 0.0f);
        m_recoilRotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    float progress = m_recoilTimer / m_recoilDuration;
    float ease = 1.0f - pow(1.0f - progress, 3.0f);

    m_recoilOffset = m_recoilOffset * (1.0f - ease);

    Quaternion identity(0.0f, 0.0f, 0.0f, 1.0f);

    float dot = m_recoilRotation.GetX() * identity.GetX() +
        m_recoilRotation.GetY() * identity.GetY() +
        m_recoilRotation.GetZ() * identity.GetZ() +
        m_recoilRotation.GetW() * identity.GetW();

    if (dot < 0.0f)
    {
        dot = -dot;
        identity = Quaternion(-identity.GetX(), -identity.GetY(), -identity.GetZ(), -identity.GetW());
    }


    if (dot > 0.9995f)
    {
        float invT = 1.0f - ease;
        m_recoilRotation = Quaternion( m_recoilRotation.GetX() * invT + identity.GetX() * ease,m_recoilRotation.GetY() * invT + identity.GetY() * ease,m_recoilRotation.GetZ() * invT + identity.GetZ() * ease,
        m_recoilRotation.GetW() * invT + identity.GetW() * ease);

        float len = sqrt(m_recoilRotation.GetX() * m_recoilRotation.GetX() +m_recoilRotation.GetY() * m_recoilRotation.GetY() +m_recoilRotation.GetZ() * m_recoilRotation.GetZ() +
        m_recoilRotation.GetW() * m_recoilRotation.GetW());

        if (len > 0.0f)
        {
            m_recoilRotation = Quaternion(
                m_recoilRotation.GetX() / len,
                m_recoilRotation.GetY() / len,
                m_recoilRotation.GetZ() / len,
                m_recoilRotation.GetW() / len
            );
        }
    }
    else
    {
        float theta = acos(dot);
        float sinTheta = sin(theta);
        float wa = sin((1.0f - ease) * theta) / sinTheta;
        float wb = sin(ease * theta) / sinTheta;

        m_recoilRotation = Quaternion(m_recoilRotation.GetX() * wa + identity.GetX() * wb, m_recoilRotation.GetY() * wa + identity.GetY() * wb,m_recoilRotation.GetZ() * wa + identity.GetZ() * wb,
        m_recoilRotation.GetW() * wa + identity.GetW() * wb);
    }
}

void CameraFeedback::UpdateWalk(float deltaTime)
{
    if (!m_isMoving)
    {
        m_walkTimer = 0.0f;
        return;
    }

    m_walkTimer += deltaTime * m_walkSpeed;
}

void CameraFeedback::ApplyTransform()
{
    if (!m_camera) return;

    Vector3f32 finalPosition = m_originalLocalPosition;

    finalPosition = finalPosition + m_shakeOffset;

    finalPosition = finalPosition + m_recoilOffset;

    if (m_isMoving)
    {
        float bobY = sin(m_walkTimer) * m_walkAmount;
        float bobX = sin(m_walkTimer * 0.5f) * m_walkAmount * 0.5f;
        finalPosition = finalPosition + Vector3f32(bobX, bobY, 0.0f);
    }

    m_camera->transform.SetLocalPosition(finalPosition);

}

Quaternion CameraFeedback::MultiplyQuaternions(const Quaternion& q1, const Quaternion& q2)
{
    float x = q1.GetW() * q2.GetX() + q1.GetX() * q2.GetW() + q1.GetY() * q2.GetZ() - q1.GetZ() * q2.GetY();
    float y = q1.GetW() * q2.GetY() - q1.GetX() * q2.GetZ() + q1.GetY() * q2.GetW() + q1.GetZ() * q2.GetX();
    float z = q1.GetW() * q2.GetZ() + q1.GetX() * q2.GetY() - q1.GetY() * q2.GetX() + q1.GetZ() * q2.GetW();
    float w = q1.GetW() * q2.GetW() - q1.GetX() * q2.GetX() - q1.GetY() * q2.GetY() - q1.GetZ() * q2.GetZ();

    return Quaternion(x, y, z, w);
}

void CameraFeedback::Reset()
{
    m_isShaking = false;
    m_isRecoiling = false;
    m_shakeTimer = 0.0f;
    m_recoilTimer = 0.0f;
    m_shakeOffset = Vector3f32(0.0f, 0.0f, 0.0f);
    m_recoilOffset = Vector3f32(0.0f, 0.0f, 0.0f);
    m_recoilRotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    m_walkTimer = 0.0f;
    m_isMoving = false;

    if (m_camera)
    {
        m_camera->transform.SetLocalPosition(m_originalLocalPosition);
        m_camera->transform.SetLocalRotation(m_originalLocalRotation);
    }
}