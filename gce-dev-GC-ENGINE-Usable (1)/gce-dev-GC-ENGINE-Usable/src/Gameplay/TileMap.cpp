#include "TileMap.h"
#include <algorithm>
#include <queue>
#include <cmath>

namespace gce
{
    TileMap* g_tileMapInstance = nullptr;

    TileMap::TileMap(int width, int height, float cellSize, Vector3f32 const& origin): m_width(width), m_height(height), m_cellSize(cellSize), m_origin(origin)
    {
        m_walkable.Resize(width * height);
        for (int i = 0; i < width * height; ++i) m_walkable[i] = 1;
    }

    void TileMap::CreateInstance(int width, int height, float cellSize, Vector3f32 const& origin)
    {
        if (g_tileMapInstance == nullptr)
            g_tileMapInstance = new TileMap(width, height, cellSize, origin);
    }

    TileMap* TileMap::Instance()
    {
        return g_tileMapInstance;
    }


    bool TileMap::IsInside(int x, int y) const
    {
        return x >= 0 && y >= 0 && x < m_width && y < m_height;
    }

    bool TileMap::IsWalkable(int x, int y) const
    {
        if (!IsInside(x, y)) return false;
        return m_walkable[y * m_width + x] != 0;
    }

    void TileMap::SetWalkable(int x, int y,bool walkable)
    {
        if (!IsInside(x, y)) return;
        if (m_walkable[y * m_width + x]) walkable = 1;
        else walkable = 0;
    }

    Vector2i32 TileMap::WorldToGrid(Vector3f32 const& world) const
    {
        Vector3f32 rel = { world.x - m_origin.x, world.y - m_origin.y, world.z - m_origin.z };
        int gx = static_cast<int>(std::floor(rel.x / m_cellSize));
        int gy = static_cast<int>(std::floor(rel.y / m_cellSize));
        return { gx, gy };
    }

    Vector3f32 TileMap::GridToWorld(int x, int y) const
    {
        float wx = m_origin.x + (x + 0.5f) * m_cellSize;
        float wy = m_origin.y + (y + 0.5f) * m_cellSize;
        return { wx, wy, m_origin.z };
    }

    int TileMap::idx(int x, int y) const {
        return y * m_width + x;
    }

    Vector<Vector3f32> TileMap::FindPath(Vector3f32 const& startWorld, Vector3f32 const& goalWorld) const
    {
       
    }
