#pragma once

#include "Node.h"
#include <Engine.h>

using namespace gce;
using namespace std;

class TileMap
{
private:
    int m_width;
    int m_length;
    float m_cellSize;
	Vector3f32 m_origin;
    std::vector<std::vector<Node<Tile>*>> m_nodeVector;

    bool FindPath(Node<Tile>* const& start, Node<Tile>* const& target);
    vector<Node<Tile>*> ReconstructPath(Node<Tile>* start, Node<Tile>* target);

public:

    TileMap(int width, int length, float cellSize, gce::Scene &scene, Vector3f32 const& origin = { 0.f,0.f,0.f });
    ~TileMap();

    std::vector<std::vector<Node<Tile>*>> GetNodeVector() const { return m_nodeVector; }	
	GameObject* DebugMode(gce::Scene& scene);
    
    vector<Node<Tile>*> GeneratePath(Node<Tile>* start, Node<Tile>* target);
    void ClearPathData();

    Vector3f32 GetOrigin() const { return m_origin; }
    float GetCellSize() const { return m_cellSize; }
    Node<Tile>* GetNodeFromWorldPosition(Vector3f32 const& worldPos);
};