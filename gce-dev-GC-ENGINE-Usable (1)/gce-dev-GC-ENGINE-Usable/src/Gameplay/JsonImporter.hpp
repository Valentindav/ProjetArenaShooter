#pragma once
#include "GameManager.h"
#include "GameObject.h"
#include "Geometry.h"
#include "nlohmann.hpp"
#include <fstream>
#include <vector>
#include "Components/BoxCollider.h" 

using json = nlohmann::json;

// 1. On ajoute le champ 'root' dans la structure
struct ImportedLevelData {
    gce::GameObject* root;
    std::vector<gce::GameObject*> allObjects;
    std::vector<gce::BoxCollider*> allColliders;
    std::vector<gce::GameObject*> spawnZones;
};

struct MeshData {
    gce::Vector<float> vertices;
    gce::Vector<uint32> indices;
    gce::Vector<float> uvs;
};

struct SceneObject {
    String name;
    String parent;
    gce::Vector<float> position;
    gce::Vector<float> rotation;
    gce::Vector<float> scale;
    MeshData mesh;
    String material;
};

struct SceneRoot {
    gce::Vector<SceneObject> objects;
};

// ... (Les fonctions from_json pour MeshData et SceneObject restent identiques) ...
inline void from_json(const json& j, MeshData& m) {
    if (!j.is_object()) return;
    if (j.contains("vertices") && !j.at("vertices").is_null()) j.at("vertices").get_to(m.vertices);
    if (j.contains("indices") && !j.at("indices").is_null())  j.at("indices").get_to(m.indices);
    if (j.contains("uvs") && !j.at("uvs").is_null())      j.at("uvs").get_to(m.uvs);
}

inline void from_json(const json& _j, SceneObject& _o) {
    if (!_j.is_object()) return;
    if (_j.contains("name") && !_j.at("name").is_null())     _j.at("name").get_to(_o.name);
    if (_j.contains("parent") && !_j.at("parent").is_null())   _j.at("parent").get_to(_o.parent);
    if (_j.contains("position") && !_j.at("position").is_null()) _j.at("position").get_to(_o.position);
    if (_j.contains("rotation") && !_j.at("rotation").is_null()) _j.at("rotation").get_to(_o.rotation);
    if (_j.contains("scale") && !_j.at("scale").is_null())    _j.at("scale").get_to(_o.scale);
    if (_j.contains("mesh") && !_j.at("mesh").is_null())     from_json(_j.at("mesh"), _o.mesh);
    if (_j.contains("material") && !_j.at("material").is_null()) _j.at("material").get_to(_o.material);
}

inline void from_json(const json& _j, SceneRoot& _r) {
    if (!_j.is_object()) return;
    if (!_j.contains("objects") || _j.at("objects").is_null()) return;
    const json& arr = _j.at("objects");
    if (arr.is_array()) {
        _r.objects.Resize(arr.size());
        for (int i = 0; i < arr.size(); i++) from_json(arr[i], _r.objects[i]);
    }
    else if (arr.is_object()) {
        SceneObject single;
        from_json(arr, single);
        _r.objects.PushBack(single);
    }
}
// ... (Fin des fonctions from_json inchangées) ...


