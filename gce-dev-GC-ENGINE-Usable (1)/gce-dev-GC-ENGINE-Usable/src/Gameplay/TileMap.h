#pragma once
#pragma once

#include <Containers/Vector.hpp>
#include "Maths/Vector3.h"
#include "Maths/Vector2.hpp"
    
    using namespace gce;
    using namespace std;

    struct Node { 
        int x, y;
        float g, h;
        int parentIdx;
    };

    class TileMap
    {
    private:
        int m_width;
        int m_height;
        float m_cellSize;
        Vector3f32 m_origin;
        Vector<uint8> m_walkable;
    public:

        TileMap(int width, int height, float cellSize, Vector3f32 const& origin = { 0,0,0 });
        ~TileMap() = default;

        static void CreateInstance(int width, int height, float cellSize, Vector3f32 const& origin = { 0,0,0 });
        static TileMap* Instance();

        bool IsInside(int x, int y) const;
        bool IsWalkable(int x, int y) const;
        void SetWalkable(int x, int y, bool walkable);

        Vector2i32 WorldToGrid(Vector3f32 const& world) const;
        Vector3f32 GridToWorld(int x, int y) const;

        Vector<Vector3f32> FindPath(Vector3f32 const& startWorld, Vector3f32 const& goalWorld) const;

        int idx(int x, int y) const;
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        float GetCellSize() const { return m_cellSize; }
        Vector3f32 GetOrigin() const { return m_origin; }

    };

