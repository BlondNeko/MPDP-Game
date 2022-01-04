#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

#include "Pickup.hpp"
#include "Projectile.hpp"
#include "Utility.hpp"

//Note - With the background, the road is 457px in height, so ~455px
World::World(sf::RenderWindow& window, FontHolder& font)
	: m_window(window)
	, m_camera(window.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_scenegraph()
	, m_scene_layers()
	, m_world_bounds(0.f, 0.f, 5000, m_camera.getSize().y)
	, m_spawn_position(m_camera.getSize().x/8.f, m_world_bounds.height - m_camera.getSize().y /6.f)
	, m_scrollspeed(-50.f)
	, m_player_aircraft(nullptr)
{
	LoadTextures();
	BuildScene();
	std::cout << m_camera.getSize().x << m_camera.getSize().y << std::endl;
	//m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	m_camera.move(-(m_scrollspeed * dt.asSeconds()), 0);

	m_player_aircraft->SetVelocity(0.f, 0.f);
	DestroyEntitiesOutsideView();
	GuideMissiles();

	//Forward commands to the scenegraph until the command queue is empty
	while(!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity();

	HandleCollisions();
	//Remove all destroyed entities
	m_scenegraph.RemoveWrecks();

	SpawnEnemies();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();
}

void World::Draw()
{
	m_window.setView(m_camera);
	m_window.draw(m_scenegraph);
}

bool World::HasAlivePlayer() const
{
	return !m_player_aircraft->IsMarkedForRemoval();
}

bool World::HasPlayerReachedEnd() const
{
	return !m_world_bounds.contains(m_player_aircraft->getPosition());
}

void World::LoadTextures()
{
	m_textures.Load(Textures::kEagle, "Media/Textures/Eagle.png");
	m_textures.Load(Textures::kRaptor, "Media/Textures/Raptor.png");
	m_textures.Load(Textures::kAvenger, "Media/Textures/Avenger.png");
	m_textures.Load(Textures::kDesert, "Media/Textures/Desert.png");

	m_textures.Load(Textures::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(Textures::kMissile, "Media/Textures/Missile.png");
	m_textures.Load(Textures::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(Textures::kMissileRefill, "Media/Textures/MissileRefill.png");
	m_textures.Load(Textures::kFireSpread, "Media/Textures/FireSpread.png");
	m_textures.Load(Textures::kFireRate, "Media/Textures/FireRate.png");

	m_textures.Load(Textures::kNormal, "Media/Sprites/BBike1.png");
	m_textures.Load(Textures::kRacer, "Media/Sprites/RBike1.png");
	m_textures.Load(Textures::kNitro, "Media/Sprites/GBike1.png");
	m_textures.Load(Textures::kOffroader, "Media/Sprites/TBike1.png");

	m_textures.Load(Textures::kSpecialA, "Media/Sprites/PIdle.png");
	m_textures.Load(Textures::kSpecialB, "Media/Sprites/BIdle.png");
	m_textures.Load(Textures::kSpecialC, "Media/Sprites/GBBoar1.png");
	m_textures.Load(Textures::kSpecialD, "Media/Sprites/GGBoar1.png");

	m_textures.Load(Textures::kBarrier, "Media/Sprites/Obs1.png");
	m_textures.Load(Textures::KPowerup1, "Media/Sprites/PUp1.png");
	m_textures.Load(Textures::kBoost, "Media/Sprites/PUp2.png");
	m_textures.Load(Textures::kAcidSpill, "Media/Sprites/Spill1.png");
	m_textures.Load(Textures::kTarSpill, "Media/Sprites/Spill2.png");

	m_textures.Load(Textures::kFinish1, "Media/Sprites/FinishLine1.png");
	m_textures.Load(Textures::kFinish2, "Media/Sprites/FinishLine2.png");
	m_textures.Load(Textures::kCity1, "Media/Sprites/Background.png");
	m_textures.Load(Textures::kCity2, "Media/Sprites/Background2.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category = (i == static_cast<int>(Layers::kAir)) ? Category::Type::kScene : Category::Type::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(Textures::kCity1);
	sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	texture.setRepeated(true);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));

	//Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::kEagle, m_textures, m_fonts));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);
	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader));

	AddEnemies();
}

CommandQueue& World::getCommandQueue()
{
	return m_command_queue;
}

void World::AdaptPlayerPosition()
{
	//Keep the player on the screen
	sf::FloatRect view_bounds = GetViewBounds();
	const float border_distance = 40.f;
	sf::Vector2f position = m_player_aircraft->getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance);
	position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);
	m_player_aircraft->setPosition(position);

}

