#pragma once
#include <SFML/Graphics.hpp>

#include "Player.hpp"
#include "ResourceHolder.hpp"
#include "World.hpp"

class Game
{
public:
	Game();
	void Run();

private:
	void HandlePlayerInput(sf::Keyboard::Key key, bool is_pressed);
	void ProcessInput();
	void Update(sf::Time delta_time);
	float Length(const sf::Vector2f& vector2);
	sf::Vector2f Normalise(const sf::Vector2f input_vector);
	void Render();

	void UpdateStatistics(sf::Time elapsed_time);

private:
	sf::RenderWindow m_window;
	World m_world;
	Player m_player;
	
	sf::Font m_font;
	sf::Text m_statistics_text;
	sf::Time m_statistics_updatetime;

	std::size_t m_statistics_numframes;
	static const sf::Time kTimePerFrame;
};
