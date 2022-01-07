#include "Bike.hpp"

#include <iostream>

#include "DataTables.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Bike.hpp"

namespace
{
	const std::vector<BikeData> Table = InitializeBikeData();
}

Bike::Bike(BikeType type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
	, m_speed(Table[static_cast<int>(type)].m_offroad_resistance)
	, m_offroad_resistance(Table[static_cast<int>(type)].m_offroad_resistance)
	, m_is_marked_for_removal(false)
	, m_boost_display(nullptr)
	, m_can_use_boost(false)
{
	Utility::CentreOrigin(m_sprite);

	m_use_boost_command.category = static_cast<int>(Category::Type::kScene);
	m_use_boost_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		//CreateBullets(node, textures);
		UseBoost();
	};

	/*std::unique_ptr<TextNode> playerDisplay(new TextNode(fonts, ""));
	m_player_display = playerDisplay.get();
	AttachChild(std::move(playerDisplay));

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	m_health_display = healthDisplay.get();
	AttachChild(std::move(healthDisplay));*/

	std::unique_ptr<TextNode> boostDisplay(new TextNode(fonts, ""));
	m_boost_display = boostDisplay.get();
	AttachChild(std::move(boostDisplay));
	

	UpdateTexts();

}

void Bike::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

unsigned int Bike::GetCategory() const
{
	if (!IsPlayer1())
	{
		return static_cast<int>(Category::kPlayer2);
	}
	return static_cast<int>(Category::kPlayer1);
}

void Bike::IncreaseSpeed(unsigned speedIncrease)
{
	m_speed += speedIncrease;
}

void Bike::DecreaseSpeed(unsigned speedDecrease)
{
	assert(m_speed > 50 && speedDecrease < 20);
	m_speed -= speedDecrease;
}

void Bike::CollectBoost()
{
	m_can_use_boost = true;
}

void Bike::UseBoost()
{
	assert(m_can_use_boost);
	m_speed += 50;
}

void Bike::UpdateTexts()
{
	/*m_health_display->SetString(std::to_string(GetHitPoints()) + "HP");
	m_health_display->setPosition(0.f, -50.f);
	m_health_display->setRotation(-getRotation());*/

	if (m_boost_display)
	{
		if (m_boost_display == 0)
		{
			m_boost_display->SetString("");
		}
		else
		{
			m_boost_display->SetString("Boost ready!");
		}
	}

}

void Bike::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsDestroyed())
	{
		m_is_marked_for_removal = true;
		return;
	}

	Entity::UpdateCurrent(dt, commands);
	UpdateTexts();
}

float Bike::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

bool Bike::IsPlayer1() const
{
	return true;
}

sf::FloatRect Bike::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Bike::IsMarkedForRemoval() const
{
	return m_is_marked_for_removal;
}