#pragma once
#include "Obstacle.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

namespace
{
	const std::vector<ObstacleData> Table = InitializeObstacleData();
}

Obstacle::Obstacle(ObstacleType type, const TextureHolder& textures)
	: Entity(100)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
	, m_is_marked_for_removal(false)
{
	Utility::CentreOrigin(m_sprite);
}

unsigned Obstacle::GetCategory() const
{
	return static_cast<int>(Category::kObstacle);
}

sf::FloatRect Obstacle::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Obstacle::IsMarkedForRemoval() const
{
	return m_is_marked_for_removal;
}

void Obstacle::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

void Obstacle::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	Entity::UpdateCurrent(dt, commands);
}
