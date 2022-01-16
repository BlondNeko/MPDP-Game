#pragma once
#include "Entity.hpp"
#include "AircraftType.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include "Animation.hpp"
#include "CommandQueue.hpp"
#include "TextNode.hpp"


class Aircraft : public Entity
{
public:
	Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts, bool isPlayer1);
	unsigned int GetCategory() const override;

	void IncreaseFireRate();
	void IncreaseSpread();
	void UpdateTexts();
	void UpdateMovementPattern(sf::Time dt);
	float GetMaxSpeed() const;
	void UseBoost();
	void CollectBoost();
	void IncreaseSpeed(float speed);
	void DecreaseSpeed(float speed);
	float GetSpeed() const;
	float GetOffroadResistance() const;
	bool IsPlayer1() const;
	void SetPlayer1(bool isPlayer1);

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	void Remove() override;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);


private:
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	bool IsAllied() const;
	void PlayRollAnimation();
	void UpdateRollAnimation();
	void UpdateSpeed();

private:
	AircraftType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;

	bool m_boost_ready;
	bool m_use_boost;
	float m_speed;
	float m_offroad_resistance;
	unsigned int m_counter;

	bool m_is_marked_for_removal;
	bool m_show_explosion;
	bool m_played_explosion_sound;
	bool m_is_player1;

	float m_max_speed;
	TextNode* m_health_display;
	TextNode* m_boost_display;
	TextNode* m_player_display;
	float m_travelled_distance;
	int m_directions_index;
};

