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
	, m_spawn_position_1(200.f, 450.f)
	, m_spawn_position_2(200.f, 650.f)
	, m_scrollspeed(-50.f)
	, m_player1_aircraft(nullptr)
	, m_player2_aircraft(nullptr)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);

	LoadTextures();
	BuildScene();
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	m_camera.move(-m_scrollspeed * dt.asSeconds(), 0);

	m_player1_aircraft->SetVelocity(0.f, 0.f);
	m_player2_aircraft->SetVelocity(0.f, 0.f);
	DestroyEntitiesOutsideView();

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
	SpawnPickups();

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
	return !m_player1_aircraft->IsMarkedForRemoval() || !m_player2_aircraft->IsMarkedForRemoval();
}

bool World::HasPlayerReachedEnd() const
{
	return !m_world_bounds.contains(m_player1_aircraft->getPosition()) || !m_world_bounds.contains(m_player2_aircraft->getPosition());
}

bool World::HasPlayer1ReachedEnd() const
{
	return !m_world_bounds.contains(m_player1_aircraft->getPosition());
}

bool World::HasPlayer2ReachedEnd() const
{
	return !m_world_bounds.contains(m_player2_aircraft->getPosition());
}

void World::LoadTextures()
{
	m_textures.Load(Textures::kSpriteSheet, "Media/Sprites/SpriteSheet2.png");

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

	//Add player 1's aircraft
	std::unique_ptr<Aircraft> player1(new Aircraft(AircraftType::kPlayer1, m_textures, m_fonts, true));
	m_player1_aircraft = player1.get();
	m_player1_aircraft->setPosition(m_spawn_position_1);
	m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(player1));

	//Add player 2's aircraft
	std::unique_ptr<Aircraft> player2(new Aircraft(AircraftType::kPlayer2, m_textures, m_fonts, false));
	m_player2_aircraft = player2.get();
	m_player2_aircraft->setPosition(m_spawn_position_2);
	m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(player2));

	AddObstacles();
	AddPickups();
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
	const float barrier_distance = view_bounds.top - 325.f;
	sf::Vector2f position = m_player1_aircraft->getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance - barrier_distance);
	position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);
	m_player1_aircraft->setPosition(position);

	position = m_player2_aircraft->getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance - barrier_distance);
	position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);
	m_player2_aircraft->setPosition(position);

}

void World::AdaptPlayerVelocity()
{
	sf::Vector2f velocity = m_player1_aircraft->GetVelocity();
	//if moving diagonally then reduce velocity
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		m_player1_aircraft->SetVelocity(velocity / std::sqrt(2.f));
	}
	//Add scrolling velocity
	m_player1_aircraft->Accelerate(m_scrollspeed, 0.f);

	velocity = m_player2_aircraft->GetVelocity();
	//if moving diagonally then reduce velocity
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		m_player2_aircraft->SetVelocity(velocity / std::sqrt(2.f));
	}
	//Add scrolling velocity
	m_player2_aircraft->Accelerate(m_scrollspeed, 0.f);
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

	AddObstacle(ObstacleType::kTarSpill, 500.f, 450.f);

	AddObstacle(ObstacleType::kTarSpill, 800.f, 650.f);
	AddObstacle(ObstacleType::kAcidSpill, 850.f, 450.f);
	AddObstacle(ObstacleType::kBarrier, 900.f, 550.f);

	AddObstacle(ObstacleType::kBarrier, 1450.f, 650.f);
	AddObstacle(ObstacleType::kBarrier, 1950.f, 550.f);
	AddObstacle(ObstacleType::kBarrier, 2000.f, 550.f);
	
	AddObstacle(ObstacleType::kAcidSpill, 2250.f, 550.f);
	AddObstacle(ObstacleType::kTarSpill, 2250.f, 450.f);

    AddObstacle(ObstacleType::kTarSpill, 2850.f, 450.f);
    AddObstacle(ObstacleType::kAcidSpill, 3050.f, 450.f);
    AddObstacle(ObstacleType::kTarSpill, 3550.f, 450.f);
    AddObstacle(ObstacleType::kBarrier, 3750.f, 550.f);

	AddObstacle(ObstacleType::kAcidSpill, 3750.f, 450.f);
	AddObstacle(ObstacleType::kTarSpill, 4000.f, 450.f);

	AddObstacle(ObstacleType::kBarrier, 4000.f, 500.f);
	AddObstacle(ObstacleType::kAcidSpill, 4250.f, 650.f);
	AddObstacle(ObstacleType::kTarSpill, 4250.f, 450.f);

	AddObstacle(ObstacleType::kTarSpill, 4550.f, 500.f);
	AddObstacle(ObstacleType::kAcidSpill, 4850.f, 550.f);
}

void World::SpawnPickups()
{
	//Spawn an obstacle when they are relevant - they are relevant when they enter the battlefield bounds
	while (!m_pickup_spawn_points.empty() && m_pickup_spawn_points.back().m_y > GetBattlefieldBounds().top)
	{
		PickupSpawnPoint spawn = m_pickup_spawn_points.back();
		std::unique_ptr<Pickup> pickup(new Pickup(spawn.m_type, m_textures));
		pickup->setPosition(spawn.m_x, spawn.m_y);
		m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(pickup));
		m_pickup_spawn_points.pop_back();
	}
}

void World::AddPickup(PickupType type, float relX, float relY)
{
	PickupSpawnPoint spawn(type, relX, relY);
	m_pickup_spawn_points.emplace_back(spawn);
}

void World::AddPickups()
{
	//Add obstacles
	//450.f, 550.f, 650.f ( range of the road)
	AddPickup(PickupType::kBoost, 500.f, 500.f);
	AddPickup(PickupType::kBoost, 1500.f, 600.f);
	AddPickup(PickupType::kBoost, 3000.f, 500.f);
	AddPickup(PickupType::kBoost, 3500.f, 600.f);
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
		
		/*
		 * if(MatchesCategories(pair, Category::Type::kPlayer1, Category::kPlayer2))
		{
			auto& player1 = static_cast<Aircraft&>(*pair.first);
			auto& player2 = static_cast<Aircraft&>(*pair.second);
			//Collision
			player1.DecreaseSpeed(50.f);
			player2.DecreaseSpeed(50.f);
		}
		 */


		if (MatchesCategories(pair, Category::Type::kPlayer1, Category::kPickup) || MatchesCategories(pair, Category::Type::kPlayer2, Category::kPickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Apply the pickup effect
			pickup.Apply(player);
			pickup.Destroy();
			player.PlayLocalSound(m_command_queue, SoundEffect::kCollectPickup);
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1, Category::Type::kObstacle) || MatchesCategories(pair, Category::Type::kPlayer2, Category::Type::kObstacle))
		{
			auto& bike = static_cast<Aircraft&>(*pair.first);
			auto& obstacle = static_cast<Obstacle&>(*pair.second);

			//Apply the slowdown to the plane
			bike.DecreaseSpeed(obstacle.GetSlowdown());
			obstacle.Destroy();
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
	m_sounds.SetListenerPosition(m_player1_aircraft->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}
