#include "GameBoard.h"

#include "GameEngine/GameEngineMain.h"

#include "Game/Components/PlayerMovementComponent.h"
#include "Game/Components/PetMovementComponent.h"
#include "GameEngine/EntitySystem/Components/SpriteRenderComponent.h"
#include <string>
#include "GameEngine\EntitySystem\Components\AnimationComponent.h"
#include "GameEngine/EntitySystem/Components/CollidablePhysicsComponent.h"

#include "GameEngine/EntitySystem/Components/CollidableComponent.h"
#include "GameEngine/EntitySystem/Components/TextRenderComponent.h"

#include <iostream>
#include <ctime>

using namespace Game;
sf::Event event;

GameBoard::GameBoard() : boardx(1800.f), boardy(900.f), pastscreen(1), screen(1), agendaHover(false),
						 init(false), px(500), py(500), hapwidth(208.f), haplength(18.f), pastHappiness(1.f),
						m_player(nullptr), pet(nullptr), check{ false }, checklist{ nullptr }, happinessTime(30.f)
{
	CreateBackground();

	// set tasks
	taskLength = 3;
	for (int i = 0; i < taskLength; ++i) {
		taskList.emplace_back("Finish probability homework");
	}
}


GameBoard::~GameBoard()
{
	
}

void GameBoard::CreateBackground() {
	GameEngine::GameEngineMain::GetInstance()->RemoveAllEntities();

	background = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(background);

	background->SetPos(sf::Vector2f(900, 450.f));
	background->SetSize(sf::Vector2f(boardx, boardy));

	GameEngine::SpriteRenderComponent* render = background->AddComponent<GameEngine::SpriteRenderComponent>();
	if (screen == 1) {
		render->SetTexture(GameEngine::eTexture::BackgroundHome);
		CreateAgenda(false);
		for (int i = 0; i < 4; ++i) {
			SpawnBackgroundObstacles(100 + i);
		}
		//CreateObstacle();
		if (!init) {
			CreatePlayer(px, py);
			init = true;
		}
		else {
			CreatePlayer(0.f, py);
		} 
		CreatePet();
		CreateHappinessBar();
		pastscreen = 1;
	}
	else if (screen == 2) {
		render->SetTexture(GameEngine::eTexture::BackgroundHall);
		for (int i = 0; i < 3; ++i) {
			SpawnBackgroundObstacles(200 + i);
		}
		if (pastscreen == 1) {
			CreatePlayer(boardx, py);
		}
		else if (pastscreen == 3) {
			CreatePlayer(880.f, 500.f);
		}
		CreateHappinessBar();
		pastscreen = 2;
	}
	else if (screen == 3) {
		render->SetTexture(GameEngine::eTexture::BackgroundPet);

		CreateHappinessBar();
		CreateText("Coins: " + std::to_string(m_player->GetScore()), 175.f, 75.f);
		CreateText("Feed Pet (10C)", 175.f, 275.f);
		CreateFoodButton(); 
		CreateCoin();
		pastscreen = 3;
	}
	render->SetFillColor(sf::Color::Transparent);
	render->SetZLevel(-1);
}
void GameBoard::CreateAgenda(bool highlight){
	agenda = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(agenda);

	agenda->SetPos(sf::Vector2f(900.0f, 430.0f));
	agenda->SetSize(sf::Vector2f(1600.0f, 800.0f));

	GameEngine::SpriteRenderComponent* render = agenda->AddComponent<GameEngine::SpriteRenderComponent>();

	render->SetFillColor(sf::Color::Transparent);

	render->SetZLevel(1);
	if (highlight) {
		render->SetTexture(GameEngine::eTexture::AgendaHover);
	}
	else {
		render->SetTexture(GameEngine::eTexture::Agenda);
	}
}
void GameBoard::SpawnBackgroundObstacles(int id) {
	GameEngine::Entity* obst = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(obst);

	if (id == 200) {
		obst->SetPos(sf::Vector2f(0, 0.f));
		obst->SetSize(sf::Vector2f(3600.0f, 900.0f));
	}
	else if (id == 201) {
		obst->SetPos(sf::Vector2f(0, 1000.f));
		obst->SetSize(sf::Vector2f(3600.0f, 100.0f));
	}
	else if (id == 202) {
		obst->SetPos(sf::Vector2f(0, 0.f));
		obst->SetSize(sf::Vector2f(1.0f, 1800.0f));
	}else if (id == 100){
		obst->SetPos(sf::Vector2f(0, 0.f));
		obst->SetSize(sf::Vector2f(450.0f, 1000.0f));
	}
	else if (id == 101) {
		obst->SetPos(sf::Vector2f(450.f, 0.f));
		obst->SetSize(sf::Vector2f(2300.0f, 550.0f));
	}
	else if (id == 102) {
		obst->SetPos(sf::Vector2f(1780.f, 0.f));
		obst->SetSize(sf::Vector2f(450.0f, 1000.0f));
	}
	else if (id == 103) {
		obst->SetPos(sf::Vector2f(0.f, 960.f));
		obst->SetSize(sf::Vector2f(4000.0f, 10.0f));
	}

	// Render
	GameEngine::SpriteRenderComponent* render = obst->AddComponent<GameEngine::SpriteRenderComponent>();

	render->SetFillColor(sf::Color::Transparent);
	render->SetTexture(GameEngine::eTexture::HallObstacle);

	//comment out to show the obstacle
	render->SetZLevel(-8);

	obst->AddComponent<GameEngine::CollidableComponent>();
	//roomObstacles.push_back(obst);
}
void GameBoard::UpdateMousePosition() {
	//GameEngine::Entity* window = new GameEngine::Entity();
	sf::RenderWindow* window = GameEngine::GameEngineMain::GetInstance()->GetRenderWindow();
	sf::Event event;

	if (screen == 1) {
		// while there are pending events...
		while (window->pollEvent(event))
		{

			if ((sf::Mouse::getPosition().x > 700 && sf::Mouse::getPosition().x < 1200) && (sf::Mouse::getPosition().y > 160 && sf::Mouse::getPosition().y < 400)) {
				GameEngine::GameEngineMain::GetInstance()->RemoveEntity(agenda);
				agenda = nullptr;
				CreateAgenda(true);
				agendaHover = true;
			}
			else if (agendaHover) {
				GameEngine::GameEngineMain::GetInstance()->RemoveEntity(agenda);
				agenda = nullptr;
				CreateAgenda(false);
				agendaHover = false;
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			std::cout << "the right button was pressed" << std::endl;
			std::cout << "mouse x: " << sf::Mouse::getPosition().x << std::endl;
			std::cout << "mouse y: " << sf::Mouse::getPosition().y << std::endl;
			if (agendaHover) {
				if (!check) {
					CreateChecklist();
					for (int i = 0; i < taskLength; ++i) {
						CreateTasks(i);
					}
					check = true;
				}
			}

		}

		if (check && sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
			
				GameEngine::GameEngineMain::GetInstance()->RemoveEntity(checklist);
				checklist = nullptr;
				check = false;
				for (std::vector<GameEngine::Entity*>::iterator it = tasks.begin(); it != tasks.end();) {
					GameEngine::Entity* task = (*it);
					GameEngine::GameEngineMain::GetInstance()->RemoveEntity(task);
					it = tasks.erase(it);
				}
		}
	}
}
void GameBoard::UpdatePosition() {
	px = m_player->GetPos().x;
	py = m_player->GetPos().y;

	if (screen == 1) {
		if (px < 0.f) {
			screen = 2;
			CreateBackground();
		}
	}
	else if (screen == 2) {
		
		if (px > boardx) {
			screen = 1;
			CreateBackground();
		} 
		if (px > 800.f && px < 1000.f && py < 600.f && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			screen = 3;
			CreateBackground();
		}
	}
	else if (screen == 3) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
			screen = 2;
			CreateBackground();
		}
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				printf((char*)event.mouseButton.x);
				printf((char*)event.mouseButton.y);
			}
		}
	}
	
}