void World::AdaptPlayerVelocity()
{
	sf::Vector2f velocity = m_player_aircraft->GetVelocity();
	//if moving diagonally then reduce velocity
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		m_player_aircraft->SetVelocity(velocity / std::sqrt(2.f));
	}
	//Add scrolling velocity
	m_player_aircraft->Accelerate(m_scrollspeed, 0.f);
	//std::cout << m_player_aircraft->getPosition().x << "  " << m_player_aircraft->getPosition().y << std::endl;
}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top += 100.f;
	bounds.height += 50.f;

	//std::cout << bounds.top << " " << bounds.height << std::endl;
	return bounds;
}

void World::SpawnEnemies()
{
	//Spawn an enemy when they are relevant - they are relevant when they enter the battlefield bounds
	while(!m_enemy_spawn_points.empty() && m_enemy_spawn_points.back().m_y > GetBattlefieldBounds().top)
	{
		SpawnPoint spawn = m_enemy_spawn_points.back();
		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.m_type, m_textures, m_fonts));
		enemy->setPosition(spawn.m_x, spawn.m_y);
		enemy->setRotation(-90.f);
		m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(enemy));

		m_enemy_spawn_points.pop_back();
		
	}
}

void World::AddEnemy(AircraftType type, float relX, float relY)
{
	//SpawnPoint spawn(type, m_spawn_position.x + relX, m_spawn_position.y - relY);
	SpawnPoint spawn(type, relX, relY);
	m_enemy_spawn_points.emplace_back(spawn);
}

void World::AddEnemies()
{
	//Add all enemies
	/*
	 * AddEnemy(AircraftType::kRaptor, 0.f, 200.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 100.f);
	AddEnemy(AircraftType::kRaptor, 100.f, 100.f);
	AddEnemy(AircraftType::kRaptor, -100.f, 100.f);
	AddEnemy(AircraftType::kAvenger, -70.f, 400.f);
	AddEnemy(AircraftType::kAvenger, 70.f, 400.f);
	AddEnemy(AircraftType::kAvenger, 70.f, 600.f);
	 */


	//Add obstacles
	//450.f, 550.f, 650.f
	AddEnemy(AircraftType::kRaptor, 200.f, 450.f);
	AddEnemy(AircraftType::kRaptor, 250.f, 650.f);/*
	AddEnemy(AircraftType::kRaptor, 400.f, 550.f);
	AddEnemy(AircraftType::kRaptor, 500.f, 650.f);
	AddEnemy(AircraftType::kRaptor, 750.f, 450.f);
	AddEnemy(AircraftType::kAvenger, 750.f, 550.f);
	AddEnemy(AircraftType::kAvenger, 1000.f, 650.f);
	AddEnemy(AircraftType::kRaptor, 1100.f, 450.f);
	AddEnemy(AircraftType::kAvenger, 1250.f, 650.f);
	AddEnemy(AircraftType::kRaptor, 1400.f, 450.f);
	AddEnemy(AircraftType::kRaptor, 1500.f, 650.f);
	AddEnemy(AircraftType::kRaptor, 1750.f, 550.f);
	AddEnemy(AircraftType::kAvenger, 1800.f, 450.f);
	AddEnemy(AircraftType::kAvenger, 1900.f, 550.f);
	AddEnemy(AircraftType::kAvenger, 2000.f, 550.f);
	AddEnemy(AircraftType::kAvenger, 2200.f, 450.f);
	 

	//Add obstacles
	/*
		AddEnemy(ObstacleType::kTarSpill, 500.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 550.f, 650.f);
		AddEnemy(ObstacleType::kBarrier, 650.f, 550.f);
		AddEnemy(ObstacleType::kTarSpill, 800.f, 650.f);
		AddEnemy(ObstacleType::kAcidSpill, 850.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 900.f, 550.f);
		AddEnemy(ObstacleType::kTarSpill, 1000.f, 650.f);
		AddEnemy(ObstacleType::kBarrier, 1100.f, 450.f);
		AddEnemy(ObstacleType::kAcidSpill, 1150.f, 650.f);
		AddEnemy(ObstacleType::kTarSpill, 1400.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 1450.f, 650.f);
		AddEnemy(ObstacleType::kBarrier, 1750.f, 550.f);
		AddEnemy(ObstacleType::kAcidSpill, 1750.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 1950.f, 550.f);
		AddEnemy(ObstacleType::kBarrier, 2000.f, 550.f);
		
		AddEnemy(ObstacleType::kAcidSpill, 2250.f, 550.f);
		AddEnemy(ObstacleType::kTarSpill, 2250.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 2300.f, 650.f);
		AddEnemy(ObstacleType::kBarrier, 2400.f, 550.f);
		AddEnemy(ObstacleType::kTarSpill, 2450.f, 650.f);
		AddEnemy(ObstacleType::kAcidSpill, 2750.f, 450.f);
		AddEnemy(ObstacleType::kTarSpill, 2850.f, 450.f);
		AddEnemy(ObstacleType::kAcidSpill, 3000.f, 450.f);
		AddEnemy(ObstacleType::kAcidSpill, 3050.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 3200.f, 650.f);
		AddEnemy(ObstacleType::kTarSpill, 3350.f, 650.f);
		AddEnemy(ObstacleType::kBarrier, 3400.f, 450.f);
		AddEnemy(ObstacleType::kAcidSpill, 3500.f, 650.f);
		AddEnemy(ObstacleType::kTarSpill, 3550.f, 450.f);
		AddEnemy(ObstacleType::kBarrier, 3750.f, 550.f);
		
	 */

	//Sort according to x value so that lower enemies are checked first
	std::sort(m_enemy_spawn_points.begin(), m_enemy_spawn_points.end(), [](SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.m_x < rhs.m_x;
	});
}

