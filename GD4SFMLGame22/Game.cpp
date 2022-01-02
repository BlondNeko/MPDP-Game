#include "Game.hpp"

const sf::Time Game::kTimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	:m_window(sf::VideoMode(640, 480), 
"Starting"),
	m_world(m_window),
	m_font(),
	m_statistics_text(),
	m_statistics_updatetime(),
	m_statistics_numframes(0)
{
	m_font.loadFromFile("Media/Fonts/Sansation.ttf");
	m_statistics_text.setFont(m_font);
	m_statistics_text.setPosition(5.f, 5.f);
	m_statistics_text.setCharacterSize(10);
}

void Game::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while(m_window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		time_since_last_update += elapsedTime;

		while (time_since_last_update > kTimePerFrame)
		{
			time_since_last_update -= kTimePerFrame;
			ProcessInput();
			Update(kTimePerFrame);
		}
		UpdateStatistics(elapsedTime);
		Render();
	}
}

void Game::ProcessInput()
{
	CommandQueue& commands = m_world.getCommandQueue();

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_player.HandleEvent(event, commands);
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
	}
	m_player.HandleRealtimeInput(commands);
}

void Game::Update(sf::Time delta_time)
{
	m_world.Update(delta_time);
}

sf::Vector2f Game::Normalise(const sf::Vector2f input_vector)
{
	float length = Length(input_vector);
	if (length > 0)
	{
		return input_vector / length;
	}
	return input_vector;

}

float Game::Length(const sf::Vector2f& input_vector)
{
	return sqrt(pow(input_vector.x, 2) + pow(input_vector.y, 2));
}

void Game::Render()
{
	m_window.clear();
	m_world.Draw();

	m_window.setView(m_window.getDefaultView());
	m_window.draw(m_statistics_text);
	m_window.display();
}

void Game::HandlePlayerInput(sf::Keyboard::Key key, bool is_pressed)
{
}

void Game::UpdateStatistics(sf::Time elapsed_time)
{
	m_statistics_updatetime += elapsed_time;
	m_statistics_numframes += 1;

	if (m_statistics_updatetime >= sf::seconds(1.0f))
	{
		m_statistics_text.setString(
			"Frames / Second = " + std::to_string(m_statistics_numframes) + "\n" +
			"Time / Update = " + std::to_string(m_statistics_updatetime.asMicroseconds() / m_statistics_numframes) + "us");

		m_statistics_updatetime -= sf::seconds(1.0f);
		m_statistics_numframes = 0;
	}
}





