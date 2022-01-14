#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "Layers.hpp"
#include "AircraftType.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <array>
#include <SFML/Graphics/RenderWindow.hpp>

#include "BloomEffect.hpp"
#include "CommandQueue.hpp"
#include "Obstacle.hpp"
#include "ObstacleType.hpp"
#include "PickupType.hpp"
#include "SoundPlayer.hpp"


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds);
	void Update(sf::Time dt);
	void Draw();
	CommandQueue& getCommandQueue();
	bool HasAlivePlayer() const;
	bool HasPlayerReachedEnd() const;
	bool HasPlayer1ReachedEnd() const;
	bool HasPlayer2ReachedEnd() const;

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattlefieldBounds() const;

	void SpawnObstacles();
	void AddObstacle(ObstacleType type, float relX, float relY);
	void AddObstacles();

	void SpawnPickups();
	void AddPickup(PickupType type, float relX, float relY);
	void AddPickups();

	void HandleCollisions();
	void DestroyEntitiesOutsideView();
	void UpdateSounds();

private:

	struct ObstacleSpawnPoint
	{
		ObstacleSpawnPoint(ObstacleType type, float x, float y) : m_type(type), m_x(x), m_y(y)
		{

		}
		ObstacleType m_type;
		float m_x;
		float m_y;
	};

	struct PickupSpawnPoint
	{
		PickupSpawnPoint(PickupType type, float x, float y) : m_type(type), m_x(x), m_y(y)
		{

		}
		PickupType m_type;
		float m_x;
		float m_y;
	};
	

private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position_1;
	sf::Vector2f m_spawn_position_2;
	float m_scrollspeed;
	Aircraft* m_player1_aircraft;
	Aircraft* m_player2_aircraft;

	std::vector<ObstacleSpawnPoint> m_obstacle_spawn_points;
	std::vector<PickupSpawnPoint> m_pickup_spawn_points;

	BloomEffect m_bloom_effect;
};

