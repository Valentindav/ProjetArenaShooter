#pragma once

#include <Containers/Vector.hpp>
#include "Maths/Vector3.h"
#include "Maths/Vector2.hpp"
#include "Node.h"
#include <Engine.h>

using namespace gce;
using namespace std;

class TileMap
{
private:
    int m_width;
    int m_height;
    int m_length;
    float m_cellSize;
    std::vector<std::vector<std::vector<Node<Tile>*>>> m_nodeVector;
	std::vector<Node<Tile>*> m_modifiedNodes;
public:

    TileMap(int width, int lenght, int height, float cellSize, gce::Scene &scene, Vector3f32 const& origin = { 0,0,0 });
    ~TileMap() = default;

    std::vector<std::vector<std::vector<Node<Tile>*>>> GetNodeVector() const { return m_nodeVector; }	
	void SetInNodeVector(GameObject * obj);
	void LeaveInNodeVector(GameObject* obj);
	GameObject* DebugMode(gce::Scene& scene);
    std::vector<Node<Tile>*> FindPath(Vector3f32 const& startPos, Vector3f32 const& targetPos);
};