inline ImportedLevelData importSceneFromJsonText(const std::string& _jsonFileText)
{
    UnorderedMap<String, gce::GameObject*> tempMap;
    ImportedLevelData result;

    // 2. Création du ROOT global
    // On le crée dans la scène active
    gce::Scene& activeScene = *(gce::GameManager::GetScenes()[0]);
    result.root = &gce::GameObject::Create(activeScene);
    result.root->SetName("LevelRoot");

    std::ifstream file(String(RES_PATH) + _jsonFileText);
    json j;
    if (file.is_open()) j = json::parse(file);

    SceneRoot rootData;
    from_json(j, rootData);

    if (rootData.objects.Empty()) {
        std::cerr << "[ImportScene] No objects found or file empty\n";
        return result;
    }

    // --- Phase 1 : Création des objets ---
    for (const auto& obj : rootData.objects)
    {
        if (obj.name.empty()) continue;

        gce::GameObject* go = &gce::GameObject::Create(activeScene);
        go->SetName(obj.name.data());

        // Transform
        if (obj.position.Size() >= 3) go->transform.SetLocalPosition({ obj.position[0], obj.position[1], obj.position[2] });
        if (obj.rotation.Size() >= 4) go->transform.SetLocalRotation({ obj.rotation[0], obj.rotation[1], obj.rotation[2], obj.rotation[3] });
        if (obj.scale.Size() >= 3)    go->transform.SetLocalScale({ obj.scale[0], obj.scale[1], obj.scale[2] });

        // Mesh & Collider
        bool hasVerts = !obj.mesh.vertices.Empty();
        if (hasVerts) {
            // ... (Code de création du mesh identique à avant) ...
            gce::Vector<gce::Vector3f32> verts;
            gce::Vector<gce::Vector2f32> uvs;
            gce::Vector<uint32> indices;
            size_t vertCount = obj.mesh.vertices.Size() / 3;

            verts.Reserve(vertCount);
            for (size_t i = 0; i < vertCount; ++i)
                verts.PushBack({ obj.mesh.vertices[i * 3], obj.mesh.vertices[i * 3 + 1], obj.mesh.vertices[i * 3 + 2] });

            indices = obj.mesh.indices;

            if (obj.mesh.uvs.Size() == vertCount * 2) {
                uvs.Reserve(vertCount);
                for (size_t i = 0; i < vertCount; ++i)
                    uvs.PushBack({ obj.mesh.uvs[i * 2], obj.mesh.uvs[i * 2 + 1] });
            }
            else {
                // UVs par défaut si manquants
                for (size_t i = 0; i < vertCount; ++i) uvs.PushBack({ 0.f, 0.f });
            }

            gce::Vector<gce::Vertex> vertexs;
            for (int i = 0; i < vertCount; ++i)
                vertexs.PushBack(gce::Vertex(verts[i], { 0.f,0.f,0.f }, { 0.f , 0.f, 0.f }, uvs[i]));

            gce::MeshRenderer* mr = go->AddComponent<gce::MeshRenderer>();
            gce::Geometry* geo = new gce::Geometry(vertexs.Data(), vertexs.Size(), indices.Data(), indices.Size());
            mr->SetGeometry(geo);

            // Ajout du Collider
            gce::BoxCollider* col = go->AddComponent<gce::BoxCollider>();
            col->SetActive(true);
            result.allColliders.push_back(col);
        }

        if (obj.name.find("zone") == 0)
        {
            // On s'assure qu'il a un collider pour définir la zone
            if (go->GetComponent<gce::BoxCollider>())
            {
                // On rend la zone invisible (optionnel mais conseillé)
                if (auto* mr = go->GetComponent<gce::MeshRenderer>()) {
                    mr->SetActive(false);
                }

                // On l'ajoute à la liste des zones
                result.spawnZones.push_back(go);
            }
        }
        else
        {
            // Si ce n'est pas une zone, on peut considérer que c'est un obstacle
            // et l'ajouter aux colliders pour le Pathfinding/TileMap
            if (auto* col = go->GetComponent<gce::BoxCollider>()) {
                result.allColliders.push_back(col);
            }
        }

        tempMap.emplace(obj.name, go);
        result.allObjects.push_back(go);

        tempMap.emplace(obj.name, go);
        result.allObjects.push_back(go);
    }

    // --- Phase 2 : Hiérarchie (Parenting) ---
    for (const auto& obj : rootData.objects) {
        auto itChild = tempMap.find(obj.name);
        if (itChild == tempMap.end()) continue;

        if (!obj.parent.empty()) {
            // Cas 1 : L'objet a un parent défini dans le JSON
            auto itParent = tempMap.find(obj.parent);
            if (itParent != tempMap.end()) {
                itChild->second->SetParent(*itParent->second);
            }
            else {
                std::cerr << "[ImportScene] Parent '" << obj.parent << "' not found for '" << obj.name << "'\n";
                // En fallback, on l'attache au Root pour ne pas qu'il flotte
                itChild->second->SetParent(*result.root);
            }
        }
        else {
            // 3. Cas 2 : Pas de parent JSON -> On l'attache à notre LevelRoot
            itChild->second->SetParent(*result.root);
        }
    }

    std::cout << "[ImportScene] Scene imported under 'LevelRoot'.\n";
    return result;
}