#pragma once
#include "Ennemy.h"
#include "TileMap.h"
#include <vector>

class SnowMan : public Ennemy
{
private:
	TileMap* m_tileMap = nullptr;
	std::vector<Node<Tile>*> m_currentPath;
	Vector2f32 m_currentNodeIndex = { 0.f, 0.f };
	float m_repathTimer = 0.f;
public:
	SnowMan(GameObject* obj, TileMap* tileMap, float spd = 150);
	void Die() override;
	void Attack()override;
	void AddScript();
	void GeneratePathToPlayer(GameObject* player);
	TileMap* GetTileMap() const { return m_tileMap; }
	std::vector<Node<Tile>*>& GetPath() { return m_currentPath; }
	void SetCurrentTargetNodePosition() ;
	void FollowPath();
	std::vector<Node<Tile>*> const& GetCurrentPath() const { return m_currentPath; }
	float m_ShootCooldown = 0.75f;
};