#include "Aircraft.hpp"

#include <iostream>

#include "DataTables.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "DataTables.hpp"
#include "SoundNode.hpp"


namespace
{
	const std::vector<AircraftData> Table = InitializeAircraftData();
}

Aircraft::Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts, bool isPlayer1)
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
	, m_explosion(textures.Get(Textures::kExplosion))
	, m_speed(Table[static_cast<int>(type)].m_speed)
	, m_offroad_resistance(Table[static_cast<int>(type)].m_offroad_resistance)
	, m_max_speed(Table[static_cast<int>(type)].m_max_speed)
	, m_boost_ready(true)
	, m_use_boost(false)
	, m_played_explosion_sound(false)
	, m_is_player1(isPlayer1)
, m_is_marked_for_removal(false)
, m_show_explosion(true)
, m_health_display(nullptr)
, m_player_display(nullptr)
, m_boost_display(nullptr)
, m_travelled_distance(0.f)
, m_directions_index(0)
, m_counter(0)
{
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));

	Utility::CentreOrigin(m_sprite);
	Utility::CentreOrigin(m_explosion);
	
	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	healthDisplay->setPosition(0, 25);
	m_health_display = healthDisplay.get();
	AttachChild(std::move(healthDisplay));

	std::unique_ptr<TextNode> boostDisplay(new TextNode(fonts, ""));
	boostDisplay->setPosition(0, -20);
	m_boost_display = boostDisplay.get();
	AttachChild(std::move(boostDisplay));

	std::unique_ptr<TextNode> playerDisplay(new TextNode(fonts, ""));

	if (m_is_player1)
	{
		playerDisplay->SetString("Player 1");
	}
	else
	{
		playerDisplay->SetString("Player 2");
	}

	playerDisplay->setPosition(0, 25);
	m_player_display = playerDisplay.get();
	AttachChild(std::move(playerDisplay));

	UpdateTexts();

}

void Aircraft::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(IsDestroyed() && m_show_explosion)
	{
		target.draw(m_explosion, states);
	}
	else
	{
		target.draw(m_sprite, states);
	}
}

unsigned int Aircraft::GetCategory() const
{
	if (m_is_player1)
	{
		return static_cast<int>(Category::kPlayer1);
	}
	return static_cast<int>(Category::kPlayer2);
}

void Aircraft::CollectBoost()
{
	m_boost_ready = true;
}

void Aircraft::UpdateTexts()
{
	m_health_display->SetString(std::to_string(GetHitPoints()) + "HP");
	m_health_display->setPosition(0.f, 50.f);
	m_health_display->setRotation(-getRotation());

	if (m_is_player1)
	{
		m_player_display->SetString("Player 1");
	}
	else
	{
		m_player_display->SetString("Player 2");
	}

	if (m_boost_display)
	{
		if (m_boost_ready)
		{
			m_boost_display->SetString("Boost Ready!");
		}
		else
		{
			m_boost_display->SetString("");
		}
	}
}

void Aircraft::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	UpdateTexts();
	UpdateRollAnimation();
	UpdateSpeed();

	if(IsDestroyed())
	{
		m_explosion.Update(dt);

		// Play explosion sound only once
		if (!m_played_explosion_sound)
		{
			SoundEffect soundEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, soundEffect);

			m_played_explosion_sound = true;
		}
		return;
	}

	// Update enemy movement pattern; apply velocity
	UpdateMovementPattern(dt);
	Entity::UpdateCurrent(dt, commands);
}

void Aircraft::UpdateMovementPattern(sf::Time dt)
{
	//Enemy AI
	const std::vector<Direction>& directions = Table[static_cast<int>(m_type)].m_directions;
	if(!directions.empty())
	{
		//Move along the current direction, change direction
		if(m_travelled_distance > directions[m_directions_index].m_distance)
		{
			m_directions_index = (m_directions_index + 1) % directions.size();
			m_travelled_distance = 0.f;
		}

		//Compute velocity from direction
		double radians = Utility::ToRadians(directions[m_directions_index].m_angle + 90.f);
		float vx = GetMaxSpeed() * std::cos(radians);
		float vy = GetMaxSpeed() * std::sin(radians);

		SetVelocity(vx, vy);
		m_travelled_distance += GetMaxSpeed() * dt.asSeconds();

	}
}

void Aircraft::UpdateSpeed()
{
	float maxSpeedBoost = (m_max_speed / 100.f);

	if (m_use_boost)
	{
		PlayRollAnimation();
		m_speed += maxSpeedBoost;
		++m_counter;
		if (m_counter > 250)
		{
			m_use_boost = false;
			m_counter = 0;
		}
	}

	if (m_speed < 100.f)
	{
		m_speed += maxSpeedBoost;
	}
	else if (m_speed < m_max_speed)
	{
		m_speed += (maxSpeedBoost / 10.f);
	}
	else if(!m_use_boost && m_speed > m_max_speed)
	{
		m_speed -= maxSpeedBoost;
	}
}

float Aircraft::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_max_speed;
}

float Aircraft::GetSpeed() const
{
	return m_speed;
}

float Aircraft::GetOffroadResistance() const
{
	return m_offroad_resistance;
}

void Aircraft::UseBoost()
{
	if(m_boost_ready && !m_use_boost)
	{
		m_boost_ready = false;
		m_use_boost = true;
	}
}

void Aircraft::IncreaseSpeed(float speedUp)
{
	m_speed = m_speed + (m_speed * speedUp);
	if (m_speed > m_max_speed)
		m_speed = m_max_speed;
}

void Aircraft::DecreaseSpeed(float speedDown)
{
	m_speed = m_speed - (m_speed * speedDown);
	m_speed = m_speed - (m_speed * m_offroad_resistance);
	if (m_speed < 0)
		m_speed = 0;
}

bool Aircraft::IsAllied() const
{
	return m_type == AircraftType::kPlayer1;
}

bool Aircraft::IsPlayer1() const
{
	return m_is_player1;
}

void Aircraft::SetPlayer1(bool isPlayer1)
{
	m_is_player1 = isPlayer1;
}

sf::FloatRect Aircraft::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Aircraft::IsMarkedForRemoval() const
{
	return IsDestroyed() && (m_explosion.IsFinished() || !m_show_explosion);
}

void Aircraft::Remove()
{
	Entity::Remove();
	m_show_explosion = false;
}

void Aircraft::PlayRollAnimation()
{
	if (Table[static_cast<int>(m_type)].m_has_roll_animation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(m_type)].m_texture_rect;

		if (GetVelocity().x >= 100.f)
			textureRect.left = textureRect.width;

		// Roll right: Texture rect offset twice
		else if (GetVelocity().x <= 90.f)
			textureRect.left += textureRect.width;

		m_sprite.setTextureRect(textureRect);
	}
}

void Aircraft::UpdateRollAnimation()
{
	if (Table[static_cast<int>(m_type)].m_has_roll_animation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(m_type)].m_texture_rect;

		// Roll left: Texture rect offset once
		if (GetVelocity().x < 0.f)
			textureRect.left = textureRect.width;

		// Roll right: Texture rect offset twice
		else if (GetVelocity().x > 0.f)
			textureRect.left += textureRect.width;

		m_sprite.setTextureRect(textureRect);
	}
}

void Aircraft::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = Category::kSoundEffect;
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
	{
		node.PlaySound(effect, world_position);
	});

	commands.Push(command);
}




