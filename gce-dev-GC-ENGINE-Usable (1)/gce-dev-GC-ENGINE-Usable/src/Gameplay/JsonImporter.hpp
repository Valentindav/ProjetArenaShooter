#pragma once
#include "GameManager.h"
#include "GameObject.h"
#include "Geometry.h"
#include "nlohmann.hpp"
#include <fstream>
#include <vector>
#include <string> // Nécessaire pour std::string

// Assurez-vous que le chemin est bon
#include "../Engine/Components/BoxCollider.h" 

using json = nlohmann::json;

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
    std::string name; // Utilisation de std::string pour faciliter les manipulations (.find, .c_str)
    std::string parent;
    gce::Vector<float> position;
    gce::Vector<float> rotation;
    gce::Vector<float> scale;
    MeshData mesh;
    std::string material;
};

struct SceneRoot {
    gce::Vector<SceneObject> objects;
};

// --- Fonctions de parsing JSON ---

inline void from_json(const json& j, MeshData& m) {
    if (!j.is_object()) return;
    if (j.contains("vertices") && !j.at("vertices").is_null()) j.at("vertices").get_to(m.vertices);
    if (j.contains("indices") && !j.at("indices").is_null())  j.at("indices").get_to(m.indices);
    if (j.contains("uvs") && !j.at("uvs").is_null())      j.at("uvs").get_to(m.uvs);
}

inline void from_json(const json& _j, SceneObject& _o) {
    if (!_j.is_object()) return;
    // nlohmann::json gère très bien la conversion vers std::string
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

// --- Fonction Principale d'Import ---

inline ImportedLevelData importSceneFromJsonText(const std::string& _jsonFileText)
{
    // Map temporaire : Clé std::string -> GameObject*
    std::unordered_map<std::string, gce::GameObject*> tempMap;
    ImportedLevelData result;

    // Création du ROOT global
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

        // Utilisation de .c_str() possible car obj.name est maintenant une std::string
        go->SetName(obj.name.c_str());

        // Transform
        if (obj.position.Size() >= 3) go->transform.SetLocalPosition({ obj.position[0], obj.position[1], obj.position[2] });
        if (obj.rotation.Size() >= 4) go->transform.SetLocalRotation({ obj.rotation[0], obj.rotation[1], obj.rotation[2], obj.rotation[3] });
        if (obj.scale.Size() >= 3)    go->transform.SetLocalScale({ obj.scale[0], obj.scale[1], obj.scale[2] });

        // Mesh & Collider
        bool hasVerts = !obj.mesh.vertices.Empty();
        if (hasVerts) {
            // Création de la géométrie (identique à avant)
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
                for (size_t i = 0; i < vertCount; ++i) uvs.PushBack({ 0.f, 0.f });
            }

            gce::Vector<gce::Vertex> vertexs;
            for (int i = 0; i < vertCount; ++i)
                vertexs.PushBack(gce::Vertex(verts[i], { 0.f,0.f,0.f }, { 0.f , 0.f, 0.f }, uvs[i]));

            gce::MeshRenderer* mr = go->AddComponent<gce::MeshRenderer>();
            gce::Geometry* geo = new gce::Geometry(vertexs.Data(), vertexs.Size(), indices.Data(), indices.Size());
            mr->SetGeometry(geo);

            // --- LOGIQUE DE TRI ---

            // Cas 1 : C'est une Zone de Spawn (Commence par "zone")
            if (obj.name.find("Zone") == 0)
            {
                // Ajout d'un collider trigger pour la zone
                gce::BoxCollider* col = go->AddComponent<gce::BoxCollider>();
                col->SetActive(true);
                // On cache le visuel
                mr->SetActive(false);

                result.spawnZones.push_back(go);
            }
            // Cas 2 : C'est un obstacle (Contient "BoxCollider")
            // "remet de quoi remplir le vecteur box collider quand le nom contient box collider"
            else if (obj.name.find("BoxCollider") != std::string::npos)
            {
                gce::BoxCollider* col = go->AddComponent<gce::BoxCollider>();
                col->SetActive(true);

                // On l'ajoute à la liste pour le baking de la TileMap
                result.allColliders.push_back(col);
            }
        }

        tempMap.emplace(obj.name, go);
        result.allObjects.push_back(go);
    }

    // --- Phase 2 : Hiérarchie ---
    for (const auto& obj : rootData.objects) {
        auto itChild = tempMap.find(obj.name);
        if (itChild == tempMap.end()) continue;

        if (!obj.parent.empty()) {
            auto itParent = tempMap.find(obj.parent);
            if (itParent != tempMap.end()) {
                itChild->second->SetParent(*itParent->second);
            }
            else {
                // Fallback au root si le parent n'est pas trouvé
                itChild->second->SetParent(*result.root);
            }
        }
        else {
            // Attachement par défaut au LevelRoot
            itChild->second->SetParent(*result.root);
        }
    }

    std::cout << "[ImportScene] Imported " << result.allObjects.size() << " objects ("
        << result.spawnZones.size() << " zones, "
        << result.allColliders.size() << " colliders).\n";

    return result;
}