void World::GuideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::kEnemyAircraft;
	enemyCollector.action = DerivedAction<Aircraft>([this](Aircraft& enemy, sf::Time)
	{
		if (!enemy.IsDestroyed())
			m_active_enemies.push_back(&enemy);
	});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::kAlliedProjectile;
	missileGuider.action = DerivedAction<Projectile>([this](Projectile& missile, sf::Time)
	{
		// Ignore unguided bullets
		if (!missile.IsGuided())
			return;

		float minDistance = std::numeric_limits<float>::max();
		Aircraft* closestEnemy = nullptr;

		// Find closest enemy
		for(Aircraft * enemy :  m_active_enemies)
		{
			float enemyDistance = Distance(missile, *enemy);

			if (enemyDistance < minDistance)
			{
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}

		if (closestEnemy)
			missile.GuideTowards(closestEnemy->GetWorldPosition());
	});

	// Push commands, reset active enemies
	m_command_queue.Push(enemyCollector);
	m_command_queue.Push(missileGuider);
	m_active_enemies.clear();
}

bool MatchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();
	std::cout << category1 << category2 << std::endl;
	if(type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if(type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for(SceneNode::Pair pair : collision_pairs)
	{
		if(MatchesCategories(pair, Category::Type::kPlayerAircraft, Category::Type::kEnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);
			//Collision
			player.Damage(enemy.GetHitPoints());
			enemy.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayerAircraft, Category::Type::kPickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Apply the pickup effect
			pickup.Apply(player);
			pickup.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayerAircraft, Category::Type::kEnemyProjectile) || MatchesCategories(pair, Category::Type::kEnemyAircraft, Category::Type::kAlliedProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Apply the projectile damage to the plane
			aircraft.Damage(projectile.GetDamage());
			projectile.Destroy();
		}

		/*
		    Collisions needed:
		    - Players & Barriers - should stop the player fully and destroy the barrier.
		    - Players & Acid spills - slows down the player a bit. Acid remains.
		    - Players & Tar spills - halves the players speed for as long as they are in the spill.
				Tar remains, and speed returns once player gets out of the tar.
			- Player & Player - knocks both players speed down by ~ 20% 
		 */


	}
}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kEnemyAircraft | Category::Type::kProjectile;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
	{
		//Does the object intersect with the battlefield
		if (!GetBattlefieldBounds().intersects(e.GetBoundingRect()))
		{
			std::cout << "Destroying the entity" << std::endl;
			e.Destroy();
		}
	});
	m_command_queue.Push(command);
}