void GameBoard::CreateChecklist() {
	checklist = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(checklist);

	checklist->SetPos(sf::Vector2f(400.0f, 150.0f));
	checklist->SetSize(sf::Vector2f(275.0f, 300.0f));

	// Render

	GameEngine::SpriteRenderComponent* render = static_cast<GameEngine::SpriteRenderComponent*>(checklist->AddComponent<GameEngine::SpriteRenderComponent>());

	render->SetTexture(GameEngine::eTexture::Checklist);
	render->SetFillColor(sf::Color::Transparent);
	render->SetZLevel(101);

}

void GameBoard::CreateTasks(int id) {
	GameEngine::Entity* task = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(task);

	task->SetPos(sf::Vector2f(300.f, 100.f + id * 40.f));
	GameEngine::TextRenderComponent* render = task->AddComponent<GameEngine::TextRenderComponent>();
	render->SetString(taskList[id]);
	render->SetFont("joystix.ttf");
	render->SetColor(sf::Color::Black);
	render->SetFillColor(sf::Color::Transparent);
	render->SetCharacterSizePixels(20);
	render->SetZLevel(102);

	tasks.push_back(task);
}

void GameBoard::CreatePlayer(float x, float y) {
	m_player = new PlayerEntity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(m_player);

	m_player->SetPos(sf::Vector2f(x, y));
	m_player->SetSize(sf::Vector2f(100.0f, 150.0f));

	//Render
	playerRender = static_cast<GameEngine::SpriteRenderComponent*>(m_player->AddComponent<GameEngine::SpriteRenderComponent>());

	playerRender->SetFillColor(sf::Color::Transparent);
	playerRender->SetTexture(GameEngine::eTexture::Player);
	playerRender->SetZLevel(100); // Player in front of everything

	//Movement
	m_player->AddComponent<Game::PlayerMovementComponent>();  // <-- Added the movement component to the player
	m_player->AddComponent<GameEngine::AnimationComponent>();
	m_player->AddComponent<GameEngine::CollidablePhysicsComponent>();
}

