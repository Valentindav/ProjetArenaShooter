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

    // Méthodes privées pour l'algorithme A*
    bool FindPath(Node<Tile>* const& start, Node<Tile>* const& target);
    vector<Node<Tile>*> ReconstructPath(Node<Tile>* start, Node<Tile>* target);

public:

    TileMap(int width, int lenght, float cellSize, gce::Scene &scene, Vector3f32 const& origin = { 0,0,0 });
    ~TileMap();

    std::vector<std::vector<Node<Tile>*>> GetNodeVector() const { return m_nodeVector; }	
	GameObject* DebugMode(gce::Scene& scene);

    vector<Node<Tile>*> GeneratePath(Node<Tile>* start, Node<Tile>* target);
    void ClearPathData();
};