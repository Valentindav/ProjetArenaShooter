#pragma once
#include "GameManager.h"
#include "GameObject.h"
#include "Geometry.h"
#include "nlohmann.hpp"
#include "Maths/Quaternion.h"
#include <fstream>

using json = nlohmann::json;

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

inline void from_json(const json& j, MeshData& m)
{
    if (!j.is_object()) return;
    if (j.contains("vertices") && !j.at("vertices").is_null()) j.at("vertices").get_to(m.vertices);
    if (j.contains("indices")  && !j.at("indices").is_null())  j.at("indices").get_to(m.indices);
    if (j.contains("uvs")      && !j.at("uvs").is_null())      j.at("uvs").get_to(m.uvs);
}

inline void from_json(const json& _j, SceneObject& _o)
{
    if (!_j.is_object()) return;
    if (_j.contains("name")     && !_j.at("name").is_null())     _j.at("name").get_to(_o.name);
    if (_j.contains("parent")   && !_j.at("parent").is_null())   _j.at("parent").get_to(_o.parent);

    if (_j.contains("position") && !_j.at("position").is_null()) _j.at("position").get_to(_o.position);
    if (_j.contains("rotation") && !_j.at("rotation").is_null()) _j.at("rotation").get_to(_o.rotation);
    if (_j.contains("scale")    && !_j.at("scale").is_null())    _j.at("scale").get_to(_o.scale);

    if (_j.contains("mesh")     && !_j.at("mesh").is_null())     from_json(_j.at("mesh"),_o.mesh);
    if (_j.contains("material") && !_j.at("material").is_null()) _j.at("material").get_to(_o.material);
}

inline void from_json(const json& _j, SceneRoot& _r)
{
    if (!_j.is_object()) return;
    if (!_j.contains("objects") || _j.at("objects").is_null()) return;
    
    const json& arr = _j.at("objects");
    if (arr.is_array())
    {
        _r.objects.Resize(arr.size());
        for (int i = 0; i < arr.size(); i++)
            from_json(arr[i], _r.objects[i]);
    }
    
    else if (arr.is_object())
    {
        SceneObject single;
        from_json(arr, single);
        _r.objects.PushBack(single);
    }
    else
        throw std::runtime_error("\"objects\" must be an array or object");
}

inline UnorderedMap<String, gce::GameObject*> importSceneFromJsonText(const std::string& _jsonFileText)
{
    UnorderedMap<String, gce::GameObject*> created;

    std::ifstream file(String(RES_PATH) + _jsonFileText);
    
    json j;
    if (file.is_open())
    {
        j = json::parse(file);
    }
    
    SceneRoot root;
    from_json(j, root);
    
    if (root.objects.Empty()) 
    {
        std::cerr << "[ImportScene] No objects found in scene\n";
        return created;
    }
    
    for (const auto& obj : root.objects) 
{
        if (obj.name.empty())
        {
            std::cerr << "[ImportScene] Skipping object with empty name\n";
            continue;
        }

        gce::GameObject* go = &gce::GameObject::Create(*(gce::GameManager::GetScenes()[0]));

        // Transform
        if (obj.position.Size() >= 3)
        {
            go->transform.SetLocalPosition(gce::Vector3f32(obj.position[0], obj.position[1], obj.position[2]));
        }
        if (obj.rotation.Size() >= 4)
        {
            go->transform.SetLocalRotation(gce::Quaternion(obj.rotation[0], obj.rotation[1], obj.rotation[2], obj.rotation[3]));
        }
        if (obj.scale.Size() >= 3)
        {
            go->transform.SetLocalScale(gce::Vector3f32(obj.scale[0], obj.scale[1], obj.scale[2]));
        }

        // Mesh
        bool hasVerts = !obj.mesh.vertices.Empty();
        bool hasTris  = !obj.mesh.indices.Empty();
        if (hasVerts && hasTris) {
            gce::Vector<gce::Vector3f32> verts;
            gce::Vector<gce::Vector2f32> uvs;
            gce::Vector<uint32> indices;
            
            const size_t vertCount = obj.mesh.vertices.Size() / 3;
            if (vertCount == 0)
            {
                std::cerr << "[ImportScene] Object '" << obj.name << "' has vertices size not multiple of 3\n";
            }
            else
            {
                verts.Reserve(vertCount);
                for (size_t i = 0; i < vertCount; ++i)
                {
                    float x = obj.mesh.vertices[i*3 + 0];
                    float y = obj.mesh.vertices[i*3 + 1];
                    float z = obj.mesh.vertices[i*3 + 2];
                    verts.PushBack({x,y,z});
                }
            }
            
           indices = obj.mesh.indices;
            
            if (!obj.mesh.uvs.Empty()) {
                if (obj.mesh.uvs.Size() == vertCount * 2)
                {
                    uvs.Reserve(vertCount);
                    for (size_t i = 0; i < vertCount; ++i)
                    {
                        float u = obj.mesh.uvs[i*2 + 0];
                        float v = obj.mesh.uvs[i*2 + 1];
                        uvs.PushBack({u,v});
                    }
                }
                else
                    std::cerr << "[ImportScene] UV array size mismatch on object '" << obj.name << "'\n";
            }

            gce::Vector<gce::Vertex> vertexs;
            for (int i = 0; i < vertCount; ++i)
                vertexs.PushBack(gce::Vertex(verts[i], { 0.f,0.f,0.f }, { 0.f , 0.f, 0.f }, uvs[i]));
            
            gce::MeshRenderer* mr = go->AddComponent<gce::MeshRenderer>();
            // Changer cette ligne car la géométrie se retrouve un peu perdu sinon
            gce::Geometry* geo = new gce::Geometry(vertexs.Data(), vertexs.Size(), indices.Data(), indices.Size());
            mr->SetGeometry(geo);
        }
        created.emplace(obj.name, go);
    }
    
    for (const auto& obj : root.objects) {
        if (!obj.parent.empty()) {
            auto itChild = created.find(obj.name);
            auto itParent = created.find(obj.parent);
            if (itChild != created.end() && itParent != created.end())
            {
                itChild->second->SetParent(*itParent->second);
            }
            else
            {
                std::cerr << "[ImportScene] Parent relationship not satisfied for '" << obj.name << "' -> '" << obj.parent << "'\n";
            }
        }
    }

    std::cout << "[ImportScene] Scene imported (" << created.size() << " objects).\n";
    return created;
}