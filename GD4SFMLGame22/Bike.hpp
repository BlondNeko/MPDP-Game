#pragma once
#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include "BikeType.hpp"
#include "CommandQueue.hpp"
#include "TextNode.hpp"

class Bike : public Entity
{
public:

	Bike(BikeType type, const TextureHolder & textures, const FontHolder & fonts);
	unsigned int GetCategory() const override;

	void IncreaseSpeed(unsigned int speedUp);
	void DecreaseSpeed(unsigned int speedDown);

	void CollectBoost();
	void UpdateTexts();
	void UpdateMovementPattern(sf::Time dt);
	float GetMaxSpeed() const;
	void UseBoost();

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;

private:
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	bool IsPlayer1() const;

private:
	BikeType m_type;
	sf::Sprite m_sprite;

	Command m_use_boost_command;

	bool m_is_marked_for_removal;
	bool m_can_use_boost;

	float m_speed;
	float m_offroad_resistance;

	//TextNode* m_player_display;
	TextNode* m_boost_display;
	//float m_travelled_distance;
	//int m_directions_index;
};
