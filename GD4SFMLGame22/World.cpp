#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

#include "ParticleNode.hpp"
#include "ParticleType.hpp"
#include "Pickup.hpp"
#include "PostEffect.hpp"
#include "Projectile.hpp"
#include "Utility.hpp"

World::World(sf::RenderTarget& output_target, FontHolder& font)
	: m_target(output_target)
	, m_camera(output_target.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_scenegraph()
	, m_scene_layers()
	, m_world_bounds(0.f, 0.f,  5000, m_camera.getSize().x)
	, m_spawn_position(m_camera.getSize().x/2.f, m_world_bounds.height - m_camera.getSize().y /2.f + 50.f)
	, m_scrollspeed(-50.f)
	, m_player_aircraft(nullptr)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);

	LoadTextures();
	BuildScene();
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	m_camera.move(-m_scrollspeed * dt.asSeconds(), 0);

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

	//SpawnEnemies();
	SpawnObstacles();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();
}

void World::Draw()
{
	if(PostEffect::IsSupported())
	{
		m_scene_texture.clear();
		m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_scenegraph);
		m_scene_texture.display();
		m_bloom_effect.Apply(m_scene_texture, m_target);
	}
	else
	{
		m_target.setView(m_camera);
		m_target.draw(m_scenegraph);
	}

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
	m_textures.Load(Textures::kSpriteSheet, "Media/Sprites/SpriteSheet.png");

	m_textures.Load(Textures::kFinish1, "Media/Sprites/FinishLine1.png");
	m_textures.Load(Textures::kFinish2, "Media/Sprites/FinishLine2.png");
	m_textures.Load(Textures::kCity1, "Media/Sprites/Background.png");
	m_textures.Load(Textures::kCity2, "Media/Sprites/Background2.png");

	m_textures.Load(Textures::kEntities, "Media/Textures/Entities.png");
	m_textures.Load(Textures::kJungle, "Media/Textures/Jungle.png");
	m_textures.Load(Textures::kExplosion, "Media/Textures/Explosion.png");
	m_textures.Load(Textures::kParticle, "Media/Textures/Particle.png");
	m_textures.Load(Textures::kFinishLine, "Media/Sprites/FinishLine2.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category = (i == static_cast<int>(Layers::kLowerAir)) ? Category::Type::kScene : Category::Type::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& jungle_texture = m_textures.Get(Textures::kCity1);
	//sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	jungle_texture.setRepeated(true);

	float view_height = m_camera.getSize().y;
	sf::IntRect texture_rect(m_world_bounds);
	texture_rect.height += static_cast<int>(view_height);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> jungle_sprite(new SpriteNode(jungle_texture, texture_rect));
	jungle_sprite->setPosition(m_world_bounds.left, m_world_bounds.top - view_height);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(jungle_sprite));

	// Add the finish line to the scene
	sf::Texture& finish_texture = m_textures.Get(Textures::kFinishLine);
	std::unique_ptr<SpriteNode> finish_sprite(new SpriteNode(finish_texture));
	finish_sprite->setPosition(0.f, -76.f);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(finish_sprite));

	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(ParticleType::kSmoke, m_textures));
	m_scene_layers[static_cast<int>(Layers::kLowerAir)]->AttachChild(std::move(smokeNode));

	// Add propellant particle node to the scene
	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(ParticleType::kPropellant, m_textures));
	m_scene_layers[static_cast<int>(Layers::kLowerAir)]->AttachChild(std::move(propellantNode));

	//Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::kEagle, m_textures, m_fonts));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);
	m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(leader));

	// //Add two escorts
	// std::unique_ptr<Aircraft> leftEscort(new Aircraft(AircraftType::kRaptor, m_textures, m_fonts));
	// leftEscort->setPosition(-80.f, 50.f);
	// m_player_aircraft->AttachChild(std::move(leftEscort));
	//
	// std::unique_ptr<Aircraft> rightEscort(new Aircraft(AircraftType::kRaptor, m_textures, m_fonts));
	// rightEscort->setPosition(80.f, 50.f);
	// m_player_aircraft->AttachChild(std::move(rightEscort));

	//AddEnemies();
	AddObstacles();
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
	const float barrier_distance = view_bounds.top - 310.f;
	sf::Vector2f position = m_player_aircraft->getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance - barrier_distance);
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
}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

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
		enemy->setRotation(180.f);
		m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(enemy));

		m_enemy_spawn_points.pop_back();
		
	}
}

