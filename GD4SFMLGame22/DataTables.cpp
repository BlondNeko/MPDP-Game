#include "DataTables.hpp"
#include "AircraftType.hpp"
#include "Aircraft.hpp"
#include "BikeType.hpp"
#include "ObstacleType.hpp"
#include "PickupType.hpp"
#include "ProjectileType.hpp"

std::vector<AircraftData> InitializeAircraftData()
{
	std::vector<AircraftData> data(static_cast<int>(AircraftType::kAircraftCount));

	data[static_cast<int>(AircraftType::kEagle)].m_hitpoints = 100;
	data[static_cast<int>(AircraftType::kEagle)].m_speed = 200.f;
	data[static_cast<int>(AircraftType::kEagle)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(AircraftType::kEagle)].m_texture = Textures::kNormal;

	data[static_cast<int>(AircraftType::kRaptor)].m_hitpoints = 20;
	data[static_cast<int>(AircraftType::kRaptor)].m_speed = 80.f;
	data[static_cast<int>(AircraftType::kRaptor)].m_fire_interval = sf::Time::Zero;
	data[static_cast<int>(AircraftType::kRaptor)].m_texture = Textures::kAcidSpill;
	//AI
	data[static_cast<int>(AircraftType::kRaptor)].m_directions.emplace_back(Direction(+45.f, 80.f));
	data[static_cast<int>(AircraftType::kRaptor)].m_directions.emplace_back(Direction(-45.f, 160.f));
	data[static_cast<int>(AircraftType::kRaptor)].m_directions.emplace_back(Direction(+45.f, 80.f));

	data[static_cast<int>(AircraftType::kAvenger)].m_hitpoints = 40;
	data[static_cast<int>(AircraftType::kAvenger)].m_speed = 50.f;
	data[static_cast<int>(AircraftType::kAvenger)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kAvenger)].m_texture = Textures::kNitro;
	//AI
	data[static_cast<int>(AircraftType::kAvenger)].m_directions.emplace_back(Direction(+45.f, 50.f));
	data[static_cast<int>(AircraftType::kAvenger)].m_directions.emplace_back(Direction(0.f, 50.f));
	data[static_cast<int>(AircraftType::kAvenger)].m_directions.emplace_back(Direction(-45.f, 100.f));
	data[static_cast<int>(AircraftType::kAvenger)].m_directions.emplace_back(Direction(0.f, 50.f));
	data[static_cast<int>(AircraftType::kAvenger)].m_directions.emplace_back(Direction(+45.f, 50.f));

	return data;
}

std::vector<BikeData> InitializeBikeData()
{
	std::vector<BikeData> data(static_cast<int>(BikeType::kBikeCount));

	data[static_cast<int>(BikeType::kNormal)].m_hitpoints = 100;
	data[static_cast<int>(BikeType::kNormal)].m_speed = 200.f;
	data[static_cast<int>(BikeType::kNormal)].m_offroad_resistance = 0.2f;
	data[static_cast<int>(BikeType::kNormal)].m_texture = Textures::kNormal;

	data[static_cast<int>(BikeType::kNitro)].m_hitpoints = 80;
	data[static_cast<int>(BikeType::kNitro)].m_speed = 220.f;
	data[static_cast<int>(BikeType::kNitro)].m_offroad_resistance = 0.1f;
	data[static_cast<int>(BikeType::kNitro)].m_texture = Textures::kNitro;

	data[static_cast<int>(BikeType::kOffroader)].m_hitpoints = 120;
	data[static_cast<int>(BikeType::kOffroader)].m_speed = 175.f;
	data[static_cast<int>(BikeType::kOffroader)].m_offroad_resistance = 0.3f;
	data[static_cast<int>(BikeType::kOffroader)].m_texture = Textures::kOffroader;

	data[static_cast<int>(BikeType::kRacer)].m_hitpoints = 110;
	data[static_cast<int>(BikeType::kRacer)].m_speed = 210.f;
	data[static_cast<int>(BikeType::kRacer)].m_offroad_resistance = 0.2f;
	data[static_cast<int>(BikeType::kRacer)].m_texture = Textures::kRacer;

	data[static_cast<int>(BikeType::kSpecialA)].m_hitpoints = 60;
	data[static_cast<int>(BikeType::kSpecialA)].m_speed = 300.f;
	data[static_cast<int>(BikeType::kSpecialA)].m_offroad_resistance = 0.3f;
	data[static_cast<int>(BikeType::kSpecialA)].m_texture = Textures::kSpecialA;

	data[static_cast<int>(BikeType::kSpecialB)].m_hitpoints = 40;
	data[static_cast<int>(BikeType::kSpecialB)].m_speed = 350.f;
	data[static_cast<int>(BikeType::kSpecialB)].m_offroad_resistance = 0.25f;
	data[static_cast<int>(BikeType::kSpecialB)].m_texture = Textures::kSpecialB;

	data[static_cast<int>(BikeType::kSpecialC)].m_hitpoints = 150;
	data[static_cast<int>(BikeType::kSpecialC)].m_speed = 160.f;
	data[static_cast<int>(BikeType::kSpecialC)].m_offroad_resistance = 0.5f;
	data[static_cast<int>(BikeType::kSpecialC)].m_texture = Textures::kSpecialC;

	data[static_cast<int>(BikeType::kSpecialD)].m_hitpoints = 150;
	data[static_cast<int>(BikeType::kSpecialD)].m_speed = 160.f;
	data[static_cast<int>(BikeType::kSpecialD)].m_offroad_resistance = 0.5f;
	data[static_cast<int>(BikeType::kSpecialD)].m_texture = Textures::kSpecialD;

	return data;
}

std::vector<ProjectileData> InitializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));

	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kMissile)].m_damage = 200;
	data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150.f;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture = Textures::kMissile;
	return data;
}


std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = Textures::kHealthRefill;
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Aircraft& a) {a.Repair(25); };

	data[static_cast<int>(PickupType::kMissileRefill)].m_texture = Textures::kMissileRefill;
	data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Aircraft::CollectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupType::kFireSpread)].m_texture = Textures::kFireSpread;
	data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Aircraft::IncreaseSpread, std::placeholders::_1);

	data[static_cast<int>(PickupType::kFireRate)].m_texture = Textures::kFireRate;
	data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Aircraft::IncreaseFireRate, std::placeholders::_1);
	return data;
}

std::vector<ObstacleData> InitializeObstacleData()
{
	std::vector<ObstacleData> data(static_cast<int>(ObstacleType::kObstacleCount));

	data[static_cast<int>(ObstacleType::kBarrier)].m_texture = Textures::kBarrier;
	data[static_cast<int>(ObstacleType::kTarSpill)].m_texture = Textures::kTarSpill;
	data[static_cast<int>(ObstacleType::kAcidSpill)].m_texture = Textures::kAcidSpill;
	return data;
}

