#include "TileMap.h"
#include <queue>
#include <cmath>

TileMap::TileMap(int width, int length, float cellSize, gce::Scene& scene, Vector3f32 const& origin) : m_width(width), m_length(length), m_cellSize(cellSize), m_origin(origin)
{
	m_nodeVector = std::vector<std::vector<Node<Tile>*>>(width, std::vector<Node<Tile>*>(length, nullptr));
    int directions[8][2] =
    {
        // 4 directions cardinales
        {1, 0},   // +X
        {-1, 0},  // -X
        {0, 1},   // +Y
        {0, -1},  // -Y
        // 4 diagonales
        {1, 1},   // +X +Y
        {1, -1},  // +X -Y
        {-1, 1},  // -X +Y
        {-1, -1}  // -X -Y
    };
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < length; ++j)
        {
            m_nodeVector[i][j] = new Node<Tile>();
            m_nodeVector[i][j]->data = new Tile();
            m_nodeVector[i][j]->data->gridX = i;
            m_nodeVector[i][j]->data->gridY = j;
            m_nodeVector[i][j]->data->worldPosition.x = origin.x + (i * cellSize);
            m_nodeVector[i][j]->data->worldPosition.y = origin.y + (j * cellSize);
            m_nodeVector[i][j]->data->way = false;
            m_nodeVector[i][j]->cameFrom = nullptr;
			m_nodeVector[i][j]->data->walkable = true;
            m_nodeVector[i][j]->data->cost = 1.0f;
            m_nodeVector[i][j]->neighbors.clear();
            for (int l = 0; l < 8; l++)
            {
                int dx = i + directions[l][0];
                int dy = j + directions[l][1];
                if (dx >= 0 && dx < width && dy >= 0 && dy < length)
                {
                    m_nodeVector[i][j]->neighbors.push_back(m_nodeVector[dx][dy]);
                }
            }
        }
    }
}

GameObject* TileMap::DebugMode(gce::Scene& scene)
{
    GameObject& zone = GameObject::Create(scene);
    MeshRenderer* pZoneRenderer = zone.AddComponent<MeshRenderer>();
    pZoneRenderer->SetGeometry(SHAPES.CUBE);
    zone.transform.LocalScale({ m_length * m_cellSize, m_height * m_cellSize, m_width * m_cellSize });
    zone.SetName("Floor");
	return &zone;
}

std::vector<Node<Tile>*> TileMap::GetTilePath(Node<Tile>* player, Node<Tile>* target) {
    std::vector<Node<Tile>*> path;
    Node<Tile>* current = target;

    // Réinitialiser les flags 'way' en 2D
    for (int i = 0; i < m_nodeVector.size(); i++) {
        for (int j = 0; j < m_nodeVector[i].size(); j++) {
            if (m_nodeVector[i][j]->data->way) {
                m_nodeVector[i][j]->data->way = false;
            }
        }
    }

    // Reconstruire le chemin de target vers player
    while (current != nullptr && current != player) {
        path.push_back(current);
        current->data->way = true;
        current = current->cameFrom;
    }

    return path;
}

bool TileMap::FindPath(Node<Tile>* const& start, Node<Tile>* const& target)
{
    std::priority_queue<Node<Tile>*, std::vector<Node<Tile>*>, CompareTileAStar> priority;

    int dx;
    int dy;
    const float INF = std::numeric_limits<float>::infinity();
    const float D = 1.0f;
    const float D2 = std::sqrt(2.0f);
    float movementCost;
    float newDist;

    for (int i = 0; i < m_width; ++i) {
        for (int j = 0; j < m_length; ++j) {
            m_nodeVector[i][j]->visited = false;
            m_nodeVector[i][j]->cameFrom = nullptr;
            dx = std::abs(m_nodeVector[i][j]->data->gridX - target->data->gridX);
            dy = std::abs(m_nodeVector[i][j]->data->gridY - target->data->gridY);
            m_nodeVector[i][j]->data->distanceToEnd = D * (dx + dy) + (D2 - 2 * D) * min(dx, dy); // Heuristique de distance diagonale
            m_nodeVector[i][j]->data->distanceToStart = INF;
            m_nodeVector[i][j]->data->totalCost = INF;
        }
    }

    start->data->distanceToStart = 0.0f;
    dx = std::abs(start->data->gridX - target->data->gridX);
    dy = std::abs(start->data->gridY - target->data->gridY);
    start->data->distanceToEnd = D * (dx + dy) + (D2 - 2 * D) * min(dx, dy);
    start->data->totalCost = start->data->distanceToStart + start->data->distanceToEnd;
    start->visited = false;
    start->cameFrom = nullptr;
        
    priority.push(start);

    while (!priority.empty()) {
        Node<Tile>* front = priority.top();
        priority.pop();

        if (front == target) {
            return true;
        }

        if (front->visited || !front->data->walkable) continue;
        front->visited = true;

        for (size_t i = 0; i < front->neighbors.size(); i++) {
            Node<Tile>* neighbor = front->neighbors[i];
            if (neighbor == nullptr || neighbor->visited || !neighbor->data->walkable) continue;

            dx = std::abs(front->data->gridX - neighbor->data->gridX);
            dy = std::abs(front->data->gridY - neighbor->data->gridY);

            movementCost = (dx == 1 && dy == 1) ? D2 : D;
            movementCost *= neighbor->data->cost;

            newDist = front->data->distanceToStart + movementCost;

            if (newDist < neighbor->data->distanceToStart) {
                neighbor->data->distanceToStart = newDist;
                dx = std::abs(neighbor->data->gridX - target->data->gridX);
                dy = std::abs(neighbor->data->gridY - target->data->gridY);
                neighbor->data->distanceToEnd = D * (dx + dy) + (D2 - 2 * D) * min(dx, dy);
                neighbor->data->totalCost = newDist + neighbor->data->distanceToEnd;
                neighbor->cameFrom = front;
                priority.push(neighbor);
            }
        }
    }

	return false;
}