void World::AddEnemy(AircraftType type, float relX, float relY)
{
	SpawnPoint spawn(type, m_spawn_position.x + relX, m_spawn_position.y - relY);
	m_enemy_spawn_points.emplace_back(spawn);
}

void World::AddEnemies()
{
	//Add all enemies
	AddEnemy(AircraftType::kRaptor, 0.f, 500.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 1000.f);
	AddEnemy(AircraftType::kRaptor, +100.f, 1150.f);
	AddEnemy(AircraftType::kRaptor, -100.f, 1150.f);
	AddEnemy(AircraftType::kAvenger, 70.f, 1500.f);
	AddEnemy(AircraftType::kAvenger, -70.f, 1500.f);
	AddEnemy(AircraftType::kAvenger, -70.f, 1710.f);
	AddEnemy(AircraftType::kAvenger, 70.f, 1700.f);
	AddEnemy(AircraftType::kAvenger, 30.f, 1850.f);
	AddEnemy(AircraftType::kRaptor, 300.f, 2200.f);
	AddEnemy(AircraftType::kRaptor, -300.f, 2200.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 2200.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 2500.f);
	AddEnemy(AircraftType::kAvenger, -300.f, 2700.f);
	AddEnemy(AircraftType::kAvenger, -300.f, 2700.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 3000.f);
	AddEnemy(AircraftType::kRaptor, 250.f, 3250.f);
	AddEnemy(AircraftType::kRaptor, -250.f, 3250.f);
	AddEnemy(AircraftType::kAvenger, 0.f, 3500.f);
	AddEnemy(AircraftType::kAvenger, 0.f, 3700.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 3800.f);
	AddEnemy(AircraftType::kAvenger, 0.f, 4000.f);
	AddEnemy(AircraftType::kAvenger, -200.f, 4200.f);
	AddEnemy(AircraftType::kRaptor, 200.f, 4200.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 4400.f);

	//Sort according to y value so that lower enemies are checked first
	std::sort(m_enemy_spawn_points.begin(), m_enemy_spawn_points.end(), [](SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.m_y < rhs.m_y;
	});
}

void World::SpawnObstacles()
{
	//Spawn an obstacle when they are relevant - they are relevant when they enter the battlefield bounds
	while (!m_obstacle_spawn_points.empty() && m_obstacle_spawn_points.back().m_y > GetBattlefieldBounds().top)
	{
		ObstacleSpawnPoint spawn = m_obstacle_spawn_points.back();
		std::unique_ptr<Obstacle> obs(new Obstacle(spawn.m_type, m_textures));
		obs->setPosition(spawn.m_x, spawn.m_y);
		m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(obs));
		m_obstacle_spawn_points.pop_back();
	}
}

void World::AddObstacle(ObstacleType type, float relX, float relY)
{
	ObstacleSpawnPoint spawn(type, relX, relY);
	m_obstacle_spawn_points.emplace_back(spawn);
}

void World::AddObstacles()
{
	//Add obstacles
	//450.f, 550.f, 650.f ( range of the road)
	AddObstacle(ObstacleType::kBarrier, 200.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 250.f, 650.f);

	AddObstacle(ObstacleType::kTarSpill, 500.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 550.f, 650.f);
	AddObstacle(ObstacleType::kBarrier, 650.f, 550.f);
	AddObstacle(ObstacleType::kTarSpill, 800.f, 650.f);
	AddObstacle(ObstacleType::kAcidSpill, 850.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 900.f, 550.f);
	AddObstacle(ObstacleType::kTarSpill, 1000.f, 650.f);
	AddObstacle(ObstacleType::kBarrier, 1100.f, 450.f);
	AddObstacle(ObstacleType::kAcidSpill, 1150.f, 650.f);
	AddObstacle(ObstacleType::kTarSpill, 1400.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 1450.f, 650.f);
	AddObstacle(ObstacleType::kBarrier, 1750.f, 550.f);
	AddObstacle(ObstacleType::kAcidSpill, 1750.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 1950.f, 550.f);
	AddObstacle(ObstacleType::kBarrier, 2000.f, 550.f);
	/*
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

		else if (MatchesCategories(pair, Category::Type::kPlayerAircraft, Category::Type::kObstacle))
		{
			auto& obstacle = static_cast<Obstacle&>(*pair.second);
			obstacle.Destroy();
		}


	}
}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kEnemyAircraft | Category::Type::kProjectile | Category::Type::kObstacle;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
	{
		//Does the object intersect with the battlefield
		if (!GetBattlefieldBounds().intersects(e.GetBoundingRect()))
		{
			e.Destroy();
		}
	});
	m_command_queue.Push(command);
}
