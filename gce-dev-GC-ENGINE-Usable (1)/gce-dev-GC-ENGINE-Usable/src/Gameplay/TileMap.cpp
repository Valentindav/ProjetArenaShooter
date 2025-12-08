#include "TileMap.h"
#include <algorithm>
#include <queue>
#include <cmath>

TileMap::TileMap(int width, int length, int height, float cellSize, gce::Scene &scene, Vector3f32 const& origin) : m_width(width), m_height(height), m_length(length), m_cellSize(cellSize)
{
	m_nodeVector = std::vector<std::vector<std::vector<Node<Tile>*>>>(height, std::vector<std::vector<Node<Tile>*>>(width, std::vector<Node<Tile>*>(length, nullptr)));
    int directions[26][3] =
    {
        // 6 directions cardinales (faces)
        {1, 0, 0},   // +X
        {-1, 0, 0},  // -X
        {0, 1, 0},   // +Y
        {0, -1, 0},  // -Y
        {0, 0, 1},   // +Z
        {0, 0, -1},  // -Z
        // 12 diagonales sur les arêtes
        {1, 1, 0},   // +X +Y
        {1, -1, 0},  // +X -Y
        {-1, 1, 0},  // -X +Y
        {-1, -1, 0}, // -X -Y
        {1, 0, 1},   // +X +Z
        {1, 0, -1},  // +X -Z
        {-1, 0, 1},  // -X +Z
        {-1, 0, -1}, // -X -Z
        {0, 1, 1},   // +Y +Z
        {0, 1, -1},  // +Y -Z
        {0, -1, 1},  // -Y +Z
        {0, -1, -1}, // -Y -Z
        // 8 diagonales dans les coins
        {1, 1, 1},   // +X +Y +Z
        {1, 1, -1},  // +X +Y -Z
        {1, -1, 1},  // +X -Y +Z
        {1, -1, -1}, // +X -Y -Z
        {-1, 1, 1},  // -X +Y +Z
        {-1, 1, -1}, // -X +Y -Z
        {-1, -1, 1}, // -X -Y +Z
        {-1, -1, -1} // -X -Y -Z
    };
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            for (int k = 0; k < length; ++k)
            {
                m_nodeVector[i][j][k] = new Node<Tile>();
                m_nodeVector[i][j][k]->Data = new Tile();
                m_nodeVector[i][j][k]->Data->gridX = i;
                m_nodeVector[i][j][k]->Data->gridY = j;
                m_nodeVector[i][j][k]->Data->gridZ = k;
                m_nodeVector[i][j][k]->Data->X = origin.x + (i * cellSize);
                m_nodeVector[i][j][k]->Data->Y = origin.y + (j * cellSize);
                m_nodeVector[i][j][k]->Data->Z = origin.z + (k * cellSize);
                m_nodeVector[i][j][k]->Visited = false;
                m_nodeVector[i][j][k]->CameFrom = nullptr;
                m_nodeVector[i][j][k]->Data->Walkable = false;
                m_nodeVector[i][j][k]->Data->Cost = 1.0f;
                m_nodeVector[i][j][k]->neighbors.clear();
                for (int l = 0; l < 26; l++)
                {
                    int Dx = i + directions[l][0];
                    int Dy = j + directions[l][1];
                    int Dz = k + directions[l][2];
                    if (Dx >= 0 && Dx < height && Dy >= 0 && Dy < width && Dz >= 0 && Dz < length)
                    {
                        m_nodeVector[i][j][k]->neighbors.push_back(m_nodeVector[Dx][Dy][Dz]);
                    }
                }
            }
        }
    }
}

void TileMap::SetInNodeVector(GameObject* obj)
{
    gce::Box objectBox = obj->GetComponent<BoxCollider>()->GetWorldBox();
    gce::Vector3f32 objectBoxMinimum = objectBox.min;
    gce::Vector3f32 objectBoxMaximum = objectBox.max;
    for (int i = objectBoxMinimum.x / m_cellSize; i < objectBoxMaximum.x/m_cellSize; i++)
    {      
        for (int j = objectBoxMinimum.y/m_cellSize ; j < objectBoxMaximum.y/m_cellSize; j++)
        {
            for (int k = objectBoxMinimum.z; k < objectBoxMaximum.z/m_cellSize; k++)
            {
                if (i >= 0 && i < m_width && j >= 0 && j < m_height && k >= 0 && k < m_length)
                {
                    m_nodeVector[i][j][k]->Data->Walkable = false;
                }
            }
		}
	}
}

