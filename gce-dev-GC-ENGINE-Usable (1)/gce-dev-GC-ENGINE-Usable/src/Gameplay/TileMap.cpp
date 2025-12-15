#include "TileMap.h"
#include <queue>
#include <cmath>
#include <algorithm>

TileMap::TileMap(int width, int length, float cellSize, gce::Scene& scene, Vector3f32 const& origin) 
    : m_width(width), m_length(length), m_cellSize(cellSize), m_origin(origin)
{
	m_nodeVector = std::vector<std::vector<Node<Tile>*>>(width, std::vector<Node<Tile>*>(length, nullptr));

    // Calcul du coin supérieur gauche (Start) pour que 'origin' soit le CENTRE de la grille
    float halfWidth = (width * cellSize) / 2.0f;
    float halfLength = (length * cellSize) / 2.0f;
    
    // On assume que la grille est sur le plan XZ (Y est la hauteur)
    float startX = origin.x - halfWidth;
    float startZ = origin.z - halfLength;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < length; ++j)
        {
            m_nodeVector[i][j] = new Node<Tile>();
            m_nodeVector[i][j]->data = new Tile();
            m_nodeVector[i][j]->data->gridX = i;
            m_nodeVector[i][j]->data->gridY = j;
            
            // Position du CENTRE de la cellule
            m_nodeVector[i][j]->data->worldPosition.x = startX + (i * cellSize) + (cellSize * 0.5f);
            m_nodeVector[i][j]->data->worldPosition.y = startZ + (j * cellSize) + (cellSize * 0.5f); // .y stocke le Z monde
        }
    }

    int directions[8][2] =
    {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < length; ++j)
        {
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

TileMap::~TileMap()
{
    for (auto& row : m_nodeVector)
    {
        for (auto& node : row)
        {
            delete node->data;
            delete node;
        }
    }
}

GameObject* TileMap::DebugMode(gce::Scene& scene)
{
    GameObject& zone = GameObject::Create(scene);
    MeshRenderer* pZoneRenderer = zone.AddComponent<MeshRenderer>();
    pZoneRenderer->SetGeometry(SHAPES.CUBE);

    float widthReal = m_width * m_cellSize;
    float lengthReal = m_length * m_cellSize;

    zone.transform.LocalScale({ widthReal, m_cellSize, lengthReal });

    zone.transform.SetWorldPosition({
        m_origin.x,
        m_origin.y - (m_cellSize * 0.5f) + 30, 
        m_origin.z
    });

    zone.SetName("Floor");
	return &zone;
}

Node<Tile>* TileMap::GetNodeFromWorldPosition(Vector3f32 const& worldPos)
{
    float halfWidth = (m_width * m_cellSize) / 2.0f;
    float halfLength = (m_length * m_cellSize) / 2.0f;
    float startX = m_origin.x - halfWidth;
    float startZ = m_origin.z - halfLength;

    float relX = worldPos.x - startX;
    float relZ = worldPos.z - startZ;

    int x = static_cast<int>(relX / m_cellSize);
    int y = static_cast<int>(relZ / m_cellSize);

    if (x >= 0 && x < m_width && y >= 0 && y < m_length)
    {
        return m_nodeVector[x][y];
    }
    return nullptr;
}

vector<Node<Tile>*> TileMap::GeneratePath(Node<Tile>* start, Node<Tile>* target)
{
    if (FindPath(start, target))
    {
        return ReconstructPath(start, target);
    }
    return {};
}

void TileMap::ClearPathData()
{
    for (auto& row : m_nodeVector)
    {
        for (auto& node : row)
        {
            node->data->distanceToStart = 0.f;
            node->data->distanceToEnd = 0.f;
            node->data->totalCost = 0.f;
            node->data->way = false;
            node->visited = false;
            node->cameFrom = nullptr;
        }
    }
}

vector<Node<Tile>*> TileMap::ReconstructPath(Node<Tile>* start, Node<Tile>* target) {
    std::vector<Node<Tile>*> path;
    Node<Tile>* current = target;

    while (current != nullptr) {
        path.push_back(current);
        current->data->way = true;
        if (current == start) break;
        current = current->cameFrom;
    }

    std::reverse(path.begin(), path.end());

    if (!path.empty() && path[0] == start) {
        return path;
    }
    
    return {};
}

bool TileMap::FindPath(Node<Tile>* const& start, Node<Tile>* const& target)
{
    if (!start || !target )
    {
        return false;
    }

    std::priority_queue<Node<Tile>*, std::vector<Node<Tile>*>, CompareTileAStar> priority;

    const float D = 1.0f;
    const float D2 = std::sqrt(2.0f);

    for (auto& row : m_nodeVector) {
        for (auto& node : row) {
            node->data->distanceToStart = std::numeric_limits<float>::infinity();
            node->data->totalCost = std::numeric_limits<float>::infinity();
            node->cameFrom = nullptr;
            node->visited = false;
        }
    }

    start->data->distanceToStart = 0.0f;
    int dx = std::abs(start->data->gridX - target->data->gridX);
    int dy = std::abs(start->data->gridY - target->data->gridY);
    start->data->distanceToEnd = D * (dx + dy) + (D2 - 2 * D) * min(dx, dy);
    start->data->totalCost = start->data->distanceToEnd;
        
    priority.push(start);

    while (!priority.empty()) {
        Node<Tile>* current = priority.top();
        priority.pop();

        if (current == target) {
            return true;
        }

        if (current->visited) continue;
        current->visited = true;

        for (Node<Tile>* neighbor : current->neighbors) {
            if (neighbor == nullptr || !neighbor->data->walkable) continue;

            float movementCost = (std::abs(current->data->gridX - neighbor->data->gridX) == 1 && std::abs(current->data->gridY - neighbor->data->gridY) == 1) ? D2 : D;
            movementCost *= neighbor->data->cost;

            float newDistToStart = current->data->distanceToStart + movementCost;

            if (newDistToStart < neighbor->data->distanceToStart) {
                neighbor->data->distanceToStart = newDistToStart;
                dx = std::abs(neighbor->data->gridX - target->data->gridX);
                dy = std::abs(neighbor->data->gridY - target->data->gridY);
                neighbor->data->distanceToEnd = D * (dx + dy) + (D2 - 2 * D) * min(dx, dy);
                neighbor->data->totalCost = newDistToStart + neighbor->data->distanceToEnd;
                neighbor->cameFrom = current;
                
                priority.push(neighbor);
            }
        }
    }
	return false;
}

void TileMap::SetWalkableWithCollider(BoxCollider box, bool walkable)
{
    Vector3f32 boxMin = box.GetWorldBox().min;
    Vector3f32 boxMax = box.GetWorldBox().max;
    Node<Tile>* topLeftNode = GetNodeFromWorldPosition({ boxMin.x, 0.f, boxMin.z });
    Node<Tile>* bottomRightNode = GetNodeFromWorldPosition({ boxMax.x, 0.f, boxMax.z });
    if (!topLeftNode || !bottomRightNode) return;
    for (int i = topLeftNode->data->gridX; i <= bottomRightNode->data->gridX; ++i)
    {
        for (int j = topLeftNode->data->gridY; j <= bottomRightNode->data->gridY; ++j)
        {
            if (i >= 0 && i < m_width && j >= 0 && j < m_length)
            {
                m_nodeVector[i][j]->data->walkable = walkable;
				std::cout << m_nodeVector[i][j]->data->gridX << ", " << m_nodeVector[i][j]->data->gridY << " walkable: " << m_nodeVector[i][j]->data->walkable << std::endl;
            }
        }
	}
}