void GameBoard::CreatePet() {
	pet = new PetEntity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(pet);

	pet->SetPos(sf::Vector2f(700.0f, 600.0f));
	pet->SetSize(sf::Vector2f(100.0f, 100.0f));

	petRender = static_cast<GameEngine::SpriteRenderComponent*>(pet->AddComponent<GameEngine::SpriteRenderComponent>());

	petRender->SetFillColor(sf::Color::Transparent);
	petRender->SetTexture(GameEngine::eTexture::Dog);
	petRender->SetZLevel(99);

	pet->AddComponent<GameEngine::AnimationComponent>();
	Game::PetMovementComponent* temp =  pet->AddComponent<Game::PetMovementComponent>();

	temp->SetPlayerEntity(m_player);
	temp->GetHappiness(pet->GetHappiness());
}



void GameBoard::CreateObstacle() {
	obstacle = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(obstacle);

	obstacle->SetPos(sf::Vector2f(350.f, 150.f));
	obstacle->SetSize(sf::Vector2f(100.0f, 120.0f));

	// Render
	GameEngine::SpriteRenderComponent* render = obstacle->AddComponent<GameEngine::SpriteRenderComponent>();

	render->SetFillColor(sf::Color::Transparent);
	render->SetTexture(GameEngine::eTexture::Obstacle);

	obstacle->AddComponent<GameEngine::CollidableComponent>();

}


void GameBoard::CreateFoodButton() {
	foodbutton = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(foodbutton);
	foodbutton->SetPos(sf::Vector2f(100.0f, 300.0f));
	foodbutton->SetSize(sf::Vector2f(200.f, 200.f));
	GameEngine::SpriteRenderComponent* render = foodbutton->AddComponent<GameEngine::SpriteRenderComponent>();
	render->SetFillColor(sf::Color::Transparent);
	render->SetTexture(GameEngine::eTexture::DogFood);
}


