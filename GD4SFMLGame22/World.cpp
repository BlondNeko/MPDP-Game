#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

#include "Obstacle.hpp"
#include "ObstacleType.hpp"
#include "ParticleNode.hpp"
#include "ParticleType.hpp"
#include "Pickup.hpp"
#include "PostEffect.hpp"
#include "Projectile.hpp"
#include "SoundNode.hpp"
#include "Utility.hpp"

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds)
	: m_target(output_target)
	, m_camera(output_target.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_sounds(sounds)
	, m_scenegraph()
	, m_scene_layers()
	, m_world_bounds(0.f, 0.f,  5000, m_camera.getSize().x)
	, m_spawn_position(200.f, 550.f)//m_camera.getSize().x/2.f, m_world_bounds.height - m_camera.getSize().y /2.f + 50.f
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

	SpawnObstacles();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();

	UpdateSounds();
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
	m_textures.Load(Textures::kFinishLine, "Media/Textures/FinishLine.png");
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
	sf::Texture& city_texture = m_textures.Get(Textures::kCity1);
	//sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	city_texture.setRepeated(true);

	float view_height = m_camera.getSize().y;
	sf::IntRect texture_rect(m_world_bounds);
	texture_rect.height += static_cast<int>(view_height);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> city_sprite(new SpriteNode(city_texture, texture_rect));
	city_sprite->setPosition(m_world_bounds.left, m_world_bounds.top - view_height);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(city_sprite));

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

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));

	//Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::kNormal, m_textures, m_fonts));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);
	m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(leader));

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
	//Return camera bounds + wider width where obtacles should spawn
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;
	bounds.width += 300.f;

	return bounds;
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
	
	AddObstacle(ObstacleType::kAcidSpill, 2250.f, 550.f);
	AddObstacle(ObstacleType::kTarSpill, 2250.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 2300.f, 650.f);
	AddObstacle(ObstacleType::kBarrier, 2400.f, 550.f);
	AddObstacle(ObstacleType::kTarSpill, 2450.f, 650.f);
	AddObstacle(ObstacleType::kAcidSpill, 2750.f, 450.f);
    AddObstacle(ObstacleType::kTarSpill, 2850.f, 450.f);
    AddObstacle(ObstacleType::kAcidSpill, 3000.f, 450.f);
    AddObstacle(ObstacleType::kAcidSpill, 3050.f, 450.f);
    AddObstacle(ObstacleType::kBarrier, 3200.f, 650.f);
    AddObstacle(ObstacleType::kTarSpill, 3350.f, 650.f);
    AddObstacle(ObstacleType::kBarrier, 3400.f, 450.f);
    AddObstacle(ObstacleType::kAcidSpill, 3500.f, 650.f);
    AddObstacle(ObstacleType::kTarSpill, 3550.f, 450.f);
    AddObstacle(ObstacleType::kBarrier, 3750.f, 550.f);

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
			player.PlayLocalSound(m_command_queue, SoundEffect::kCollectPickup);
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
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& obstacle = static_cast<Obstacle&>(*pair.second);
			//Apply the projectile damage to the plane
			aircraft.DecreaseSpeed(obstacle.GetSlowdown());
			//obstacle.Destroy();
		}
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
			e.Destroy();
		}
	});
	m_command_queue.Push(command);
}

void World::UpdateSounds()
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_player_aircraft->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}
