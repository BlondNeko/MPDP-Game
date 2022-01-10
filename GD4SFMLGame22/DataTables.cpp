#include "DataTables.hpp"
#include "AircraftType.hpp"
#include "Aircraft.hpp"
#include "BikeType.hpp"
#include "ObstacleType.hpp"
#include "ParticleType.hpp"
#include "PickupType.hpp"
#include "ProjectileType.hpp"

std::vector<AircraftData> InitializeAircraftData()
{
	std::vector<AircraftData> data(static_cast<int>(AircraftType::kAircraftCount));

	data[static_cast<int>(AircraftType::kNormal)].m_hitpoints = 100;
	data[static_cast<int>(AircraftType::kNormal)].m_speed = 200.f;
	data[static_cast<int>(AircraftType::kNormal)].m_max_speed = 250.f;
	data[static_cast<int>(AircraftType::kNormal)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(AircraftType::kNormal)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kNormal)].m_texture_rect = sf::IntRect(58, 0, 57, 29);
	data[static_cast<int>(AircraftType::kNormal)].m_has_roll_animation = true;
	data[static_cast<int>(AircraftType::kNormal)].m_offroad_resistance = 0.2f;

	data[static_cast<int>(AircraftType::KNitro)].m_hitpoints = 80;
	data[static_cast<int>(AircraftType::KNitro)].m_speed = 210.f;
	data[static_cast<int>(AircraftType::KNitro)].m_max_speed = 275.f;
	data[static_cast<int>(AircraftType::KNitro)].m_fire_interval = sf::Time::Zero;
	data[static_cast<int>(AircraftType::KNitro)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::KNitro)].m_texture_rect = sf::IntRect(58, 30, 57, 29);
	data[static_cast<int>(AircraftType::KNitro)].m_offroad_resistance = 0.1f;

	data[static_cast<int>(AircraftType::kOffroader)].m_hitpoints = 120;
	data[static_cast<int>(AircraftType::kOffroader)].m_speed = 180.f;
	data[static_cast<int>(AircraftType::kOffroader)].m_max_speed = 225.f;
	data[static_cast<int>(AircraftType::kOffroader)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kOffroader)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kOffroader)].m_texture_rect = sf::IntRect(58, 60, 57, 29);
	data[static_cast<int>(AircraftType::kOffroader)].m_offroad_resistance = 0.3f;

	data[static_cast<int>(AircraftType::kRacer)].m_hitpoints = 120;
	data[static_cast<int>(AircraftType::kRacer)].m_speed = 220.f;
	data[static_cast<int>(AircraftType::kRacer)].m_max_speed = 250.f;
	data[static_cast<int>(AircraftType::kRacer)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kRacer)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kRacer)].m_texture_rect = sf::IntRect(58, 90, 57, 29);
	data[static_cast<int>(AircraftType::kRacer)].m_offroad_resistance = 0.2f;

	data[static_cast<int>(AircraftType::kSpecialA)].m_hitpoints = 80;
	data[static_cast<int>(AircraftType::kSpecialA)].m_speed = 180.f;
	data[static_cast<int>(AircraftType::kSpecialA)].m_max_speed = 275.f;
	data[static_cast<int>(AircraftType::kSpecialA)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kSpecialA)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kSpecialA)].m_texture_rect = sf::IntRect(54, 120, 26, 33);
	data[static_cast<int>(AircraftType::kSpecialA)].m_offroad_resistance = 0.3f;

	data[static_cast<int>(AircraftType::kSpecialB)].m_hitpoints = 50;
	data[static_cast<int>(AircraftType::kSpecialB)].m_speed = 180.f;
	data[static_cast<int>(AircraftType::kSpecialB)].m_max_speed = 300.f;
	data[static_cast<int>(AircraftType::kSpecialB)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kSpecialB)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kSpecialB)].m_texture_rect = sf::IntRect(51, 154, 25, 25);
	data[static_cast<int>(AircraftType::kSpecialB)].m_offroad_resistance = 0.2f;

	data[static_cast<int>(AircraftType::kSpecialC)].m_hitpoints = 150;
	data[static_cast<int>(AircraftType::kSpecialC)].m_speed = 150.f;
	data[static_cast<int>(AircraftType::kSpecialC)].m_max_speed = 250.f;
	data[static_cast<int>(AircraftType::kSpecialC)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kSpecialC)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kSpecialC)].m_texture_rect = sf::IntRect(60, 0, 59, 32);
	data[static_cast<int>(AircraftType::kSpecialC)].m_offroad_resistance = 0.5f;

	data[static_cast<int>(AircraftType::kSpecialD)].m_hitpoints = 150;
	data[static_cast<int>(AircraftType::kSpecialD)].m_speed = 150.f;
	data[static_cast<int>(AircraftType::kSpecialD)].m_max_speed = 250.f;
	data[static_cast<int>(AircraftType::kSpecialD)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(AircraftType::kSpecialD)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(AircraftType::kSpecialD)].m_texture_rect = sf::IntRect(60, 213, 59, 32);
	data[static_cast<int>(AircraftType::kSpecialD)].m_offroad_resistance = 0.5f;

	return data;
}

