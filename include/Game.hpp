#pragma once 

#include "Map.hpp"
#include "Player.hpp"
#include "Collision.hpp"

class Game
{
public:
    enum GameState
    {
        MENU,
        MAP,
        PLAYER_SELECTOR,
        SETTINGS
    };
public:
    void Init();
    void Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event, float deltaTime);
    void Draw(Vector2 mousePos, sf::RenderWindow &window);
    void Save();
    void Load();
private:
    void SpawnPlayer();
    void HandleMouseInput(Vector2 mousePos, Vector2 windowCenter, float deltaTime);
    void InitBackground();
public:
    Player player;
private:
    Map map;
    std::shared_ptr<sf::RectangleShape> background{std::make_shared<sf::RectangleShape>(sf::Vector2f{SCREEN_WIDTH + TILE_SIZE * 3, SCREEN_HEIGHT + TILE_SIZE * 3})};
    GameState gameState{MENU};
};