#include "GameState.hpp"

#include <iostream>

#include "Player.hpp"
#include "PlayerB.hpp"

GameState::GameState(StateStack& stack, Context context)
: State(stack, context)
, m_world(*context.window, *context.fonts, *context.sounds)
, m_player1(*context.player1)
, m_player2(*context.player2)
{
	// Play game theme
	context.music->Play(MusicThemes::kMissionTheme);
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{
	m_world.Update(dt);
	if (!m_world.HasAlivePlayer())
	{
		m_player1.SetMissionStatus(MissionStatus::kMissionFailure);
		m_player2.SetMissionStatus(MissionStatus::kMissionFailure);
		RequestStackPush(StateID::kGameOver);
	}
	else if (m_world.HasPlayerReachedEnd())
	{
		m_player1.SetMissionStatus(MissionStatus::kMissionSuccess);
		m_player2.SetMissionStatus(MissionStatus::kMissionSuccess);
		RequestStackPush(StateID::kGameOver);
	}
	CommandQueue& commands = m_world.getCommandQueue();
	m_player1.HandleRealtimeInput(commands);
	m_player2.HandleRealtimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.getCommandQueue();
	m_player1.HandleEvent(event, commands);
	//Escape should bring up the Pause Menu
	if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}

	m_player2.HandleEvent(event, commands);
	//Escape should bring up the Pause Menu
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}