/*
std::vector<BikeData> InitializeBikeData()
{
	std::vector<BikeData> data(static_cast<int>(BikeType::kBikeCount));

	data[static_cast<int>(BikeType::kNormal)].m_hitpoints = 100;
	data[static_cast<int>(BikeType::kNormal)].m_speed = 200.f;
	data[static_cast<int>(BikeType::kNormal)].m_offroad_resistance = 0.2f;
	data[static_cast<int>(BikeType::kNormal)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kNormal)].m_texture_rect = sf::IntRect(0, 0, 57, 29);

	data[static_cast<int>(BikeType::kNitro)].m_hitpoints = 80;
	data[static_cast<int>(BikeType::kNitro)].m_speed = 220.f;
	data[static_cast<int>(BikeType::kNitro)].m_offroad_resistance = 0.1f;
	data[static_cast<int>(BikeType::kNitro)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kNitro)].m_texture_rect = sf::IntRect(0, 30, 57, 29);

	data[static_cast<int>(BikeType::kOffroader)].m_hitpoints = 120;
	data[static_cast<int>(BikeType::kOffroader)].m_speed = 175.f;
	data[static_cast<int>(BikeType::kOffroader)].m_offroad_resistance = 0.3f;
	data[static_cast<int>(BikeType::kOffroader)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kOffroader)].m_texture_rect = sf::IntRect(0, 60, 57, 29);

	data[static_cast<int>(BikeType::kRacer)].m_hitpoints = 110;
	data[static_cast<int>(BikeType::kRacer)].m_speed = 210.f;
	data[static_cast<int>(BikeType::kRacer)].m_offroad_resistance = 0.2f;
	data[static_cast<int>(BikeType::kRacer)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kRacer)].m_texture_rect = sf::IntRect(0, 90, 57, 29);

	data[static_cast<int>(BikeType::kSpecialA)].m_hitpoints = 60;
	data[static_cast<int>(BikeType::kSpecialA)].m_speed = 300.f;
	data[static_cast<int>(BikeType::kSpecialA)].m_offroad_resistance = 0.3f;
	data[static_cast<int>(BikeType::kSpecialA)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kSpecialA)].m_texture_rect = sf::IntRect(0, 120, 26, 33);

	data[static_cast<int>(BikeType::kSpecialB)].m_hitpoints = 40;
	data[static_cast<int>(BikeType::kSpecialB)].m_speed = 350.f;
	data[static_cast<int>(BikeType::kSpecialB)].m_offroad_resistance = 0.25f;
	data[static_cast<int>(BikeType::kSpecialB)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kSpecialB)].m_texture_rect = sf::IntRect(0, 154, 25, 25);

	data[static_cast<int>(BikeType::kSpecialC)].m_hitpoints = 150;
	data[static_cast<int>(BikeType::kSpecialC)].m_speed = 160.f;
	data[static_cast<int>(BikeType::kSpecialC)].m_offroad_resistance = 0.5f;
	data[static_cast<int>(BikeType::kSpecialC)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kSpecialC)].m_texture_rect = sf::IntRect(0, 0, 59, 32);

	data[static_cast<int>(BikeType::kSpecialD)].m_hitpoints = 150;
	data[static_cast<int>(BikeType::kSpecialD)].m_speed = 160.f;
	data[static_cast<int>(BikeType::kSpecialD)].m_offroad_resistance = 0.5f;
	data[static_cast<int>(BikeType::kSpecialD)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(BikeType::kSpecialD)].m_texture_rect = sf::IntRect(0, 213, 59, 32);

	return data;
}
*/

