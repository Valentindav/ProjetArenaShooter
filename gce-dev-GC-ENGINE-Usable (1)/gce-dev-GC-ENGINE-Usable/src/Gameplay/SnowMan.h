#pragma once
#include "Ennemy.h"
#include "TileMap.h"
#include <vector>

class SnowMan : public Ennemy
{
private:
	TileMap* m_tileMap = nullptr;
	std::vector<Node<Tile>*> m_currentPath;
	int m_currentNodeIndex = 0;
	float m_repathTimer = 0.f;

public: 
	SnowMan(GameObject* obj, TileMap* tileMap, float spd = 150);
	void Die() override;
	void Attack()override;
	void AddScript();
	void SetPath(const std::vector<Node<Tile>*>& path);
	void FollowPath();
	TileMap* GetTileMap() const { return m_tileMap; }
	std::vector<Node<Tile>*>& GetPath() { return m_currentPath; }
};

