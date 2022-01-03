#pragma once
#include <functional>
#include <vector>
#include <SFML/System/Time.hpp>

#include "ResourceIdentifiers.hpp"

class Aircraft;
class Bike;

struct Direction
{
	Direction(float angle, float distance)
		: m_angle(angle), m_distance(distance)
	{
	}
	float m_angle;
	float m_distance;
};

struct AircraftData
{
	int m_hitpoints;
	float m_speed;
	Textures m_texture;
	sf::Time m_fire_interval;
	std::vector<Direction> m_directions;
};

struct ObstacleData
{
	std::function<void(Bike&)> m_action;
	float m_speed_down;
	Textures m_texture;
};

struct BikeData
{
	int m_hitpoints;
	float m_offroad_resistance;
	float m_initial_speed;
	Textures m_texture;
};

struct ProjectileData
{
	int m_damage;
	float m_speed;
	Textures m_texture;
};

struct PickupData
{
	std::function<void(Aircraft&)> m_action;
	Textures m_texture;
};

std::vector<AircraftData> InitializeAircraftData();
std::vector<BikeData> InitializeBikeData();
std::vector<ProjectileData> InitializeProjectileData();
std::vector<PickupData> InitializePickupData();
std::vector<ObstacleData> InitializeObstacleData();