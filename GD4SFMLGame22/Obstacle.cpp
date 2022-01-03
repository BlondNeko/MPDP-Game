#include "Obstacle.hpp"
#include "Pickup.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

namespace
{
	const std::vector<ObstacleData> Table = InitializeObstacleData();
}

Obstacle::Obstacle(ObstacleType type, const TextureHolder& textures)
	: Entity(1)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
{
	Utility::CentreOrigin(m_sprite);
}

unsigned Obstacle::GetCategory() const
{
	return Category::Type::kObstacle;
}

sf::FloatRect Obstacle::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Obstacle::Apply(Bike& player) const
{
	Table[static_cast<int>(m_type)].m_action(player);
}

void Obstacle::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}