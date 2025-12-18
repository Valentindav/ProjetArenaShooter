#pragma once
#include "Ennemy.h"
#include "TileMap.h"
#include <vector>

class Elf : public Ennemy
{
private:
	TileMap* m_tileMap = nullptr;
	std::vector<Node<Tile>*> m_currentPath;
	Vector2f32 m_currentNodeIndex = { 0.f, 0.f };
	float m_repathTimer = 0.f;

public:
	Elf(GameObject* obj, TileMap* tileMap, float spd = 150);
	void Die() override;
	void Attack()override;
	void AddScript();
	float m_ShootCooldown = 0.75f;
	void GeneratePathToPlayer(GameObject* player);
	TileMap* GetTileMap() const { return m_tileMap; }
	std::vector<Node<Tile>*>& GetPath() { return m_currentPath; }
	void SetCurrentTargetNodePosition();
	void FollowPath();
};

