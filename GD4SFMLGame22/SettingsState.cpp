#include "SettingsState.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "StateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, m_gui_container()
{
	m_background_sprite.setTexture(context.textures->Get(Textures::kTitleScreen));

	//sf::Text title;

	//title.setString("Controls");
	//title.setPosition(80.f, 300.f);

	// Build key binding buttons and labels
	//AddButtonLabel(PlayerAction::kMoveLeft, 300.f, "Move Left", context);
	//AddButtonLabel(PlayerAction::kMoveRight, 350.f, "Move Right", context);
	//AddButtonLabel(PlayerAction::kMoveUp, 400.f, "Move Up", context);
	//AddButtonLabel(PlayerAction::kMoveDown, 450.f, "Move Down", context);
	//AddButtonLabel(PlayerAction::kBoost, 500.f, "Use Boost", context);
	//AddButtonLabel(PlayerAction::kLaunchMissile, 550.f, "Missile", context);

	//UpdateLabels();

	auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(80.f, 620.f);
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SettingsState::RequestStackPop, this));

	auto volume_up_button = std::make_shared<GUI::Button>(context);
	volume_up_button->setPosition(80.f, 200.f);
	volume_up_button->SetText("Volume + ");
	//volume_up_button->SetCallback(std::bind(&SettingsState::VolumeControl(10, context), this));
	//volume_up_button->SetCallback(SettingsState::VolumeControl(-10, context));
	//volume_up_button->SetCallback(std::bind(&MusicPlayer::SetVolume(0.f), this));

	auto volume_down_button = std::make_shared<GUI::Button>(context);
	volume_down_button->setPosition(300.f, 200.f);
	volume_down_button->SetText("Volume - ");
	//volume_down_button->SetCallback(std::bind(&SettingsState::VolumeControl(-10, context), this));

	m_gui_container.Pack(back_button);
	m_gui_container.Pack(volume_up_button);
	m_gui_container.Pack(volume_down_button);

}

void SettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;

	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SettingsState::Update(sf::Time)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	//bool isKeyBinding = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	/*for (std::size_t action = 0; action < static_cast<int>(PlayerAction::kActionCount); ++action)
	{
		if (m_binding_buttons[action]->IsActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				GetContext().player1->AssignKey(static_cast<PlayerAction>(action), event.key.code);
				m_binding_buttons[action]->Deactivate();
			}
			break;
		}
	}*/

	// If pressed button changed key bindings, update labels; otherwise consider other buttons in container
	//if (isKeyBinding)
	//	UpdateLabels();
	//else
		m_gui_container.HandleEvent(event);

	return false;
}

void SettingsState::UpdateLabels()
{
	Player& player = *GetContext().player1;

	for (std::size_t i = 0; i < static_cast<int>(PlayerAction::kActionCount); ++i)
	{
		sf::Keyboard::Key key = player.GetAssignedKey(static_cast<PlayerAction>(i));
		m_binding_labels[i]->SetText(Utility::toString(key));
	}

	PlayerB& playerb = *GetContext().player2;

	for (std::size_t i = 0; i < static_cast<int>(PlayerAction::kActionCount); ++i)
	{
		sf::Keyboard::Key key = playerb.GetAssignedKey(static_cast<PlayerAction>(i));
		m_binding_labels[i]->SetText(Utility::toString(key));
	}

}

void SettingsState::VolumeControl(float vol, Context context)
{
	context.music->SetVolume(vol);
}


void SettingsState::AddButtonLabel(PlayerAction action, float y, const std::string& text, Context context)
{
	m_binding_buttons[static_cast<int>(action)] = std::make_shared<GUI::Button>(context);
	m_binding_buttons[static_cast<int>(action)]->setPosition(80.f, y);
	m_binding_buttons[static_cast<int>(action)]->SetText(text);
	m_binding_buttons[static_cast<int>(action)]->SetToggle(true);

	m_binding_labels[static_cast<int>(action)] = std::make_shared<GUI::Label>("", *context.fonts);
	m_binding_labels[static_cast<int>(action)]->setPosition(300.f, y + 15.f);

	m_gui_container.Pack(m_binding_buttons[static_cast<int>(action)]);
	m_gui_container.Pack(m_binding_labels[static_cast<int>(action)]);
}
