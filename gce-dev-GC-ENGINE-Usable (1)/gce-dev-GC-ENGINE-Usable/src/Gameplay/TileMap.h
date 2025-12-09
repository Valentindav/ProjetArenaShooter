#pragma once

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
	Vector3f32 m_origin;
    std::vector<std::vector<Node<Tile>*>> m_nodeVector;
public:

    TileMap(int width, int lenght, float cellSize, gce::Scene &scene, Vector3f32 const& origin = { 0,0,0 });
    ~TileMap() = default;

    std::vector<std::vector<Node<Tile>*>> GetNodeVector() const { return m_nodeVector; }	
	GameObject* DebugMode(gce::Scene& scene);
    bool FindPath(Node<Tile>* const& start, Node<Tile>* const& target);
    vector<Node<Tile>*> GetTilePath(Node<Tile>* player, Node<Tile>* target);
};