std::vector<ProjectileData> InitializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));

	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture = Textures::kEntities;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture_rect = sf::IntRect(175, 64, 3, 14);

	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture = Textures::kEntities;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture_rect = sf::IntRect(178, 64, 3, 14);

	data[static_cast<int>(ProjectileType::kMissile)].m_damage = 200;
	data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150.f;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture = Textures::kEntities;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture_rect = sf::IntRect(160, 64, 15, 32);
	return data;
}


std::vector<ObstacleData> InitializeObstacleData()
{
	std::vector<ObstacleData> data(static_cast<int>(ObstacleType::kObstacleCount));

	data[static_cast<int>(ObstacleType::kTarSpill)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(ObstacleType::kTarSpill)].m_texture_rect = sf::IntRect(123, 153, 45, 19);
	data[static_cast<int>(ObstacleType::kTarSpill)].m_slow_down_amount = 0.4f;

	data[static_cast<int>(ObstacleType::kAcidSpill)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(ObstacleType::kAcidSpill)].m_texture_rect = sf::IntRect(124, 132, 45, 19);
	data[static_cast<int>(ObstacleType::kAcidSpill)].m_slow_down_amount = 0.2f;

	data[static_cast<int>(ObstacleType::kBarrier)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(ObstacleType::kBarrier)].m_texture_rect = sf::IntRect(182, 86, 17, 29);
	data[static_cast<int>(ObstacleType::kBarrier)].m_slow_down_amount = 0.9f;
	return data;
}

std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = Textures::kEntities;
	data[static_cast<int>(PickupType::kHealthRefill)].m_texture_rect = sf::IntRect(0, 64, 40, 40);
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Aircraft& a) {a.Repair(25); };

	data[static_cast<int>(PickupType::kBoost)].m_texture = Textures::kSpriteSheet;
	data[static_cast<int>(PickupType::kBoost)].m_texture_rect = sf::IntRect(171, 122, 29, 28);
	data[static_cast<int>(PickupType::kBoost)].m_action = [](Aircraft& a) {a.CollectBoost(); };
	//Other pickup: top:150, left:171

	data[static_cast<int>(PickupType::kMissileRefill)].m_texture = Textures::kEntities;
	data[static_cast<int>(PickupType::kMissileRefill)].m_texture_rect = sf::IntRect(40, 64, 40, 40);
	//data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Aircraft::CollectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupType::kFireSpread)].m_texture = Textures::kEntities;
	data[static_cast<int>(PickupType::kFireSpread)].m_texture_rect = sf::IntRect(80, 64, 40, 40);
	//data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Aircraft::IncreaseSpread, std::placeholders::_1);

	data[static_cast<int>(PickupType::kFireRate)].m_texture = Textures::kEntities;
	data[static_cast<int>(PickupType::kFireRate)].m_texture_rect = sf::IntRect(120, 64, 40, 40);
	//data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Aircraft::IncreaseFireRate, std::placeholders::_1);
	return data;
}

std::vector<ParticleData> InitializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

	data[static_cast<int>(ParticleType::kPropellant)].m_color = sf::Color(255, 255, 50);
	data[static_cast<int>(ParticleType::kPropellant)].m_lifetime = sf::seconds(0.6f);

	data[static_cast<int>(ParticleType::kSmoke)].m_color = sf::Color(50, 50, 50);
	data[static_cast<int>(ParticleType::kSmoke)].m_lifetime = sf::seconds(4.f);

	return data;
}



