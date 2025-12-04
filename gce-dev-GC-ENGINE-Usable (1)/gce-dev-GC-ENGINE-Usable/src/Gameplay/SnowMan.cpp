#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
using namespace gce;

DECLARE_SCRIPT(AttackScript, ScriptFlag::Start | ScriptFlag::Update)
private:
    Geometry* bulletGeo = GeometryFactory::LoadGeometry("res/Exemple/SUZANNE.obj");
    Texture* bulletTex = new Texture("res/Exemple/TexturesTest.jpg");
    float ShootCooldown = 0.75f;

public:
    void Start()
    {
        ShootCooldown = 0.75f;
    }
    void Update()
    {
        if (ShootCooldown <= 0.0f)
        {
            GameObject* obj = m_pOwner;
            Scene* scene = const_cast<Scene*>(obj->GetScene());
            GameObject& BulletObject = GameObject::Create(*scene);
            BulletObject.transform.SetWorldPosition({ (obj->transform.GetWorldPosition().x * obj->transform.GetLocalRotation().GetX()) +1.f
                ,(obj->transform.GetWorldPosition().y + obj->transform.GetLocalRotation().GetY()) + 1.f
                ,(obj->transform.GetWorldPosition().z + obj->transform.GetLocalRotation().GetZ()) + 1.f
            });
            std::cout << obj->transform.GetLocalRotation().GetX() << " "<< obj->transform.GetLocalRotation().GetY() << " "<<obj->transform.GetLocalRotation().GetZ()<< std::endl;
            BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());
            MeshRenderer* pWeaponRenderer = BulletObject.AddComponent<MeshRenderer>();
            pWeaponRenderer->SetGeometry(bulletGeo);
            Texture* pWeaponTexture = bulletTex;
            pWeaponRenderer->SetAlbedoTexture(pWeaponTexture);
            BulletObject.transform.LocalScale({ 0.25,0.25,0.25 });
            BulletObject.AddComponent<BoxCollider>()->SetActive(true);
            BulletObject.AddComponent<PhysicComponent>();
            BulletObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
            Bullet* bullet = new Bullet(&BulletObject);
            bullet->AddShoot();
            bullet->SetOwner(obj);
            RessourcesManager::AddEntities(bullet);
			ShootCooldown = 1.0f;
		} 
        ShootCooldown -= GameManager::DeltaTime();
        
    }

    END_SCRIPT

SnowMan::SnowMan(GameObject* obj, float spd) : Ennemy(obj, spd)
{
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
    AddScript();
}

void SnowMan::Die()
{
    
}

void SnowMan::Attack()
{
   
}

    void SnowMan::AddScript()
{
    GameObject* obj = GetGameObject();
    obj->AddScript<AttackScript>();
}