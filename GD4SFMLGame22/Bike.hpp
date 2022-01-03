#pragma once
#include "Entity.hpp"
#include "BikeType.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include "CommandQueue.hpp"
#include "TextNode.hpp"

class Bike : public Entity
{
public:
	Bike(BikeType type, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int GetCategory() const override;

	void IncreaseSpeed(unsigned int speedIncrease);
	void DecreaseSpeed(unsigned int speedDecrease);
	float GetMaxSpeed() const;

	void UpdateTexts();

	void CollectBoost();
	void UseBoost();

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	bool IsPlayer1() const;


private:
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	bool IsAllied() const;

private:
	BikeType m_type;
	sf::Sprite m_sprite;

	Command m_use_boost_command;

	bool m_can_use_boost;
	sf::Time m_boost_cooldown;
	bool m_is_marked_for_removal;
	bool m_is_p_1;

	unsigned int m_current_speed;
	unsigned int m_change_speed;
	float m_offroad_resistance;

	TextNode* m_player_display;
	TextNode* m_health_display;
	TextNode* m_boost_display;
	float m_travelled_distance;
	int m_directions_index;
};