void GameBoard::CreateCoin() {
	coinicon = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(coinicon);
	coinicon->SetPos(sf::Vector2f(100.f, 100.f));
	coinicon->SetSize(sf::Vector2f(175.f, 175.f));
	GameEngine::SpriteRenderComponent* render = static_cast<GameEngine::SpriteRenderComponent*>(coinicon->AddComponent<GameEngine::SpriteRenderComponent>());
	render->SetFillColor(sf::Color::Transparent);
	render->SetTexture(GameEngine::eTexture::Coin);
}


void GameBoard::CreateText(std::string text, int xpos, int ypos) {
	GameEngine::Entity* ptscounter = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(ptscounter);

	ptscounter->SetPos(sf::Vector2f(xpos, ypos)); 
	GameEngine::TextRenderComponent* render = ptscounter->AddComponent<GameEngine::TextRenderComponent>();
	render->SetString(text);
	render->SetFont("joystix.ttf");
	render->SetColor(sf::Color::Black);
	render->SetFillColor(sf::Color::Transparent);
	render->SetCharacterSizePixels(35);
}

void GameBoard::CreateHappinessBar() {
	happinessBar = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(happinessBar);

	happinessMeter = new GameEngine::Entity();
	GameEngine::GameEngineMain::GetInstance()->AddEntity(happinessMeter);

	happinessBar->SetPos(sf::Vector2f(750.f, 20.f));
	happinessBar->SetSize(sf::Vector2f(hapwidth, haplength));
	GameEngine::SpriteRenderComponent* render = happinessBar->AddComponent<GameEngine::SpriteRenderComponent>();

	happinessMeter->SetPos(sf::Vector2f(750.f, 20.f));
	happinessMeter->SetSize(sf::Vector2f(hapwidth, haplength));
	GameEngine::RenderComponent* meter = static_cast<GameEngine::RenderComponent*>(happinessMeter->AddComponent<GameEngine::RenderComponent>());

	render->SetFillColor(sf::Color::Transparent);
	render->SetTexture(GameEngine::eTexture::HappinessBar);
	render->SetZLevel(300);

	meter->SetFillColor(sf::Color::Green);
	render->SetZLevel(200);
}

void GameBoard::UpdateHappinessBar() {
	if (pet->GetHappiness() != pastHappiness) {
		
		float happiness = pet->GetHappiness();
		printf("%f", happiness);
		if (happiness > 0.79) {
			happinessMeter->SetPos(sf::Vector2f(750.f, 20.f));
			happinessMeter->SetSize(sf::Vector2f(hapwidth*0.8, haplength));
		}
		else if(happiness > 0.59){
			happinessMeter->SetPos(sf::Vector2f(750.f, 20.f));
			happinessMeter->SetSize(sf::Vector2f(hapwidth*0.6, haplength));
		}
		else if (happiness > 0.39) {
			happinessMeter->SetPos(sf::Vector2f(750.f, 20.f));
			happinessMeter->SetSize(sf::Vector2f(hapwidth*0.4, haplength));
		}
		else if (happiness > 0.19) {
			happinessMeter->SetPos(sf::Vector2f(750.f, 20.f));
			happinessMeter->SetSize(sf::Vector2f(hapwidth*0.2, haplength));
		}
		else if (happiness == 0) {
			happinessMeter->SetPos(sf::Vector2f(750.f, 20.f));
			happinessMeter->SetSize(sf::Vector2f(0, haplength));
		}
		pastHappiness = pet->GetHappiness();
	}
}

void GameBoard::UpdateLevel() {
	if (m_player->GetPos().y < (pet->GetPos().y - 12.f)) {
		playerRender->SetZLevel(99);
		petRender->SetZLevel(100);
	}
	else {
		playerRender->SetZLevel(100);
		petRender->SetZLevel(99);
	}
}


void GameBoard::MouseClick() {

}

void GameBoard::Update()
{	
	UpdateHappinessBar();
	UpdateLevel();
	UpdatePosition();
	UpdateMousePosition();
}

GameEngine::Entity* GameBoard::getPlayer() {
	return m_player;
}