void TileMap::LeaveInNodeVector(GameObject* obj)
{
    gce::Box objectBox = obj->GetComponent<BoxCollider>()->GetWorldBox();
    gce::Vector3f32 objectBoxMinimum = objectBox.min;
    gce::Vector3f32 objectBoxMaximum = objectBox.max;
    for (int i = objectBoxMinimum.x / m_cellSize; i < objectBoxMaximum.x / m_cellSize; i++)
    {
        for (int j = objectBoxMinimum.y / m_cellSize; j < objectBoxMaximum.y / m_cellSize; j++)
        {
            for (int k = objectBoxMinimum.z; k < objectBoxMaximum.z / m_cellSize; k++)
            {
                if (i >= 0 && i < m_width && j >= 0 && j < m_height && k >= 0 && k < m_length)
                {
                    m_nodeVector[i][j][k]->Data->Walkable = true;
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

std::vector<Node<Tile>*> TileMap::FindPath(Vector3f32 const& startPos, Vector3f32 const& targetPos)
{
    for (auto& modifiedNode : m_modifiedNodes)
    {
        modifiedNode->Visited = false;
        modifiedNode->CameFrom = nullptr;
        modifiedNode->Data->DistanceToStart = 0.f;
        modifiedNode->Data->DistanceToEnd = 0.f;
        modifiedNode->Data->totalCost = 0.f;
	}

    std::priority_queue<Node<Tile>*, std::vector<Node<Tile>*>, CompareTileAStar> priority;

    int dx;
    int dy;
    int dz;
	int startX = static_cast<int>(startPos.x / m_cellSize);
	int startY = static_cast<int>(startPos.y / m_cellSize);
	int startZ = static_cast<int>(startPos.z / m_cellSize);
	int targetX = static_cast<int>(targetPos.x / m_cellSize);
	int targetY = static_cast<int>(targetPos.y / m_cellSize);
    int targetZ = static_cast<int>(targetPos.z / m_cellSize);
	Node<Tile>* start = m_nodeVector[startX][startY][startZ];

    dx = startX - targetX;
    dy = startY - targetY;
    dz = startZ - targetZ;

	start->Data->DistanceToEnd = std::abs(dx) + std::abs(dy) + std::abs(dz);
    start->Data->totalCost = start->Data->DistanceToStart + start->Data->DistanceToEnd;
    start->Visited = false;
    start->CameFrom = nullptr;

    for (auto& neighbourNode : start->neighbors)
    {
        dx = neighbourNode->Data->gridX - targetX;
        dy = neighbourNode->Data->gridY - targetY;
        dz = neighbourNode->Data->gridZ - targetZ;
        neighbourNode->Data->DistanceToEnd = std::abs(dx) + std::abs(dy) + std::abs(dz);
        neighbourNode->Data->DistanceToStart = start->Data->DistanceToStart + neighbourNode->Data->Cost;
        neighbourNode->Data->totalCost = neighbourNode->Data->DistanceToStart + neighbourNode->Data->DistanceToEnd;
        neighbourNode->CameFrom = start;
        priority.push(neighbourNode);
        m_modifiedNodes.push_back(neighbourNode);

        Node<Tile>* neighbor = front->neighbors[i];

        if (neighbor == nullptr) continue;
        if (neighbor->Visited) continue;
        if (neighbor->Data->Player && neighbor != Cursor) {
            continue;
        }
        int dX = std::abs(front->Data->gridX - neighbor->Data->gridX);
        int dY = std::abs(front->Data->gridY - neighbor->Data->gridY);

        float dist_base_move;

        if (dX == 1 && dY == 1) {
            dist_base_move = std::sqrt(2.0f);
        }
        else {
            dist_base_move = 1.0f;
        }
        float movementCost = dist_base_move * (1.0f + neighbor->Data->Cost);

        float newDist = front->Data->DistanceToStart + movementCost;

        if (newDist < front->neighbors[i]->Data->DistanceToStart) {
            front->neighbors[i]->Data->DistanceToStart = newDist;
            front->neighbors[i]->Data->totalCost = newDist + front->neighbors[i]->Data->DistanceToEnd;
            front->neighbors[i]->CameFrom = front;
            priority.push(front->neighbors[i]);
        }
	}
        
	return {};
}